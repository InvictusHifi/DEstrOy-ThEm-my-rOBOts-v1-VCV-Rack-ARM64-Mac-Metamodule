#include "scrubby_engine.hpp"

#include <algorithm>
#include <cmath>

namespace dfxmm {

ScrubbyEngine::ScrubbyEngine()
    : leftBuffer_(kMaximumSamples, 0.0f),
      rightBuffer_(kMaximumSamples, 0.0f) {
}

void ScrubbyEngine::setSampleRate(float sampleRate) noexcept {
    sampleRate_ = clampf(sampleRate, 8000.0f, 192000.0f);
    updateLoopSize();
}

void ScrubbyEngine::setParameters(const Parameters& parameters) noexcept {
    parameters_ = parameters;
    parameters_.bufferSeconds = clampf(parameters_.bufferSeconds, 0.02f, 10.0f);
    parameters_.speedRatio = clampf(parameters_.speedRatio, -8.0f, 8.0f);
    parameters_.seek = clampf(parameters_.seek, 0.0f, 1.0f);
    parameters_.range = clampf(parameters_.range, 0.0f, 1.0f);
    parameters_.jumpRateHz = clampf(parameters_.jumpRateHz, 0.0f, 40.0f);
    parameters_.jumpAmount = clampf(parameters_.jumpAmount, 0.0f, 1.0f);
    parameters_.inertia = clampf(parameters_.inertia, 0.0f, 1.0f);
    parameters_.quantizeMode = std::clamp(parameters_.quantizeMode, 0, 3);
    parameters_.reverseProbability = clampf(parameters_.reverseProbability, 0.0f, 1.0f);
    parameters_.feedback = clampf(parameters_.feedback, 0.0f, 0.97f);
    parameters_.spread = clampf(parameters_.spread, 0.0f, 1.0f);
    parameters_.mix = clampf(parameters_.mix, 0.0f, 1.0f);
    updateLoopSize();
}

void ScrubbyEngine::triggerJump() noexcept {
    jumpRequested_ = true;
}

void ScrubbyEngine::triggerReset() noexcept {
    resetRequested_ = true;
}

void ScrubbyEngine::updateLoopSize() noexcept {
    const auto requested = static_cast<std::size_t>(parameters_.bufferSeconds * sampleRate_);
    const auto newSize = std::clamp<std::size_t>(requested, 64u, kMaximumSamples - 4u);
    if (newSize == loopSize_) {
        return;
    }
    loopSize_ = newSize;
    writePosition_ %= loopSize_;
    readPosition_ = wrapPosition(readPosition_, loopSize_);
    oldReadPosition_ = wrapPosition(oldReadPosition_, loopSize_);
}

float ScrubbyEngine::quantizedSpeed(float speed) const noexcept {
    const float sign = speed < 0.0f ? -1.0f : 1.0f;
    float magnitude = std::max(std::fabs(speed), 0.03125f);
    if (parameters_.quantizeMode == 0) {
        return speed;
    }
    float semitones = std::log2(magnitude) * 12.0f;
    if (parameters_.quantizeMode == 1) {
        semitones = std::round(semitones);
    } else if (parameters_.quantizeMode == 2) {
        semitones = std::round(semitones / 12.0f) * 12.0f;
    } else {
        semitones = std::round(semitones / 7.0f) * 7.0f;
    }
    magnitude = std::exp2(semitones / 12.0f);
    return sign * clampf(magnitude, 0.03125f, 8.0f);
}

StereoFrame ScrubbyEngine::readAt(double position) const noexcept {
    return {
        hermiteRead(leftBuffer_, position, loopSize_),
        hermiteRead(rightBuffer_, position, loopSize_),
    };
}

void ScrubbyEngine::scheduleJump(bool forced) noexcept {
    if (!forced && parameters_.jumpAmount <= 0.0001f) {
        return;
    }
    oldReadPosition_ = readPosition_;
    const float randomOffset = random_.bipolar() * parameters_.range * parameters_.jumpAmount;
    const float normalizedDelay = clampf(parameters_.seek + randomOffset, 0.002f, 0.998f);
    readPosition_ = wrapPosition(
        static_cast<double>(writePosition_) - (static_cast<double>(normalizedDelay) * static_cast<double>(loopSize_)),
        loopSize_);
    direction_ = (random_.uniform() < parameters_.reverseProbability) ? -1.0f : 1.0f;
    crossfade_ = 0.0f;
    const float fadeSeconds = 0.0007f + (parameters_.inertia * parameters_.inertia * 0.075f);
    crossfadeStep_ = 1.0f / std::max(1.0f, fadeSeconds * sampleRate_);
}

StereoFrame ScrubbyEngine::process(float inputLeft, float inputRight) noexcept {
    if (resetRequested_) {
        readPosition_ = wrapPosition(
            static_cast<double>(writePosition_) - static_cast<double>(parameters_.seek * static_cast<float>(loopSize_)),
            loopSize_);
        oldReadPosition_ = readPosition_;
        currentSpeed_ = parameters_.speedRatio;
        crossfade_ = 1.0f;
        resetRequested_ = false;
    }

    ++controlCounter_;
    if (controlCounter_ >= 64u) {
        controlCounter_ = 0u;
        if (std::fabs(parameters_.seek - lastSeek_) > 0.02f) {
            jumpRequested_ = true;
            lastSeek_ = parameters_.seek;
        }
    }

    jumpPhase_ += parameters_.jumpRateHz / sampleRate_;
    if (jumpPhase_ >= 1.0f) {
        jumpPhase_ -= std::floor(jumpPhase_);
        jumpRequested_ = true;
    }
    if (jumpRequested_) {
        scheduleJump(true);
        jumpRequested_ = false;
    }

    const float targetSpeed = quantizedSpeed(parameters_.speedRatio) * direction_;
    const float slew = 0.0005f + ((1.0f - parameters_.inertia) * 0.08f);
    currentSpeed_ += (targetSpeed - currentSpeed_) * slew;

    StereoFrame wet = readAt(readPosition_);
    if (crossfade_ < 1.0f) {
        const auto oldWet = readAt(oldReadPosition_);
        wet.left = lerpf(oldWet.left, wet.left, crossfade_);
        wet.right = lerpf(oldWet.right, wet.right, crossfade_);
        crossfade_ = std::min(1.0f, crossfade_ + crossfadeStep_);
        oldReadPosition_ = wrapPosition(oldReadPosition_ + static_cast<double>(currentSpeed_), loopSize_);
    }

    const float mid = 0.5f * (wet.left + wet.right);
    const float side = 0.5f * (wet.left - wet.right) * (1.0f + parameters_.spread);
    wet.left = softClip(mid + side);
    wet.right = softClip(mid - side);

    if (!parameters_.freeze) {
        leftBuffer_[writePosition_] = softClip(inputLeft + (previousWet_.left * parameters_.feedback));
        rightBuffer_[writePosition_] = softClip(inputRight + (previousWet_.right * parameters_.feedback));
    }

    previousWet_ = wet;
    ++writePosition_;
    if (writePosition_ >= loopSize_) {
        writePosition_ = 0u;
    }
    readPosition_ = wrapPosition(readPosition_ + static_cast<double>(currentSpeed_), loopSize_);

    return {
        sanitize(lerpf(inputLeft, wet.left, parameters_.mix)),
        sanitize(lerpf(inputRight, wet.right, parameters_.mix)),
    };
}

} // namespace dfxmm
