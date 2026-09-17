#include "scrubby_module.hpp"

#include <algorithm>
#include <cmath>

namespace {
constexpr float kAudioInputScale = 0.2f;
constexpr float kAudioOutputScale = 5.0f;
constexpr float kCvFullScale = 5.0f;
constexpr float kGateThreshold = 1.0f;
}

ScrubbyModule::ScrubbyModule() {
    params_.fill(0.0f); inputs_.fill(0.0f); inputPatched_.fill(false); outputs_.fill(0.0f);
    params_[BufferParam] = 0.45f;
    params_[SpeedParam] = 0.625f;
    params_[SeekParam] = 0.5f;
    params_[RangeParam] = 0.5f;
    params_[JumpRateParam] = 0.22f;
    params_[JumpAmountParam] = 0.5f;
    params_[InertiaParam] = 0.35f;
    params_[QuantizeParam] = 0.0f;
    params_[ReverseParam] = 0.15f;
    params_[FeedbackParam] = 0.0f;
    params_[SpreadParam] = 0.25f;
    params_[MixParam] = 1.0f;
    for (int param = BufferAmountParam; param < NumParams; ++param) params_[static_cast<std::size_t>(param)] = 1.0f;
}
float ScrubbyModule::clamp(float value, float minimum, float maximum) noexcept { return std::max(minimum, std::min(value, maximum)); }
float ScrubbyModule::snapParam(int paramId, float value) noexcept {
    value = clamp(value, 0.0f, 1.0f);
    if (paramId == QuantizeParam) return std::round(value * 3.0f) / 3.0f;
    if (paramId == FreezeParam || paramId == JumpParam || paramId == ResetParam) return value >= 0.5f ? 1.0f : 0.0f;
    return value;
}
void ScrubbyModule::set_samplerate(float sampleRate) { engine_.setSampleRate(sampleRate); }
void ScrubbyModule::set_param(int id, float value) { if (id >= 0 && id < NumParams) params_[static_cast<std::size_t>(id)] = snapParam(id, value); }
float ScrubbyModule::get_param(int id) const { return (id >= 0 && id < NumParams) ? params_[static_cast<std::size_t>(id)] : 0.0f; }
void ScrubbyModule::set_input(int id, float value) { if (id >= 0 && id < NumInputs) inputs_[static_cast<std::size_t>(id)] = clamp(value, -10.0f, 10.0f); }
float ScrubbyModule::get_output(int id) const { return (id >= 0 && id < NumOutputs) ? outputs_[static_cast<std::size_t>(id)] : 0.0f; }
void ScrubbyModule::mark_all_inputs_unpatched() { inputPatched_.fill(false); }
void ScrubbyModule::mark_input_unpatched(int id) { if (id >= 0 && id < NumInputs) inputPatched_[static_cast<std::size_t>(id)] = false; }
void ScrubbyModule::mark_input_patched(int id) { if (id >= 0 && id < NumInputs) inputPatched_[static_cast<std::size_t>(id)] = true; }
float ScrubbyModule::normalizedWithCv(int p, int a, int i) const noexcept {
    const float base = params_[static_cast<std::size_t>(p)];
    if (!inputPatched_[static_cast<std::size_t>(i)]) return base;
    const float depth = params_[static_cast<std::size_t>(a)] * 2.0f - 1.0f;
    return clamp(base + (inputs_[static_cast<std::size_t>(i)] / kCvFullScale) * depth, 0.0f, 1.0f);
}
bool ScrubbyModule::gateValue(int p, int i) const noexcept {
    return inputPatched_[static_cast<std::size_t>(i)] ? inputs_[static_cast<std::size_t>(i)] >= kGateThreshold : params_[static_cast<std::size_t>(p)] >= 0.5f;
}
void ScrubbyModule::update() {
    const bool clockPatched = inputPatched_[ClockInput];
    const bool clockHigh = clockPatched && inputs_[ClockInput] >= kGateThreshold;
    const bool clockPulse = clockHigh && !previousClockHigh_;
    previousClockHigh_ = clockHigh;

    const float leftV = inputPatched_[AudioLeftInput] ? inputs_[AudioLeftInput] : 0.0f;
    const float rightV = inputPatched_[AudioRightInput] ? inputs_[AudioRightInput] : leftV;
    if (bypassed) { outputs_[AudioLeftOutput] = leftV; outputs_[AudioRightOutput] = rightV; return; }

    const float buffer = normalizedWithCv(BufferParam, BufferAmountParam, BufferCvInput);
    const float speed = normalizedWithCv(SpeedParam, SpeedAmountParam, SpeedCvInput);
    const float seek = normalizedWithCv(SeekParam, SeekAmountParam, SeekCvInput);
    const float range = normalizedWithCv(RangeParam, RangeAmountParam, RangeCvInput);
    const float jumpRate = normalizedWithCv(JumpRateParam, JumpRateAmountParam, JumpRateCvInput);
    const float jumpAmount = normalizedWithCv(JumpAmountParam, JumpAmountAmountParam, JumpAmountCvInput);
    const float inertia = normalizedWithCv(InertiaParam, InertiaAmountParam, InertiaCvInput);
    const float quantize = normalizedWithCv(QuantizeParam, QuantizeAmountParam, QuantizeCvInput);
    const float reverse = normalizedWithCv(ReverseParam, ReverseAmountParam, ReverseCvInput);
    const float feedback = normalizedWithCv(FeedbackParam, FeedbackAmountParam, FeedbackCvInput);
    const float spread = normalizedWithCv(SpreadParam, SpreadAmountParam, SpreadCvInput);
    const float mix = normalizedWithCv(MixParam, MixAmountParam, MixCvInput);

    const bool jumpHigh = gateValue(JumpParam, JumpTriggerInput);
    const bool resetHigh = gateValue(ResetParam, ResetTriggerInput);
    if ((jumpHigh && !previousJumpHigh_) || clockPulse) engine_.triggerJump();
    if (resetHigh && !previousResetHigh_) engine_.triggerReset();
    previousJumpHigh_ = jumpHigh;
    previousResetHigh_ = resetHigh;

    dfxmm::ScrubbyEngine::Parameters p;
    p.bufferSeconds = 0.02f + buffer * buffer * 9.98f;
    p.speedRatio = (speed * 2.0f - 1.0f) * 4.0f;
    p.seek = seek;
    p.range = range;
    p.jumpRateHz = clockPatched ? 0.0f : jumpRate * jumpRate * 30.0f;
    p.jumpAmount = jumpAmount;
    p.inertia = inertia;
    p.quantizeMode = static_cast<int>(std::lround(quantize * 3.0f));
    p.reverseProbability = reverse;
    p.feedback = feedback * 0.97f;
    p.spread = spread;
    p.mix = mix;
    p.freeze = gateValue(FreezeParam, FreezeGateInput);
    engine_.setParameters(p);
    const auto output = engine_.process(leftV * kAudioInputScale, rightV * kAudioInputScale);
    outputs_[AudioLeftOutput] = clamp(output.left * kAudioOutputScale, -10.0f, 10.0f);
    outputs_[AudioRightOutput] = clamp(output.right * kAudioOutputScale, -10.0f, 10.0f);
}
