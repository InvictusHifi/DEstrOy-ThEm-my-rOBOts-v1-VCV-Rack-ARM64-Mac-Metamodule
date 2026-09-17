#include "buffer_override_module.hpp"
#include "register_helpers.hpp"

#include <array>

void register_buffer_override_module() {
    using P = dfxmm::registration::ParamVisual;
    using J = dfxmm::registration::JackVisual;
    constexpr std::array<float, 10> x{{14, 38, 62, 86, 110, 134, 158, 182, 206, 230}};
    constexpr std::array<P, BufferOverrideModule::NumParams> params{{
        {"Buffer", x[0], 24, 0.22f, 10, 0, "s", 2}, {"Divisor", x[1], 24, 7.0f/63.0f, 63, 1, "", 0},
        {"Duty", x[2], 24, 1, 100, 0, "%", 0}, {"Smooth", x[3], 24, 0.12f, 100, 0, "%", 0},
        {"Decay", x[4], 24, 1, 100, 0, "%", 0}, {"Random", x[5], 24, 0, 100, 0, "%", 0},
        {"Reverse", x[6], 24, 0, 100, 0, "%", 0}, {"Feedback", x[7], 24, 0, 97, 0, "%", 0},
        {"Clock Ratio", x[8], 24, 0.5f, 6, 0, "", 0}, {"Mix", x[9], 24, 1, 100, 0, "%", 0},
        {"Clock Lock", x[0], 81, 0, 1, 0, "", 0}, {"Freeze", x[1], 81, 0, 1, 0, "", 0}, {"Capture", x[2], 81, 0, 1, 0, "", 0},
        {"Buffer Amt", x[0], 55, 1, 200, -100, "%", 0}, {"Divisor Amt", x[1], 55, 1, 200, -100, "%", 0},
        {"Duty Amt", x[2], 55, 1, 200, -100, "%", 0}, {"Smooth Amt", x[3], 55, 1, 200, -100, "%", 0},
        {"Decay Amt", x[4], 55, 1, 200, -100, "%", 0}, {"Random Amt", x[5], 55, 1, 200, -100, "%", 0},
        {"Reverse Amt", x[6], 55, 1, 200, -100, "%", 0}, {"Feedback Amt", x[7], 55, 1, 200, -100, "%", 0},
        {"Ratio Amt", x[8], 55, 1, 200, -100, "%", 0}, {"Mix Amt", x[9], 55, 1, 200, -100, "%", 0},
    }};
    constexpr std::array<J, BufferOverrideModule::NumInputs> inputs{{
        {"Audio L", 14, 104}, {"Audio R", 38, 104}, {"Buffer CV", 62, 104}, {"Divisor CV", 86, 104},
        {"Duty CV", 110, 104}, {"Smooth CV", 134, 104}, {"Decay CV", 158, 104}, {"Random CV", 182, 104},
        {"Reverse CV", 14, 123}, {"Feedback CV", 38, 123}, {"Ratio CV", 62, 123}, {"Mix CV", 86, 123},
        {"Clock", 110, 123}, {"Lock Gate", 134, 123}, {"Freeze Gate", 158, 123}, {"Capture Trig", 182, 123},
    }};
    constexpr std::array<J, BufferOverrideModule::NumOutputs> outputs{{{"Out L", 206, 123}, {"Out R", 230, 123}}};
    dfxmm::registration::registerModule<BufferOverrideModule>(
        "DTMR-BufferOverride", "Robotic minibuffer repeater with clocking, decay, randomisation and full CV", 42,
        "DestroyThemMyRobots/BufferOverride.png", params, inputs, outputs);
}
