#include "polarizer_module.hpp"

#include <algorithm>
#include <cmath>

namespace {
constexpr float kAudioInputScale = 0.2f;
constexpr float kAudioOutputScale = 5.0f;
constexpr float kCvFullScale = 5.0f;
constexpr float kGateThreshold = 1.0f;
}

PolarizerModule::PolarizerModule() {
    params_.fill(0.0f); inputs_.fill(0.0f); inputPatched_.fill(false); outputs_.fill(0.0f);
    params_[IntervalParam] = 7.0f / 511.0f;
    params_[BurstParam] = 0.0f;
    params_[ProbabilityParam] = 1.0f;
    params_[ModeParam] = 0.0f;
    params_[StereoParam] = 0.0f;
    params_[DriveParam] = 0.0f;
    params_[MixParam] = 1.0f;
    for (int param = IntervalAmountParam; param < NumParams; ++param) params_[static_cast<std::size_t>(param)] = 1.0f;
}
float PolarizerModule::clamp(float value, float minimum, float maximum) noexcept { return std::max(minimum, std::min(value, maximum)); }
float PolarizerModule::snapParam(int id, float value) noexcept {
    value = clamp(value, 0.0f, 1.0f);
    if (id == IntervalParam || id == BurstParam) return std::round(value * 511.0f) / 511.0f;
    if (id == ModeParam) return std::round(value * 2.0f) / 2.0f;
    if (id == ResetParam) return value >= 0.5f ? 1.0f : 0.0f;
    return value;
}
void PolarizerModule::set_samplerate(float) {}
void PolarizerModule::set_param(int id, float value) { if (id >= 0 && id < NumParams) params_[static_cast<std::size_t>(id)] = snapParam(id, value); }
float PolarizerModule::get_param(int id) const { return (id >= 0 && id < NumParams) ? params_[static_cast<std::size_t>(id)] : 0.0f; }
void PolarizerModule::set_input(int id, float value) { if (id >= 0 && id < NumInputs) inputs_[static_cast<std::size_t>(id)] = clamp(value, -10.0f, 10.0f); }
float PolarizerModule::get_output(int id) const { return (id >= 0 && id < NumOutputs) ? outputs_[static_cast<std::size_t>(id)] : 0.0f; }
void PolarizerModule::mark_all_inputs_unpatched() { inputPatched_.fill(false); }
void PolarizerModule::mark_input_unpatched(int id) { if (id >= 0 && id < NumInputs) inputPatched_[static_cast<std::size_t>(id)] = false; }
void PolarizerModule::mark_input_patched(int id) { if (id >= 0 && id < NumInputs) inputPatched_[static_cast<std::size_t>(id)] = true; }
float PolarizerModule::normalizedWithCv(int p, int a, int i) const noexcept {
    const float base = params_[static_cast<std::size_t>(p)];
    if (!inputPatched_[static_cast<std::size_t>(i)]) return base;
    const float depth = params_[static_cast<std::size_t>(a)] * 2.0f - 1.0f;
    return clamp(base + (inputs_[static_cast<std::size_t>(i)] / kCvFullScale) * depth, 0.0f, 1.0f);
}
bool PolarizerModule::gateValue(int p, int i) const noexcept {
    return inputPatched_[static_cast<std::size_t>(i)] ? inputs_[static_cast<std::size_t>(i)] >= kGateThreshold : params_[static_cast<std::size_t>(p)] >= 0.5f;
}
void PolarizerModule::update() {
    const float leftV = inputPatched_[AudioLeftInput] ? inputs_[AudioLeftInput] : 0.0f;
    const float rightV = inputPatched_[AudioRightInput] ? inputs_[AudioRightInput] : leftV;
    if (bypassed) { outputs_[AudioLeftOutput] = leftV; outputs_[AudioRightOutput] = rightV; return; }

    const bool resetHigh = gateValue(ResetParam, ResetTriggerInput);
    if (resetHigh && !previousResetHigh_) engine_.triggerReset();
    previousResetHigh_ = resetHigh;

    const float interval = normalizedWithCv(IntervalParam, IntervalAmountParam, IntervalCvInput);
    const float burst = normalizedWithCv(BurstParam, BurstAmountParam, BurstCvInput);
    const float probability = normalizedWithCv(ProbabilityParam, ProbabilityAmountParam, ProbabilityCvInput);
    const float mode = normalizedWithCv(ModeParam, ModeAmountParam, ModeCvInput);
    const float stereo = normalizedWithCv(StereoParam, StereoAmountParam, StereoCvInput);
    const float drive = normalizedWithCv(DriveParam, DriveAmountParam, DriveCvInput);
    const float mix = normalizedWithCv(MixParam, MixAmountParam, MixCvInput);

    dfxmm::PolarizerEngine::Parameters p;
    p.interval = 1 + static_cast<int>(std::lround(interval * 511.0f));
    p.burst = 1 + static_cast<int>(std::lround(burst * static_cast<float>(p.interval - 1)));
    p.probability = probability;
    p.mode = static_cast<int>(std::lround(mode * 2.0f));
    p.stereoOffset = stereo;
    p.drive = drive;
    p.mix = mix;
    engine_.setParameters(p);
    const auto output = engine_.process(leftV * kAudioInputScale, rightV * kAudioInputScale);
    outputs_[AudioLeftOutput] = clamp(output.left * kAudioOutputScale, -10.0f, 10.0f);
    outputs_[AudioRightOutput] = clamp(output.right * kAudioOutputScale, -10.0f, 10.0f);
}
