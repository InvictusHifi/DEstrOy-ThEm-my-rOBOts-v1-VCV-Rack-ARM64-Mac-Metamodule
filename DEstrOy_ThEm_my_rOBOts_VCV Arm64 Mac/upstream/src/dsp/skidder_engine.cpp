#include "skidder_engine.hpp"

#include <algorithm>
#include <cmath>

namespace dfxmm {

void SkidderEngine::setSampleRate(float sampleRate) noexcept {
    sampleRate_ = clampf(sampleRate, 8000.0f, 192000.0f);
}

void SkidderEngine::setParameters(const Parameters& parameters) noexcept {
    parameters_ = parameters;
    parameters_.rateHz = clampf(parameters_.rateHz, 0.02f, 80.0f);
    parameters_.pulseWidth = clampf(parameters_.pulseWidth, 0.01f, 0.99f);
    parameters_.slope = clampf(parameters_.slope, 0.0f, 1.0f);
    parameters_.floor = clampf(parameters_.floor, 0.0f, 1.0f);
    parameters_.randomAmount = clampf(parameters_.randomAmount, 0.0f, 1.0f);
    parameters_.stereoOffset = clampf(parameters_.stereoOffset, 0.0f, 1.0f);
    parameters_.clockRatio = clampf(parameters_.clockRatio, 0.125f, 8.0f);
    parameters_.mix = clampf(parameters_.mix, 0.0f, 1.0f);
}

void SkidderEngine::triggerReset() noexcept {
    resetRequested_ = true;
}

void SkidderEngine::clockPulse() noexcept {
    if (samplesSinceClock_ > 8u) {
        clockPeriodSamples_ = samplesSinceClock_;
        haveClock_ = true;
    }
    samplesSinceClock_ = 0u;
    if (parameters_.clockLock) {
        resetRequested_ = true;
    }
}

void SkidderEngine::beginCycle() noexcept {
    const float random = random_.bipolar() * parameters_.randomAmount;
    cycleRateHz_ = parameters_.rateHz * std::exp2(random * 2.0f);
    cycleRateHz_ = clampf(cycleRateHz_, 0.02f, 80.0f);
    cycleWidth_ = clampf(parameters_.pulseWidth + (random_.bipolar() * parameters_.randomAmount * 0.35f), 0.01f, 0.99f);
}

float SkidderEngine::envelopeAt(float phase) const noexcept {
    phase -= std::floor(phase);
    const float width = cycleWidth_;
    if (phase >= width) {
        return parameters_.floor;
    }
    const float maximumSlope = std::max(0.00001f, width * 0.5f);
    const float slopeLength = std::max(0.00001f, parameters_.slope * maximumSlope);
    float envelope = 1.0f;
    if (phase < slopeLength) {
        envelope = phase / slopeLength;
    }
    const float tail = width - phase;
    if (tail < slopeLength) {
        envelope = std::min(envelope, tail / slopeLength);
    }
    envelope = clampf(envelope, 0.0f, 1.0f);
    return lerpf(parameters_.floor, 1.0f, envelope);
}

StereoFrame SkidderEngine::process(float inputLeft, float inputRight) noexcept {
    ++samplesSinceClock_;
    if (resetRequested_) {
        phase_ = 0.0f;
        beginCycle();
        resetRequested_ = false;
    }

    float increment = cycleRateHz_ / sampleRate_;
    if (parameters_.clockLock && haveClock_) {
        const float period = static_cast<float>(clockPeriodSamples_) / parameters_.clockRatio;
        increment = 1.0f / std::max(1.0f, period);
    }

    const float leftEnvelope = envelopeAt(phase_);
    const float rightEnvelope = envelopeAt(phase_ + (parameters_.stereoOffset * 0.5f));
    const StereoFrame wet{inputLeft * leftEnvelope, inputRight * rightEnvelope};

    phase_ += increment;
    if (phase_ >= 1.0f) {
        phase_ -= std::floor(phase_);
        beginCycle();
    }

    return {
        sanitize(lerpf(inputLeft, wet.left, parameters_.mix)),
        sanitize(lerpf(inputRight, wet.right, parameters_.mix)),
    };
}

} // namespace dfxmm
