#include "transverb_module.hpp"

#include <algorithm>

namespace {
constexpr float kAudioInputScale = 0.2f;
constexpr float kAudioOutputScale = 5.0f;
constexpr float kCvFullScale = 5.0f;
constexpr float kGateThreshold = 1.0f;
}

TransverbModule::TransverbModule() {
    params_.fill(0.0f);
    inputs_.fill(0.0f);
    inputPatched_.fill(false);
    outputs_.fill(0.0f);
    params_[BufferParam] = 0.48f;
    params_[SpeedAParam] = 0.625f;
    params_[DelayAParam] = 0.25f;
    params_[LevelAParam] = 0.75f;
    params_[SpeedBParam] = 0.4375f;
    params_[DelayBParam] = 0.65f;
    params_[LevelBParam] = 0.75f;
    params_[FeedbackParam] = 0.25f;
    params_[CrossFeedbackParam] = 0.0f;
    params_[TomsoundParam] = 0.0f;
    params_[SpreadParam] = 0.5f;
    params_[MixParam] = 1.0f;
    for (int param = BufferAmountParam; param < NumParams; ++param) {
        params_[static_cast<std::size_t>(param)] = 1.0f;
    }
}

float TransverbModule::clamp(float value, float minimum, float maximum) noexcept {
    return std::max(minimum, std::min(value, maximum));
}

float TransverbModule::snapParam(int paramId, float value) noexcept {
    value = clamp(value, 0.0f, 1.0f);
    if (paramId == FreezeParam || paramId == ResetParam) {
        return value >= 0.5f ? 1.0f : 0.0f;
    }
    return value;
}

void TransverbModule::set_samplerate(float sampleRate) { engine_.setSampleRate(sampleRate); clockTracker_.setSampleRate(sampleRate); }
void TransverbModule::set_param(int paramId, float value) {
    if (paramId >= 0 && paramId < NumParams) params_[static_cast<std::size_t>(paramId)] = snapParam(paramId, value);
}
float TransverbModule::get_param(int paramId) const {
    return (paramId >= 0 && paramId < NumParams) ? params_[static_cast<std::size_t>(paramId)] : 0.0f;
}
void TransverbModule::set_input(int inputId, float value) {
    if (inputId >= 0 && inputId < NumInputs) inputs_[static_cast<std::size_t>(inputId)] = clamp(value, -10.0f, 10.0f);
}
float TransverbModule::get_output(int outputId) const {
    return (outputId >= 0 && outputId < NumOutputs) ? outputs_[static_cast<std::size_t>(outputId)] : 0.0f;
}
void TransverbModule::mark_all_inputs_unpatched() { inputPatched_.fill(false); }
void TransverbModule::mark_input_unpatched(int inputId) {
    if (inputId >= 0 && inputId < NumInputs) inputPatched_[static_cast<std::size_t>(inputId)] = false;
}
void TransverbModule::mark_input_patched(int inputId) {
    if (inputId >= 0 && inputId < NumInputs) inputPatched_[static_cast<std::size_t>(inputId)] = true;
}

float TransverbModule::normalizedWithCv(int paramId, int amountParamId, int inputId) const noexcept {
    const float base = params_[static_cast<std::size_t>(paramId)];
    if (!inputPatched_[static_cast<std::size_t>(inputId)]) return base;
    const float depth = params_[static_cast<std::size_t>(amountParamId)] * 2.0f - 1.0f;
    return clamp(base + ((inputs_[static_cast<std::size_t>(inputId)] / kCvFullScale) * depth), 0.0f, 1.0f);
}

bool TransverbModule::gateValue(int paramId, int inputId) const noexcept {
    return inputPatched_[static_cast<std::size_t>(inputId)]
        ? inputs_[static_cast<std::size_t>(inputId)] >= kGateThreshold
        : params_[static_cast<std::size_t>(paramId)] >= 0.5f;
}

void TransverbModule::update() {
    const bool clockPatched = inputPatched_[ClockInput];
    const bool clockPulse = clockTracker_.process(clockPatched ? inputs_[ClockInput] : 0.0f, clockPatched);

    const float inputLeftVolts = inputPatched_[AudioLeftInput] ? inputs_[AudioLeftInput] : 0.0f;
    const float inputRightVolts = inputPatched_[AudioRightInput] ? inputs_[AudioRightInput] : inputLeftVolts;
    if (bypassed) {
        outputs_[AudioLeftOutput] = inputLeftVolts;
        outputs_[AudioRightOutput] = inputRightVolts;
        return;
    }

    const float buffer = normalizedWithCv(BufferParam, BufferAmountParam, BufferCvInput);
    const float speedA = normalizedWithCv(SpeedAParam, SpeedAAmountParam, SpeedACvInput);
    const float delayA = normalizedWithCv(DelayAParam, DelayAAmountParam, DelayACvInput);
    const float levelA = normalizedWithCv(LevelAParam, LevelAAmountParam, LevelACvInput);
    const float speedB = normalizedWithCv(SpeedBParam, SpeedBAmountParam, SpeedBCvInput);
    const float delayB = normalizedWithCv(DelayBParam, DelayBAmountParam, DelayBCvInput);
    const float levelB = normalizedWithCv(LevelBParam, LevelBAmountParam, LevelBCvInput);
    const float feedback = normalizedWithCv(FeedbackParam, FeedbackAmountParam, FeedbackCvInput);
    const float cross = normalizedWithCv(CrossFeedbackParam, CrossFeedbackAmountParam, CrossFeedbackCvInput);
    const float tomsound = normalizedWithCv(TomsoundParam, TomsoundAmountParam, TomsoundCvInput);
    const float spread = normalizedWithCv(SpreadParam, SpreadAmountParam, SpreadCvInput);
    const float mix = normalizedWithCv(MixParam, MixAmountParam, MixCvInput);

    const bool resetHigh = gateValue(ResetParam, ResetTriggerInput);
    if ((resetHigh && !previousResetHigh_) || clockPulse) engine_.triggerReset();
    previousResetHigh_ = resetHigh;

    dfxmm::TransverbEngine::Parameters p;
    p.bufferSeconds = (clockPatched && clockTracker_.valid())
        ? clamp(clockTracker_.periodSeconds(), 0.02f, 10.0f)
        : 0.02f + (buffer * buffer * 9.98f);
    p.speedA = (speedA * 2.0f - 1.0f) * 4.0f;
    p.delayA = delayA;
    p.levelA = levelA * 1.25f;
    p.speedB = (speedB * 2.0f - 1.0f) * 4.0f;
    p.delayB = delayB;
    p.levelB = levelB * 1.25f;
    p.feedback = feedback * 0.985f;
    p.crossFeedback = cross;
    p.tomsound = tomsound;
    p.spread = spread;
    p.mix = mix;
    p.freeze = gateValue(FreezeParam, FreezeGateInput);
    engine_.setParameters(p);

    const auto output = engine_.process(inputLeftVolts * kAudioInputScale, inputRightVolts * kAudioInputScale);
    outputs_[AudioLeftOutput] = clamp(output.left * kAudioOutputScale, -10.0f, 10.0f);
    outputs_[AudioRightOutput] = clamp(output.right * kAudioOutputScale, -10.0f, 10.0f);
}
