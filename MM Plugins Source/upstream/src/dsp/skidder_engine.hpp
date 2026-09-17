#pragma once

#include "common_dsp.hpp"

#include <cstddef>

namespace dfxmm {

class SkidderEngine {
public:
    struct Parameters {
        float rateHz = 4.0f;
        float pulseWidth = 0.5f;
        float slope = 0.15f;
        float floor = 0.0f;
        float randomAmount = 0.0f;
        float stereoOffset = 0.0f;
        float clockRatio = 1.0f;
        float mix = 1.0f;
        bool clockLock = false;
    };

    void setSampleRate(float sampleRate) noexcept;
    void setParameters(const Parameters& parameters) noexcept;
    void triggerReset() noexcept;
    void clockPulse() noexcept;
    StereoFrame process(float inputLeft, float inputRight) noexcept;

private:
    float envelopeAt(float phase) const noexcept;
    void beginCycle() noexcept;

    Parameters parameters_{};
    float sampleRate_ = 48000.0f;
    float phase_ = 0.0f;
    float cycleRateHz_ = 4.0f;
    float cycleWidth_ = 0.5f;
    std::size_t samplesSinceClock_ = 0u;
    std::size_t clockPeriodSamples_ = 0u;
    bool haveClock_ = false;
    bool resetRequested_ = false;
    XorShift32 random_{0x736b6964u};
};

} // namespace dfxmm
