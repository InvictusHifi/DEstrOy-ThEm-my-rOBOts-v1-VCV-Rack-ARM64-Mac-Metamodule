#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace dfxmm {

struct StereoFrame {
    float left = 0.0f;
    float right = 0.0f;
};

float clampf(float value, float minimum, float maximum) noexcept;
float lerpf(float a, float b, float amount) noexcept;
float softClip(float value) noexcept;
float sanitize(float value) noexcept;
double wrapPosition(double position, std::size_t size) noexcept;
float nearestRead(const std::vector<float>& buffer, double position, std::size_t size) noexcept;
float linearRead(const std::vector<float>& buffer, double position, std::size_t size) noexcept;
float hermiteRead(const std::vector<float>& buffer, double position, std::size_t size) noexcept;

class XorShift32 {
public:
    explicit XorShift32(std::uint32_t seed = 0x6d2b79f5u) noexcept;
    std::uint32_t nextU32() noexcept;
    float uniform() noexcept;
    float bipolar() noexcept;

private:
    std::uint32_t state_;
};

class RisingEdge {
public:
    bool process(bool high) noexcept;
    void reset() noexcept;

private:
    bool previous_ = false;
};

} // namespace dfxmm
