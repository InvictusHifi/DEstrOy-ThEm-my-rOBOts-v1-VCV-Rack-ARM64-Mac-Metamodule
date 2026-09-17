#pragma once
#include <rack.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include "robot_art.hpp"

using namespace rack;

struct ModuleSpec {
    std::string slug;
    std::string panel;
    std::vector<std::string> paramNames;
    std::vector<std::string> inputNames;
    std::vector<std::string> outputNames;
    int hp = 30;
};

inline bool isTriggerParam(const std::string& s) {
    return s == "Learn" || s == "Reset" || s == "Learn Peak" || s == "Learn RMS" || s == "Capture" || s == "Jump";
}
inline bool isToggleParam(const std::string& s) {
    return s == "Freeze" || s == "Clock Lock" || s == "Gate/Duck" || s == "Peak Track" || s == "Stereo Link" || s == "Invert" || s == "Dual Head" || s == "FB Invert" || s == "Target A/B" || s == "Encode/Decode" || s == "Swap" || s == "Tuning" || s == "RMS/Peak";
}

inline Vec paramPos(const ModuleSpec& s, int index) {
    const bool large = (int)s.paramNames.size() > 16 || (int)s.inputNames.size() > 12;
    const int cols = large ? 9 : 6;
    const float w = s.hp * RACK_GRID_WIDTH;
    const float left = 26.f, right = w - 26.f;
    int row=index/cols, col=index%cols;
    return Vec(left + (right-left)*(col+0.5f)/cols, 172.f + row*44.f);
}
inline Vec portPos(const ModuleSpec& s, int index) {
    const bool large = (int)s.paramNames.size() > 16 || (int)s.inputNames.size() > 12;
    const int cols = large ? 10 : 8;
    const float w = s.hp * RACK_GRID_WIDTH;
    const float left = 26.f, right = w - 26.f;
    int row=index/cols, col=index%cols;
    return Vec(left + (right-left)*(col+0.5f)/cols, 312.f + row*39.f);
}

template <typename CoreT>
struct CoreBridgeModule : Module {
    CoreT core;
    const ModuleSpec* spec_ = nullptr;
    float lastSampleRate_ = 0.f;

    explicit CoreBridgeModule(const ModuleSpec& spec) : spec_(&spec) {
        config(CoreT::NumParams, CoreT::NumInputs, CoreT::NumOutputs, 0);
        for (int i=0;i<CoreT::NumParams;++i) {
            std::string name = i < (int)spec.paramNames.size() ? spec.paramNames[(size_t)i] : ("Param " + std::to_string(i+1));
            configParam(i, 0.f, 1.f, core.get_param(i), name);
        }
        for (int i=0;i<CoreT::NumInputs;++i) {
            std::string name = i < (int)spec.inputNames.size() ? spec.inputNames[(size_t)i] : ("Input " + std::to_string(i+1));
            configInput(i, name);
        }
        for (int i=0;i<CoreT::NumOutputs;++i) {
            std::string name = i < (int)spec.outputNames.size() ? spec.outputNames[(size_t)i] : ("Output " + std::to_string(i+1));
            configOutput(i, name);
        }
        if (CoreT::NumInputs >= 2 && CoreT::NumOutputs >= 2) {
            configBypass(0,0); configBypass(1,1);
        }
    }

    void process(const ProcessArgs& args) override {
        if (args.sampleRate != lastSampleRate_) {
            lastSampleRate_ = args.sampleRate;
            core.set_samplerate(args.sampleRate);
        }
        for (int i=0;i<CoreT::NumParams;++i) core.set_param(i, params[i].getValue());
        core.mark_all_inputs_unpatched();
        for (int i=0;i<CoreT::NumInputs;++i) {
            if (inputs[i].isConnected()) {
                core.mark_input_patched(i);
                core.set_input(i, inputs[i].getVoltage());
            }
        }
        core.bypassed = false;
        core.update();
        for (int i=0;i<CoreT::NumOutputs;++i) outputs[i].setVoltage(core.get_output(i));
    }
};

template <typename RackModuleT>
struct CoreBridgeWidget : ModuleWidget {
    explicit CoreBridgeWidget(RackModuleT* module) {
        setModule(module);
        const ModuleSpec& s = RackModuleT::spec();
        box.size = Vec(s.hp * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
        auto* panel = new RobotPanel(s.panel);
        panel->box.size=box.size;
        addChild(panel);
        for (int i=0;i<(int)s.paramNames.size();++i) {
            Vec p=paramPos(s,i);
            const auto& n=s.paramNames[(size_t)i];
            if (isTriggerParam(n)) addParam(createParamCentered<RobotTrigger>(p,module,i));
            else if (isToggleParam(n)) addParam(createParamCentered<RobotToggle>(p,module,i));
            else addParam(createParamCentered<RobotKnob>(p,module,i));
        }
        int portIndex=0;
        for (int i=0;i<(int)s.inputNames.size();++i,++portIndex) addInput(createInputCentered<RobotPort>(portPos(s,portIndex),module,i));
        for (int i=0;i<(int)s.outputNames.size();++i,++portIndex) addOutput(createOutputCentered<RobotPort>(portPos(s,portIndex),module,i));
    }
};
