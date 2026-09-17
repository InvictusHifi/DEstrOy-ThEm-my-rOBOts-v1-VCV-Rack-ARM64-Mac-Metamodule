#pragma once

#include "common_dsp.hpp"

#include <cstddef>
#include <vector>

namespace dfxmm {

class TransverbEngine {
public:
    static constexpr std::size_t kMaximumSamples = 1u << 20u;

    struct Parameters {
        float bufferSeconds = 2.0f;
        float speedA = 0.5f;
        float delayA = 0.25f;
        float levelA = 0.75f;
        float speedB = -0.5f;
        float delayB = 0.65f;
        float levelB = 0.75f;
        float feedback = 0.25f;
        float crossFeedback = 0.0f;
        float tomsound = 0.0f;
        float spread = 0.5f;
        float mix = 1.0f;
        bool freeze = false;
    };

    TransverbEngine();
    void setSampleRate(float sampleRate) noexcept;
    void setParameters(const Parameters& parameters) noexcept;
    void triggerReset() noexcept;
    StereoFrame process(float inputLeft, float inputRight) noexcept;

private:
    StereoFrame readHead(double position) const noexcept;
    void updateLoopSize() noexcept;

    std::vector<float> leftBuffer_;
    std::vector<float> rightBuffer_;
    Parameters parameters_{};
    float sampleRate_ = 48000.0f;
    std::size_t loopSize_ = 96000u;
    std::size_t writePosition_ = 0u;
    double transportA_ = 0.0;
    double transportB_ = 0.0;
    bool resetRequested_ = true;
    std::uint32_t glitchCounter_ = 0u;
    std::uint32_t glitchRemaining_ = 0u;
    float glitchOffsetA_ = 0.0f;
    float glitchOffsetB_ = 0.0f;
    StereoFrame previousWet_{};
    XorShift32 random_{0x7472616eu};
};

} // namespace dfxmm
