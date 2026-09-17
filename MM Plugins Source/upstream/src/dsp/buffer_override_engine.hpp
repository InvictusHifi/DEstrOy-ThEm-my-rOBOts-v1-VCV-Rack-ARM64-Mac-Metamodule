#pragma once

#include "common_dsp.hpp"

#include <cstddef>
#include <vector>

namespace dfxmm {

class BufferOverrideEngine {
public:
    static constexpr std::size_t kMaximumSamples = 1u << 20u;

    struct Parameters {
        float bufferSeconds = 0.5f;
        int divisor = 8;
        float duty = 1.0f;
        float smooth = 0.12f;
        float decay = 1.0f;
        float randomSize = 0.0f;
        float reverseProbability = 0.0f;
        float feedback = 0.0f;
        float mix = 1.0f;
        float clockRatio = 1.0f;
        bool clockLock = false;
        bool freeze = false;
    };

    BufferOverrideEngine();
    void setSampleRate(float sampleRate) noexcept;
    void setParameters(const Parameters& parameters) noexcept;
    void triggerCapture() noexcept;
    void clockPulse() noexcept;
    StereoFrame process(float inputLeft, float inputRight) noexcept;

private:
    void refreshCapture(bool forced) noexcept;
    void refreshMiniBuffer() noexcept;
    std::size_t currentCaptureLength() const noexcept;
    float edgeEnvelope(std::size_t position) const noexcept;

    std::vector<float> leftBuffer_;
    std::vector<float> rightBuffer_;
    Parameters parameters_{};
    float sampleRate_ = 48000.0f;
    std::size_t writePosition_ = 0u;
    std::size_t basePosition_ = 0u;
    std::size_t miniSize_ = 1024u;
    std::size_t readPosition_ = 0u;
    std::size_t captureCounter_ = 0u;
    std::size_t repeatCount_ = 0u;
    float repeatGain_ = 1.0f;
    bool reverse_ = false;
    bool captureRequested_ = true;
    std::size_t samplesSinceClock_ = 0u;
    std::size_t clockPeriodSamples_ = 0u;
    bool haveClock_ = false;
    StereoFrame previousWet_{};
    XorShift32 random_{0x62756666u};
};

} // namespace dfxmm
