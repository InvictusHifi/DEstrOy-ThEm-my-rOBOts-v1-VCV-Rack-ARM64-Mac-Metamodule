#pragma once

#include <cstdint>

namespace dfxclock {

class ClockTracker {
public:
    void setSampleRate(float sampleRate) noexcept {
        if (sampleRate >= 8000.0f && sampleRate <= 192000.0f) sampleRate_ = sampleRate;
    }

    bool process(float volts, bool patched) noexcept {
        if (!patched) {
            if (wasPatched_) valid_ = false;
            wasPatched_ = false;
            previousHigh_ = false;
            samplesSinceEdge_ = 0u;
            return false;
        }
        wasPatched_ = true;
        const bool high = volts >= 1.0f;
        const bool rising = high && !previousHigh_;
        previousHigh_ = high;
        const auto maxSamples = static_cast<std::uint64_t>(sampleRate_ * 32.0f);
        if (samplesSinceEdge_ < maxSamples) ++samplesSinceEdge_;
        if (rising) {
            const auto measured = samplesSinceEdge_;
            if (measured >= 8u && measured <= maxSamples) {
                periodSamples_ = valid_ ? (periodSamples_ * 3u + measured) / 4u : measured;
                valid_ = true;
            }
            samplesSinceEdge_ = 0u;
        }
        return rising;
    }

    [[nodiscard]] bool valid() const noexcept { return valid_; }
    [[nodiscard]] float periodSeconds() const noexcept {
        return valid_ ? static_cast<float>(periodSamples_) / sampleRate_ : 0.0f;
    }

private:
    float sampleRate_ = 48000.0f;
    std::uint64_t samplesSinceEdge_ = 0u;
    std::uint64_t periodSamples_ = 0u;
    bool previousHigh_ = false;
    bool valid_ = false;
    bool wasPatched_ = false;
};

} // namespace dfxclock
