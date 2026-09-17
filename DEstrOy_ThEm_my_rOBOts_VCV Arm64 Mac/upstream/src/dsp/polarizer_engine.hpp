#pragma once

#include "common_dsp.hpp"

#include <cstddef>

namespace dfxmm {

class PolarizerEngine {
public:
    struct Parameters {
        int interval = 8;
        int burst = 1;
        float probability = 1.0f;
        int mode = 0;
        float stereoOffset = 0.0f;
        float drive = 0.0f;
        float mix = 1.0f;
    };

    void setParameters(const Parameters& parameters) noexcept;
    void triggerReset() noexcept;
    StereoFrame process(float inputLeft, float inputRight) noexcept;

private:
    bool invertedAt(std::size_t phase, bool eventDecision) const noexcept;

    Parameters parameters_{};
    std::size_t counter_ = 0u;
    bool randomDecisionLeft_ = true;
    bool randomDecisionRight_ = true;
    bool alternate_ = false;
    bool resetRequested_ = false;
    XorShift32 random_{0x706f6c61u};
};

} // namespace dfxmm
