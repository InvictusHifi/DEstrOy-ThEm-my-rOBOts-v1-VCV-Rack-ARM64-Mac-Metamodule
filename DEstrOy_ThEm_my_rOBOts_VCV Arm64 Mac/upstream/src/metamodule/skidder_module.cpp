#include "skidder_module.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace {
constexpr float kAudioInputScale = 0.2f;
constexpr float kAudioOutputScale = 5.0f;
constexpr float kCvFullScale = 5.0f;
constexpr float kGateThreshold = 1.0f;
constexpr std::array<float, 7> kClockRatios{{0.125f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f, 8.0f}};
}

SkidderModule::SkidderModule() {
    params_.fill(0.0f); inputs_.fill(0.0f); inputPatched_.fill(false); outputs_.fill(0.0f);
    params_[RateParam] = 0.32f;
    params_[WidthParam] = 0.5f;
    params_[SlopeParam] = 0.15f;
    params_[FloorParam] = 0.0f;
    params_[RandomParam] = 0.0f;
    params_[StereoParam] = 0.0f;
    params_[ClockRatioParam] = 0.5f;
    params_[MixParam] = 1.0f;
    for (int param = RateAmountParam; param < NumParams; ++param) params_[static_cast<std::size_t>(param)] = 1.0f;
}
float SkidderModule::clamp(float value, float minimum, float maximum) noexcept { return std::max(minimum, std::min(value, maximum)); }
float SkidderModule::snapParam(int id, float value) noexcept {
    value = clamp(value, 0.0f, 1.0f);
    if (id == ClockRatioParam) return std::round(value * 6.0f) / 6.0f;
    if (id == ClockLockParam || id == ResetParam) return value >= 0.5f ? 1.0f : 0.0f;
    return value;
}
void SkidderModule::set_samplerate(float sampleRate) { engine_.setSampleRate(sampleRate); }
void SkidderModule::set_param(int id, float value) { if (id >= 0 && id < NumParams) params_[static_cast<std::size_t>(id)] = snapParam(id, value); }
float SkidderModule::get_param(int id) const { return (id >= 0 && id < NumParams) ? params_[static_cast<std::size_t>(id)] : 0.0f; }
void SkidderModule::set_input(int id, float value) { if (id >= 0 && id < NumInputs) inputs_[static_cast<std::size_t>(id)] = clamp(value, -10.0f, 10.0f); }
float SkidderModule::get_output(int id) const { return (id >= 0 && id < NumOutputs) ? outputs_[static_cast<std::size_t>(id)] : 0.0f; }
void SkidderModule::mark_all_inputs_unpatched() { inputPatched_.fill(false); }
void SkidderModule::mark_input_unpatched(int id) { if (id >= 0 && id < NumInputs) inputPatched_[static_cast<std::size_t>(id)] = false; }
void SkidderModule::mark_input_patched(int id) { if (id >= 0 && id < NumInputs) inputPatched_[static_cast<std::size_t>(id)] = true; }
float SkidderModule::normalizedWithCv(int p, int a, int i) const noexcept {
    const float base = params_[static_cast<std::size_t>(p)];
    if (!inputPatched_[static_cast<std::size_t>(i)]) return base;
    const float depth = params_[static_cast<std::size_t>(a)] * 2.0f - 1.0f;
    return clamp(base + (inputs_[static_cast<std::size_t>(i)] / kCvFullScale) * depth, 0.0f, 1.0f);
}
bool SkidderModule::gateValue(int p, int i) const noexcept {
    return inputPatched_[static_cast<std::size_t>(i)] ? inputs_[static_cast<std::size_t>(i)] >= kGateThreshold : params_[static_cast<std::size_t>(p)] >= 0.5f;
}
void SkidderModule::update() {
    const float leftV = inputPatched_[AudioLeftInput] ? inputs_[AudioLeftInput] : 0.0f;
    const float rightV = inputPatched_[AudioRightInput] ? inputs_[AudioRightInput] : leftV;
    if (bypassed) { outputs_[AudioLeftOutput] = leftV; outputs_[AudioRightOutput] = rightV; return; }
    const bool clockHigh = inputPatched_[ClockInput] && inputs_[ClockInput] >= kGateThreshold;
    const bool resetHigh = gateValue(ResetParam, ResetTriggerInput);
    if (clockHigh && !previousClockHigh_) engine_.clockPulse();
    if (resetHigh && !previousResetHigh_) engine_.triggerReset();
    previousClockHigh_ = clockHigh;
    previousResetHigh_ = resetHigh;

    const float rate = normalizedWithCv(RateParam, RateAmountParam, RateCvInput);
    const float width = normalizedWithCv(WidthParam, WidthAmountParam, WidthCvInput);
    const float slope = normalizedWithCv(SlopeParam, SlopeAmountParam, SlopeCvInput);
    const float floor = normalizedWithCv(FloorParam, FloorAmountParam, FloorCvInput);
    const float random = normalizedWithCv(RandomParam, RandomAmountParam, RandomCvInput);
    const float stereo = normalizedWithCv(StereoParam, StereoAmountParam, StereoCvInput);
    const float ratio = normalizedWithCv(ClockRatioParam, ClockRatioAmountParam, ClockRatioCvInput);
    const float mix = normalizedWithCv(MixParam, MixAmountParam, MixCvInput);

    dfxmm::SkidderEngine::Parameters p;
    p.rateHz = 0.02f + rate * rate * 50.0f;
    p.pulseWidth = 0.01f + width * 0.98f;
    p.slope = slope;
    p.floor = floor;
    p.randomAmount = random;
    p.stereoOffset = stereo;
    p.clockRatio = kClockRatios[static_cast<std::size_t>(std::lround(ratio * 6.0f))];
    p.mix = mix;
    p.clockLock = gateValue(ClockLockParam, ClockLockGateInput);
    engine_.setParameters(p);
    const auto output = engine_.process(leftV * kAudioInputScale, rightV * kAudioInputScale);
    outputs_[AudioLeftOutput] = clamp(output.left * kAudioOutputScale, -10.0f, 10.0f);
    outputs_[AudioRightOutput] = clamp(output.right * kAudioOutputScale, -10.0f, 10.0f);
}
