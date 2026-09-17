#pragma once

#include "CoreModules/CoreProcessor.hh"
#include "dsp/scrubby_engine.hpp"

#include <array>

class ScrubbyModule final : public CoreProcessor {
public:
    enum ParamId {
        BufferParam, SpeedParam, SeekParam, RangeParam, JumpRateParam, JumpAmountParam,
        InertiaParam, QuantizeParam, ReverseParam, FeedbackParam, SpreadParam, MixParam,
        FreezeParam, JumpParam, ResetParam,
        BufferAmountParam, SpeedAmountParam, SeekAmountParam, RangeAmountParam,
        JumpRateAmountParam, JumpAmountAmountParam, InertiaAmountParam, QuantizeAmountParam,
        ReverseAmountParam, FeedbackAmountParam, SpreadAmountParam, MixAmountParam,
        NumParams
    };
    enum InputId {
        AudioLeftInput, AudioRightInput,
        BufferCvInput, SpeedCvInput, SeekCvInput, RangeCvInput, JumpRateCvInput,
        JumpAmountCvInput, InertiaCvInput, QuantizeCvInput, ReverseCvInput,
        FeedbackCvInput, SpreadCvInput, MixCvInput,
        FreezeGateInput, JumpTriggerInput, ResetTriggerInput, ClockInput,
        NumInputs
    };
    enum OutputId { AudioLeftOutput, AudioRightOutput, NumOutputs };

    ScrubbyModule();
    void update() override;
    void set_samplerate(float sampleRate) override;
    void set_param(int paramId, float value) override;
    float get_param(int paramId) const override;
    void set_input(int inputId, float value) override;
    float get_output(int outputId) const override;
    void mark_all_inputs_unpatched() override;
    void mark_input_unpatched(int inputId) override;
    void mark_input_patched(int inputId) override;

private:
    static float clamp(float value, float minimum, float maximum) noexcept;
    static float snapParam(int paramId, float value) noexcept;
    float normalizedWithCv(int paramId, int amountParamId, int inputId) const noexcept;
    bool gateValue(int paramId, int inputId) const noexcept;
    std::array<float, NumParams> params_{};
    std::array<float, NumInputs> inputs_{};
    std::array<bool, NumInputs> inputPatched_{};
    std::array<float, NumOutputs> outputs_{};
    dfxmm::ScrubbyEngine engine_{};
    bool previousJumpHigh_ = false;
    bool previousResetHigh_ = false;
    bool previousClockHigh_ = false;
};
