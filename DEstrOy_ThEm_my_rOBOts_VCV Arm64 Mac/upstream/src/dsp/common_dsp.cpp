#include "common_dsp.hpp"

#include <limits>

namespace dfxmm {

float clampf(float value, float minimum, float maximum) noexcept {
    return std::max(minimum, std::min(value, maximum));
}

float lerpf(float a, float b, float amount) noexcept {
    return a + ((b - a) * amount);
}

float softClip(float value) noexcept {
    if (!std::isfinite(value)) {
        return 0.0f;
    }
    const float x = clampf(value, -8.0f, 8.0f);
    return x / (1.0f + std::fabs(x));
}

float sanitize(float value) noexcept {
    if (!std::isfinite(value) || std::fabs(value) < std::numeric_limits<float>::min()) {
        return 0.0f;
    }
    return value;
}

double wrapPosition(double position, std::size_t size) noexcept {
    if (size == 0u || !std::isfinite(position)) {
        return 0.0;
    }
    const double span = static_cast<double>(size);
    if (position >= 0.0 && position < span) {
        return position;
    }
    if (position < 0.0 && position >= -span) {
        return position + span;
    }
    if (position >= span && position < (span * 2.0)) {
        return position - span;
    }
    position -= std::floor(position / span) * span;
    if (position >= span) {
        position -= span;
    } else if (position < 0.0) {
        position += span;
    }
    return position;
}

float nearestRead(const std::vector<float>& buffer, double position, std::size_t size) noexcept {
    if (size == 0u || buffer.empty()) {
        return 0.0f;
    }
    const auto index = static_cast<std::size_t>(wrapPosition(std::round(position), size));
    return buffer[index];
}

float linearRead(const std::vector<float>& buffer, double position, std::size_t size) noexcept {
    if (size < 2u || buffer.empty()) {
        return 0.0f;
    }
    const double wrapped = wrapPosition(position, size);
    const auto index0 = static_cast<std::size_t>(wrapped);
    const auto next = index0 + 1u;
    const auto index1 = next < size ? next : 0u;
    const float fraction = static_cast<float>(wrapped - static_cast<double>(index0));
    return lerpf(buffer[index0], buffer[index1], fraction);
}

float hermiteRead(const std::vector<float>& buffer, double position, std::size_t size) noexcept {
    if (size < 4u || buffer.empty()) {
        return linearRead(buffer, position, size);
    }
    const double wrapped = wrapPosition(position, size);
    const auto i1 = static_cast<std::size_t>(wrapped);
    const auto i0 = i1 == 0u ? size - 1u : i1 - 1u;
    const auto next = i1 + 1u;
    const auto i2 = next < size ? next : 0u;
    const auto next2 = i2 + 1u;
    const auto i3 = next2 < size ? next2 : 0u;
    const float t = static_cast<float>(wrapped - static_cast<double>(i1));

    const float y0 = buffer[i0];
    const float y1 = buffer[i1];
    const float y2 = buffer[i2];
    const float y3 = buffer[i3];

    const float c0 = y1;
    const float c1 = 0.5f * (y2 - y0);
    const float c2 = y0 - (2.5f * y1) + (2.0f * y2) - (0.5f * y3);
    const float c3 = (0.5f * (y3 - y0)) + (1.5f * (y1 - y2));
    return (((c3 * t) + c2) * t + c1) * t + c0;
}

XorShift32::XorShift32(std::uint32_t seed) noexcept
    : state_(seed == 0u ? 0x6d2b79f5u : seed) {
}

std::uint32_t XorShift32::nextU32() noexcept {
    std::uint32_t x = state_;
    x ^= x << 13u;
    x ^= x >> 17u;
    x ^= x << 5u;
    state_ = x;
    return x;
}

float XorShift32::uniform() noexcept {
    return static_cast<float>(nextU32() >> 8u) * (1.0f / 16777216.0f);
}

float XorShift32::bipolar() noexcept {
    return (uniform() * 2.0f) - 1.0f;
}

bool RisingEdge::process(bool high) noexcept {
    const bool result = high && !previous_;
    previous_ = high;
    return result;
}

void RisingEdge::reset() noexcept {
    previous_ = false;
}

} // namespace dfxmm
