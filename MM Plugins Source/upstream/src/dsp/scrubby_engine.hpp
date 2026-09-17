#pragma once

#include "common_dsp.hpp"

#include <cstddef>
#include <vector>

namespace dfxmm {

class ScrubbyEngine {
public:
    static constexpr std::size_t kMaximumSamples = 1u << 20u;

    struct Parameters {
        float bufferSeconds = 2.0f;
        float speedRatio = 1.0f;
        float seek = 0.5f;
        float range = 0.5f;
        float jumpRateHz = 1.0f;
        float jumpAmount = 0.5f;
        float inertia = 0.35f;
        int quantizeMode = 0;
        float reverseProbability = 0.0f;
        float feedback = 0.0f;
        float spread = 0.25f;
        float mix = 1.0f;
        bool freeze = false;
    };

    ScrubbyEngine();
    void setSampleRate(float sampleRate) noexcept;
    void setParameters(const Parameters& parameters) noexcept;
    void triggerJump() noexcept;
    void triggerReset() noexcept;
    StereoFrame process(float inputLeft, float inputRight) noexcept;

private:
    float quantizedSpeed(float speed) const noexcept;
    void scheduleJump(bool forced) noexcept;
    StereoFrame readAt(double position) const noexcept;
    void updateLoopSize() noexcept;

    std::vector<float> leftBuffer_;
    std::vector<float> rightBuffer_;
    Parameters parameters_{};
    float sampleRate_ = 48000.0f;
    std::size_t loopSize_ = 96000u;
    std::size_t writePosition_ = 0u;
    double readPosition_ = 0.0;
    double oldReadPosition_ = 0.0;
    float currentSpeed_ = 1.0f;
    float direction_ = 1.0f;
    float crossfade_ = 1.0f;
    float crossfadeStep_ = 1.0f;
    float jumpPhase_ = 0.0f;
    float lastSeek_ = 0.5f;
    std::uint32_t controlCounter_ = 0u;
    bool jumpRequested_ = true;
    bool resetRequested_ = true;
    StereoFrame previousWet_{};
    XorShift32 random_{0x73637275u};
};

} // namespace dfxmm
