#include "polarizer_engine.hpp"

#include <algorithm>
#include <cmath>

namespace dfxmm {

void PolarizerEngine::setParameters(const Parameters& parameters) noexcept {
    parameters_ = parameters;
    parameters_.interval = std::clamp(parameters_.interval, 1, 512);
    parameters_.burst = std::clamp(parameters_.burst, 1, parameters_.interval);
    parameters_.probability = clampf(parameters_.probability, 0.0f, 1.0f);
    parameters_.mode = std::clamp(parameters_.mode, 0, 2);
    parameters_.stereoOffset = clampf(parameters_.stereoOffset, 0.0f, 1.0f);
    parameters_.drive = clampf(parameters_.drive, 0.0f, 1.0f);
    parameters_.mix = clampf(parameters_.mix, 0.0f, 1.0f);
}

void PolarizerEngine::triggerReset() noexcept {
    resetRequested_ = true;
}

bool PolarizerEngine::invertedAt(std::size_t phase, bool eventDecision) const noexcept {
    const auto interval = static_cast<std::size_t>(parameters_.interval);
    if (phase >= interval) {
        phase -= interval;
    }
    const bool insideBurst = phase < static_cast<std::size_t>(parameters_.burst);
    if (!insideBurst) {
        return false;
    }
    if (parameters_.mode == 0) {
        return eventDecision;
    }
    if (parameters_.mode == 1) {
        return alternate_ && eventDecision;
    }
    const std::size_t mirrored = interval - 1u - phase;
    return (phase <= mirrored) && eventDecision;
}

StereoFrame PolarizerEngine::process(float inputLeft, float inputRight) noexcept {
    if (resetRequested_) {
        counter_ = 0u;
        alternate_ = false;
        randomDecisionLeft_ = true;
        randomDecisionRight_ = true;
        resetRequested_ = false;
    }

    const auto interval = static_cast<std::size_t>(parameters_.interval);
    if (counter_ >= interval) {
        counter_ = 0u;
    }
    if (counter_ == 0u) {
        randomDecisionLeft_ = random_.uniform() <= parameters_.probability;
        randomDecisionRight_ = random_.uniform() <= parameters_.probability;
        alternate_ = !alternate_;
    }

    const auto rightOffset = static_cast<std::size_t>(parameters_.stereoOffset * static_cast<float>(interval - 1u));
    std::size_t rightPhase = counter_ + rightOffset;
    if (rightPhase >= interval) {
        rightPhase -= interval;
    }
    const bool invertLeft = invertedAt(counter_, randomDecisionLeft_);
    const bool invertRight = invertedAt(rightPhase, randomDecisionRight_);

    float wetLeft = invertLeft ? -inputLeft : inputLeft;
    float wetRight = invertRight ? -inputRight : inputRight;
    const float gain = 1.0f + (parameters_.drive * 7.0f);
    wetLeft = lerpf(wetLeft, softClip(wetLeft * gain), parameters_.drive);
    wetRight = lerpf(wetRight, softClip(wetRight * gain), parameters_.drive);

    ++counter_;
    return {
        sanitize(lerpf(inputLeft, wetLeft, parameters_.mix)),
        sanitize(lerpf(inputRight, wetRight, parameters_.mix)),
    };
}

} // namespace dfxmm
