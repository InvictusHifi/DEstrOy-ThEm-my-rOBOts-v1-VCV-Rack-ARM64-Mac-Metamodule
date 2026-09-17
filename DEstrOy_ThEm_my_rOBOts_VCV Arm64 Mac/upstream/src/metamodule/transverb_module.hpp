#pragma once

#include "CoreModules/CoreProcessor.hh"
#include "dsp/transverb_engine.hpp"
#include "clock_sync.hpp"

#include <array>

class TransverbModule final : public CoreProcessor {
public:
    enum ParamId {
        BufferParam,
        SpeedAParam,
        DelayAParam,
        LevelAParam,
        SpeedBParam,
        DelayBParam,
        LevelBParam,
        FeedbackParam,
        CrossFeedbackParam,
        TomsoundParam,
        SpreadParam,
        MixParam,
        FreezeParam,
        ResetParam,
        BufferAmountParam,
        SpeedAAmountParam,
        DelayAAmountParam,
        LevelAAmountParam,
        SpeedBAmountParam,
        DelayBAmountParam,
        LevelBAmountParam,
        FeedbackAmountParam,
        CrossFeedbackAmountParam,
        TomsoundAmountParam,
        SpreadAmountParam,
        MixAmountParam,
        NumParams
    };

    enum InputId {
        AudioLeftInput,
        AudioRightInput,
        BufferCvInput,
        SpeedACvInput,
        DelayACvInput,
        LevelACvInput,
        SpeedBCvInput,
        DelayBCvInput,
        LevelBCvInput,
        FeedbackCvInput,
        CrossFeedbackCvInput,
        TomsoundCvInput,
        SpreadCvInput,
        MixCvInput,
        FreezeGateInput,
        ResetTriggerInput,
        ClockInput,
        NumInputs
    };

    enum OutputId { AudioLeftOutput, AudioRightOutput, NumOutputs };

    TransverbModule();
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
    dfxmm::TransverbEngine engine_{};
    dfxclock::ClockTracker clockTracker_{};
    bool previousResetHigh_ = false;
};
