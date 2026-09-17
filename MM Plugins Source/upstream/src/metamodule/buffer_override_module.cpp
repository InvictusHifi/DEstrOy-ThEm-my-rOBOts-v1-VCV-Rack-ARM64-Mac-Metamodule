#include "buffer_override_module.hpp"

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

BufferOverrideModule::BufferOverrideModule() {
    params_.fill(0.0f); inputs_.fill(0.0f); inputPatched_.fill(false); outputs_.fill(0.0f);
    params_[BufferParam] = 0.22f;
    params_[DivisorParam] = 7.0f / 63.0f;
    params_[DutyParam] = 1.0f;
    params_[SmoothParam] = 0.12f;
    params_[DecayParam] = 1.0f;
    params_[ClockRatioParam] = 0.5f;
    params_[MixParam] = 1.0f;
    for (int param = BufferAmountParam; param < NumParams; ++param) params_[static_cast<std::size_t>(param)] = 1.0f;
}
float BufferOverrideModule::clamp(float value, float minimum, float maximum) noexcept { return std::max(minimum, std::min(value, maximum)); }
float BufferOverrideModule::snapParam(int paramId, float value) noexcept {
    value = clamp(value, 0.0f, 1.0f);
    if (paramId == DivisorParam) return std::round(value * 63.0f) / 63.0f;
    if (paramId == ClockRatioParam) return std::round(value * 6.0f) / 6.0f;
    if (paramId == ClockLockParam || paramId == FreezeParam || paramId == CaptureParam) return value >= 0.5f ? 1.0f : 0.0f;
    return value;
}
void BufferOverrideModule::set_samplerate(float sampleRate) { engine_.setSampleRate(sampleRate); }
void BufferOverrideModule::set_param(int id, float value) { if (id >= 0 && id < NumParams) params_[static_cast<std::size_t>(id)] = snapParam(id, value); }
float BufferOverrideModule::get_param(int id) const { return (id >= 0 && id < NumParams) ? params_[static_cast<std::size_t>(id)] : 0.0f; }
void BufferOverrideModule::set_input(int id, float value) { if (id >= 0 && id < NumInputs) inputs_[static_cast<std::size_t>(id)] = clamp(value, -10.0f, 10.0f); }
float BufferOverrideModule::get_output(int id) const { return (id >= 0 && id < NumOutputs) ? outputs_[static_cast<std::size_t>(id)] : 0.0f; }
void BufferOverrideModule::mark_all_inputs_unpatched() { inputPatched_.fill(false); }
void BufferOverrideModule::mark_input_unpatched(int id) { if (id >= 0 && id < NumInputs) inputPatched_[static_cast<std::size_t>(id)] = false; }
void BufferOverrideModule::mark_input_patched(int id) { if (id >= 0 && id < NumInputs) inputPatched_[static_cast<std::size_t>(id)] = true; }
float BufferOverrideModule::normalizedWithCv(int p, int a, int i) const noexcept {
    const float base = params_[static_cast<std::size_t>(p)];
    if (!inputPatched_[static_cast<std::size_t>(i)]) return base;
    const float depth = params_[static_cast<std::size_t>(a)] * 2.0f - 1.0f;
    return clamp(base + (inputs_[static_cast<std::size_t>(i)] / kCvFullScale) * depth, 0.0f, 1.0f);
}
bool BufferOverrideModule::gateValue(int p, int i) const noexcept {
    return inputPatched_[static_cast<std::size_t>(i)] ? inputs_[static_cast<std::size_t>(i)] >= kGateThreshold : params_[static_cast<std::size_t>(p)] >= 0.5f;
}
void BufferOverrideModule::update() {
    const float leftV = inputPatched_[AudioLeftInput] ? inputs_[AudioLeftInput] : 0.0f;
    const float rightV = inputPatched_[AudioRightInput] ? inputs_[AudioRightInput] : leftV;
    if (bypassed) { outputs_[AudioLeftOutput] = leftV; outputs_[AudioRightOutput] = rightV; return; }

    const bool clockHigh = inputPatched_[ClockInput] && inputs_[ClockInput] >= kGateThreshold;
    const bool captureHigh = gateValue(CaptureParam, CaptureTriggerInput);
    if (clockHigh && !previousClockHigh_) engine_.clockPulse();
    if (captureHigh && !previousCaptureHigh_) engine_.triggerCapture();
    previousClockHigh_ = clockHigh;
    previousCaptureHigh_ = captureHigh;

    const float buffer = normalizedWithCv(BufferParam, BufferAmountParam, BufferCvInput);
    const float divisor = normalizedWithCv(DivisorParam, DivisorAmountParam, DivisorCvInput);
    const float duty = normalizedWithCv(DutyParam, DutyAmountParam, DutyCvInput);
    const float smooth = normalizedWithCv(SmoothParam, SmoothAmountParam, SmoothCvInput);
    const float decay = normalizedWithCv(DecayParam, DecayAmountParam, DecayCvInput);
    const float random = normalizedWithCv(RandomParam, RandomAmountParam, RandomCvInput);
    const float reverse = normalizedWithCv(ReverseParam, ReverseAmountParam, ReverseCvInput);
    const float feedback = normalizedWithCv(FeedbackParam, FeedbackAmountParam, FeedbackCvInput);
    const float ratio = normalizedWithCv(ClockRatioParam, ClockRatioAmountParam, ClockRatioCvInput);
    const float mix = normalizedWithCv(MixParam, MixAmountParam, MixCvInput);

    dfxmm::BufferOverrideEngine::Parameters p;
    p.bufferSeconds = 0.005f + buffer * buffer * 9.995f;
    p.divisor = 1 + static_cast<int>(std::lround(divisor * 63.0f));
    p.duty = duty;
    p.smooth = smooth;
    p.decay = decay;
    p.randomSize = random;
    p.reverseProbability = reverse;
    p.feedback = feedback * 0.97f;
    p.clockRatio = kClockRatios[static_cast<std::size_t>(std::lround(ratio * 6.0f))];
    p.mix = mix;
    p.clockLock = gateValue(ClockLockParam, ClockLockGateInput);
    p.freeze = gateValue(FreezeParam, FreezeGateInput);
    engine_.setParameters(p);
    const auto output = engine_.process(leftV * kAudioInputScale, rightV * kAudioInputScale);
    outputs_[AudioLeftOutput] = clamp(output.left * kAudioOutputScale, -10.0f, 10.0f);
    outputs_[AudioRightOutput] = clamp(output.right * kAudioOutputScale, -10.0f, 10.0f);
}
