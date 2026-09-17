#include "buffer_override_engine.hpp"

#include <algorithm>
#include <cmath>

namespace dfxmm {

BufferOverrideEngine::BufferOverrideEngine()
    : leftBuffer_(kMaximumSamples, 0.0f),
      rightBuffer_(kMaximumSamples, 0.0f) {
}

void BufferOverrideEngine::setSampleRate(float sampleRate) noexcept {
    sampleRate_ = clampf(sampleRate, 8000.0f, 192000.0f);
}

void BufferOverrideEngine::setParameters(const Parameters& parameters) noexcept {
    parameters_ = parameters;
    parameters_.bufferSeconds = clampf(parameters_.bufferSeconds, 0.005f, 10.0f);
    parameters_.divisor = std::clamp(parameters_.divisor, 1, 64);
    parameters_.duty = clampf(parameters_.duty, 0.0f, 1.0f);
    parameters_.smooth = clampf(parameters_.smooth, 0.0f, 1.0f);
    parameters_.decay = clampf(parameters_.decay, 0.0f, 1.0f);
    parameters_.randomSize = clampf(parameters_.randomSize, 0.0f, 1.0f);
    parameters_.reverseProbability = clampf(parameters_.reverseProbability, 0.0f, 1.0f);
    parameters_.feedback = clampf(parameters_.feedback, 0.0f, 0.97f);
    parameters_.mix = clampf(parameters_.mix, 0.0f, 1.0f);
    parameters_.clockRatio = clampf(parameters_.clockRatio, 0.125f, 8.0f);
}

void BufferOverrideEngine::triggerCapture() noexcept {
    captureRequested_ = true;
}

void BufferOverrideEngine::clockPulse() noexcept {
    if (samplesSinceClock_ > 8u) {
        clockPeriodSamples_ = samplesSinceClock_;
        haveClock_ = true;
    }
    samplesSinceClock_ = 0u;
    if (parameters_.clockLock) {
        captureRequested_ = true;
    }
}

std::size_t BufferOverrideEngine::currentCaptureLength() const noexcept {
    std::size_t length = static_cast<std::size_t>(parameters_.bufferSeconds * sampleRate_);
    if (parameters_.clockLock && haveClock_) {
        length = static_cast<std::size_t>(static_cast<float>(clockPeriodSamples_) * parameters_.clockRatio);
    }
    return std::clamp<std::size_t>(length, 32u, kMaximumSamples - 4u);
}

void BufferOverrideEngine::refreshMiniBuffer() noexcept {
    const auto captureLength = currentCaptureLength();
    const float randomScale = 1.0f + (random_.bipolar() * parameters_.randomSize * 0.45f);
    const float nominal = static_cast<float>(captureLength) / static_cast<float>(parameters_.divisor);
    miniSize_ = std::clamp<std::size_t>(static_cast<std::size_t>(nominal * randomScale), 8u, captureLength);
    reverse_ = random_.uniform() < parameters_.reverseProbability;
    readPosition_ = 0u;
}

void BufferOverrideEngine::refreshCapture(bool forced) noexcept {
    const auto captureLength = currentCaptureLength();
    if (!forced && captureCounter_ < captureLength) {
        return;
    }
    basePosition_ = (writePosition_ + kMaximumSamples - captureLength) % kMaximumSamples;
    captureCounter_ = 0u;
    repeatCount_ = 0u;
    repeatGain_ = 1.0f;
    refreshMiniBuffer();
}

float BufferOverrideEngine::edgeEnvelope(std::size_t position) const noexcept {
    if (miniSize_ < 4u) {
        return 1.0f;
    }
    const std::size_t audibleLength = std::max<std::size_t>(1u, static_cast<std::size_t>(parameters_.duty * static_cast<float>(miniSize_)));
    if (position >= audibleLength) {
        return 0.0f;
    }
    const std::size_t maxFade = std::max<std::size_t>(1u, std::min(miniSize_ / 2u, audibleLength / 2u));
    const std::size_t fadeLength = std::max<std::size_t>(1u, static_cast<std::size_t>(parameters_.smooth * static_cast<float>(maxFade)));
    float envelope = 1.0f;
    if (position < fadeLength) {
        envelope = static_cast<float>(position) / static_cast<float>(fadeLength);
    }
    const std::size_t remaining = audibleLength - position;
    if (remaining < fadeLength) {
        envelope = std::min(envelope, static_cast<float>(remaining) / static_cast<float>(fadeLength));
    }
    return clampf(envelope, 0.0f, 1.0f);
}

StereoFrame BufferOverrideEngine::process(float inputLeft, float inputRight) noexcept {
    ++samplesSinceClock_;
    ++captureCounter_;

    if (captureRequested_) {
        refreshCapture(true);
        captureRequested_ = false;
    } else {
        refreshCapture(false);
    }

    if (!parameters_.freeze) {
        leftBuffer_[writePosition_] = softClip(inputLeft + (previousWet_.left * parameters_.feedback));
        rightBuffer_[writePosition_] = softClip(inputRight + (previousWet_.right * parameters_.feedback));
    }

    const std::size_t phase = reverse_ ? (miniSize_ - 1u - readPosition_) : readPosition_;
    const std::size_t index = (basePosition_ + phase) % kMaximumSamples;
    const float envelope = edgeEnvelope(readPosition_);
    StereoFrame wet{
        leftBuffer_[index] * envelope * repeatGain_,
        rightBuffer_[index] * envelope * repeatGain_,
    };
    wet.left = softClip(wet.left * 1.25f);
    wet.right = softClip(wet.right * 1.25f);
    previousWet_ = wet;

    writePosition_ = (writePosition_ + 1u) % kMaximumSamples;
    ++readPosition_;
    if (readPosition_ >= miniSize_) {
        ++repeatCount_;
        repeatGain_ *= parameters_.decay;
        refreshMiniBuffer();
    }

    return {
        sanitize(lerpf(inputLeft, wet.left, parameters_.mix)),
        sanitize(lerpf(inputRight, wet.right, parameters_.mix)),
    };
}

} // namespace dfxmm
