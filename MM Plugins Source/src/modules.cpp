#include "plugin.hpp"
#include "bridge.hpp"
#include "metamodule/transverb_module.hpp"
#include "metamodule/scrubby_module.hpp"
#include "metamodule/buffer_override_module.hpp"
#include "metamodule/skidder_module.hpp"
#include "metamodule/polarizer_module.hpp"

struct RackTransverb : CoreBridgeModule<TransverbModule> {
    RackTransverb() : CoreBridgeModule(spec()) {}
    static const ModuleSpec& spec() {
        static const ModuleSpec s{"DTMR-Transverb", "res/Transverb.png", {"Buffer", "Speed A", "Delay A", "Level A", "Speed B", "Delay B", "Level B", "Feedback", "Cross FB", "Tomsound", "Spread", "Mix", "Freeze", "Reset", "Buffer Amt", "Speed A Amt", "Delay A Amt", "Level A Amt", "Speed B Amt", "Delay B Amt", "Level B Amt", "Feedback Amt", "Cross Amt", "Tomsound Amt", "Spread Amt", "Mix Amt"}, {"Audio L", "Audio R", "Buffer CV", "Speed A CV", "Delay A CV", "Level A CV", "Speed B CV", "Delay B CV", "Level B CV", "Feedback CV", "Cross CV", "Tomsound CV", "Spread CV", "Mix CV", "Freeze Gate", "Reset Trig", "Clock"}, {"Out L", "Out R"}, 42};
        return s;
    }
};
Model* modelTransverb = createModel<RackTransverb, CoreBridgeWidget<RackTransverb>>("DTMR-Transverb");

struct RackScrubby : CoreBridgeModule<ScrubbyModule> {
    RackScrubby() : CoreBridgeModule(spec()) {}
    static const ModuleSpec& spec() {
        static const ModuleSpec s{"DTMR-Scrubby", "res/Scrubby.png", {"Buffer", "Speed", "Seek", "Range", "Jump Rate", "Jump Amt", "Inertia", "Quantize", "Reverse", "Feedback", "Spread", "Mix", "Freeze", "Jump", "Reset", "Buffer Amt", "Speed Amt", "Seek Amt", "Range Amt", "Jump Rate Amt", "Jump Amt Amt", "Inertia Amt", "Quantize Amt", "Reverse Amt", "Feedback Amt", "Spread Amt", "Mix Amt"}, {"Audio L", "Audio R", "Buffer CV", "Speed CV", "Seek CV", "Range CV", "Jump Rate CV", "Jump Amt CV", "Inertia CV", "Quantize CV", "Reverse CV", "Feedback CV", "Spread CV", "Mix CV", "Freeze Gate", "Jump Trig", "Reset Trig", "Clock"}, {"Out L", "Out R"}, 42};
        return s;
    }
};
Model* modelScrubby = createModel<RackScrubby, CoreBridgeWidget<RackScrubby>>("DTMR-Scrubby");

struct RackBufferOverride : CoreBridgeModule<BufferOverrideModule> {
    RackBufferOverride() : CoreBridgeModule(spec()) {}
    static const ModuleSpec& spec() {
        static const ModuleSpec s{"DTMR-BufferOverride", "res/BufferOverride.png", {"Buffer", "Divisor", "Duty", "Smooth", "Decay", "Random", "Reverse", "Feedback", "Clock Ratio", "Mix", "Clock Lock", "Freeze", "Capture", "Buffer Amt", "Divisor Amt", "Duty Amt", "Smooth Amt", "Decay Amt", "Random Amt", "Reverse Amt", "Feedback Amt", "Ratio Amt", "Mix Amt"}, {"Audio L", "Audio R", "Buffer CV", "Divisor CV", "Duty CV", "Smooth CV", "Decay CV", "Random CV", "Reverse CV", "Feedback CV", "Ratio CV", "Mix CV", "Clock", "Lock Gate", "Freeze Gate", "Capture Trig"}, {"Out L", "Out R"}, 42};
        return s;
    }
};
Model* modelBufferOverride = createModel<RackBufferOverride, CoreBridgeWidget<RackBufferOverride>>("DTMR-BufferOverride");

struct RackSkidder : CoreBridgeModule<SkidderModule> {
    RackSkidder() : CoreBridgeModule(spec()) {}
    static const ModuleSpec& spec() {
        static const ModuleSpec s{"DTMR-Skidder", "res/Skidder.png", {"Rate", "Width", "Slope", "Floor", "Random", "Stereo", "Clock Ratio", "Mix", "Clock Lock", "Reset", "Rate Amt", "Width Amt", "Slope Amt", "Floor Amt", "Random Amt", "Stereo Amt", "Ratio Amt", "Mix Amt"}, {"Audio L", "Audio R", "Rate CV", "Width CV", "Slope CV", "Floor CV", "Random CV", "Stereo CV", "Ratio CV", "Mix CV", "Clock", "Lock Gate", "Reset Trig"}, {"Out L", "Out R"}, 42};
        return s;
    }
};
Model* modelSkidder = createModel<RackSkidder, CoreBridgeWidget<RackSkidder>>("DTMR-Skidder");

struct RackPolarizer : CoreBridgeModule<PolarizerModule> {
    RackPolarizer() : CoreBridgeModule(spec()) {}
    static const ModuleSpec& spec() {
        static const ModuleSpec s{"DTMR-Polarizer", "res/Polarizer.png", {"Interval", "Burst", "Probability", "Mode", "Stereo", "Drive", "Mix", "Reset", "Interval Amt", "Burst Amt", "Probability Amt", "Mode Amt", "Stereo Amt", "Drive Amt", "Mix Amt"}, {"Audio L", "Audio R", "Interval CV", "Burst CV", "Probability CV", "Mode CV", "Stereo CV", "Drive CV", "Mix CV", "Reset Trig"}, {"Out L", "Out R"}, 30};
        return s;
    }
};
Model* modelPolarizer = createModel<RackPolarizer, CoreBridgeWidget<RackPolarizer>>("DTMR-Polarizer");
