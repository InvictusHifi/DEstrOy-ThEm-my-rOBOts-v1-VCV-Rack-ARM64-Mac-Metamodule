#include "transverb_engine.hpp"

#include <algorithm>
#include <cmath>

namespace dfxmm {

TransverbEngine::TransverbEngine()
    : leftBuffer_(kMaximumSamples, 0.0f),
      rightBuffer_(kMaximumSamples, 0.0f) {
}

void TransverbEngine::setSampleRate(float sampleRate) noexcept {
    sampleRate_ = clampf(sampleRate, 8000.0f, 192000.0f);
    updateLoopSize();
}

void TransverbEngine::setParameters(const Parameters& parameters) noexcept {
    parameters_ = parameters;
    parameters_.bufferSeconds = clampf(parameters_.bufferSeconds, 0.01f, 10.0f);
    parameters_.speedA = clampf(parameters_.speedA, -8.0f, 8.0f);
    parameters_.speedB = clampf(parameters_.speedB, -8.0f, 8.0f);
    parameters_.delayA = clampf(parameters_.delayA, 0.0f, 0.999f);
    parameters_.delayB = clampf(parameters_.delayB, 0.0f, 0.999f);
    parameters_.levelA = clampf(parameters_.levelA, 0.0f, 1.5f);
    parameters_.levelB = clampf(parameters_.levelB, 0.0f, 1.5f);
    parameters_.feedback = clampf(parameters_.feedback, 0.0f, 0.985f);
    parameters_.crossFeedback = clampf(parameters_.crossFeedback, 0.0f, 1.0f);
    parameters_.tomsound = clampf(parameters_.tomsound, 0.0f, 1.0f);
    parameters_.spread = clampf(parameters_.spread, 0.0f, 1.0f);
    parameters_.mix = clampf(parameters_.mix, 0.0f, 1.0f);
    updateLoopSize();
}

void TransverbEngine::triggerReset() noexcept {
    resetRequested_ = true;
}

void TransverbEngine::updateLoopSize() noexcept {
    const auto requested = static_cast<std::size_t>(parameters_.bufferSeconds * sampleRate_);
    const auto newSize = std::clamp<std::size_t>(requested, 64u, kMaximumSamples - 4u);
    if (newSize == loopSize_) {
        return;
    }
    loopSize_ = newSize;
    writePosition_ %= loopSize_;
    transportA_ = wrapPosition(transportA_, loopSize_);
    transportB_ = wrapPosition(transportB_, loopSize_);
}

StereoFrame TransverbEngine::readHead(double position) const noexcept {
    const float roughness = parameters_.tomsound * 0.72f;
    if (roughness <= 0.0001f) {
        return {
            hermiteRead(leftBuffer_, position, loopSize_),
            hermiteRead(rightBuffer_, position, loopSize_),
        };
    }
    const float cleanLeft = hermiteRead(leftBuffer_, position, loopSize_);
    const float cleanRight = hermiteRead(rightBuffer_, position, loopSize_);
    const float roughLeft = nearestRead(leftBuffer_, position, loopSize_);
    const float roughRight = nearestRead(rightBuffer_, position, loopSize_);
    return {
        lerpf(cleanLeft, roughLeft, roughness),
        lerpf(cleanRight, roughRight, roughness),
    };
}

StereoFrame TransverbEngine::process(float inputLeft, float inputRight) noexcept {
    if (resetRequested_) {
        transportA_ = static_cast<double>(writePosition_);
        transportB_ = static_cast<double>(writePosition_);
        glitchRemaining_ = 0u;
        resetRequested_ = false;
    }

    ++glitchCounter_;
    if (glitchCounter_ >= 256u) {
        glitchCounter_ = 0u;
        const float probability = parameters_.tomsound * parameters_.tomsound * 0.45f;
        if (random_.uniform() < probability) {
            const float maximumJump = static_cast<float>(loopSize_) * (0.002f + (0.12f * parameters_.tomsound));
            glitchOffsetA_ = random_.bipolar() * maximumJump;
            glitchOffsetB_ = random_.bipolar() * maximumJump;
            glitchRemaining_ = 64u + static_cast<std::uint32_t>(random_.uniform() * 448.0f);
        }
    }
    if (glitchRemaining_ > 0u) {
        --glitchRemaining_;
    } else {
        glitchOffsetA_ *= 0.92f;
        glitchOffsetB_ *= 0.92f;
    }

    const double delaySamplesA = static_cast<double>(parameters_.delayA) * static_cast<double>(loopSize_ - 4u);
    const double delaySamplesB = static_cast<double>(parameters_.delayB) * static_cast<double>(loopSize_ - 4u);
    const auto headA = readHead(transportA_ - delaySamplesA + static_cast<double>(glitchOffsetA_));
    const auto headB = readHead(transportB_ - delaySamplesB + static_cast<double>(glitchOffsetB_));

    const float aLeft = headA.left * parameters_.levelA;
    const float aRight = headA.right * parameters_.levelA;
    const float bLeft = headB.left * parameters_.levelB;
    const float bRight = headB.right * parameters_.levelB;

    const StereoFrame centered{0.5f * (aLeft + bLeft), 0.5f * (aRight + bRight)};
    const StereoFrame separated{aLeft, bRight};
    StereoFrame wet{
        lerpf(centered.left, separated.left, parameters_.spread),
        lerpf(centered.right, separated.right, parameters_.spread),
    };
    wet.left = softClip(wet.left * 1.35f);
    wet.right = softClip(wet.right * 1.35f);

    if (!parameters_.freeze) {
        const float cross = parameters_.crossFeedback;
        const float feedbackLeft = lerpf(previousWet_.left, previousWet_.right, cross) * parameters_.feedback;
        const float feedbackRight = lerpf(previousWet_.right, previousWet_.left, cross) * parameters_.feedback;
        leftBuffer_[writePosition_] = softClip(inputLeft + feedbackLeft);
        rightBuffer_[writePosition_] = softClip(inputRight + feedbackRight);
    }

    previousWet_ = wet;
    ++writePosition_;
    if (writePosition_ >= loopSize_) {
        writePosition_ = 0u;
    }
    transportA_ = wrapPosition(transportA_ + static_cast<double>(parameters_.speedA), loopSize_);
    transportB_ = wrapPosition(transportB_ + static_cast<double>(parameters_.speedB), loopSize_);

    StereoFrame output;
    output.left = sanitize(lerpf(inputLeft, wet.left, parameters_.mix));
    output.right = sanitize(lerpf(inputRight, wet.right, parameters_.mix));
    return output;
}

} // namespace dfxmm
