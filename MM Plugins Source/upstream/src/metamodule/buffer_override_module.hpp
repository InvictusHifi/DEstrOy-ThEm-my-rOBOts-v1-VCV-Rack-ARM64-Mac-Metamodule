#pragma once

#include "CoreModules/CoreProcessor.hh"
#include "dsp/buffer_override_engine.hpp"

#include <array>

class BufferOverrideModule final : public CoreProcessor {
public:
    enum ParamId {
        BufferParam, DivisorParam, DutyParam, SmoothParam, DecayParam, RandomParam,
        ReverseParam, FeedbackParam, ClockRatioParam, MixParam,
        ClockLockParam, FreezeParam, CaptureParam,
        BufferAmountParam, DivisorAmountParam, DutyAmountParam, SmoothAmountParam,
        DecayAmountParam, RandomAmountParam, ReverseAmountParam, FeedbackAmountParam,
        ClockRatioAmountParam, MixAmountParam,
        NumParams
    };
    enum InputId {
        AudioLeftInput, AudioRightInput,
        BufferCvInput, DivisorCvInput, DutyCvInput, SmoothCvInput, DecayCvInput,
        RandomCvInput, ReverseCvInput, FeedbackCvInput, ClockRatioCvInput, MixCvInput,
        ClockInput, ClockLockGateInput, FreezeGateInput, CaptureTriggerInput,
        NumInputs
    };
    enum OutputId { AudioLeftOutput, AudioRightOutput, NumOutputs };

    BufferOverrideModule();
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
    dfxmm::BufferOverrideEngine engine_{};
    bool previousClockHigh_ = false;
    bool previousCaptureHigh_ = false;
};
