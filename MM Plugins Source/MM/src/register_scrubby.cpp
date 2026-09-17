#include "scrubby_module.hpp"
#include "register_helpers.hpp"

#include <array>

void register_scrubby_module() {
    using P = dfxmm::registration::ParamVisual;
    using J = dfxmm::registration::JackVisual;
    constexpr std::array<float, 12> x{{13, 34, 55, 76, 97, 118, 139, 160, 181, 202, 223, 244}};
    constexpr std::array<P, ScrubbyModule::NumParams> params{{
        {"Buffer", x[0], 24, 0.45f, 10, 0, "s", 2}, {"Speed", x[1], 24, 0.625f, 8, -4, "x", 2},
        {"Seek", x[2], 24, 0.5f, 100, 0, "%", 0}, {"Range", x[3], 24, 0.5f, 100, 0, "%", 0},
        {"Jump Rate", x[4], 24, 0.22f, 30, 0, "Hz", 2}, {"Jump Amt", x[5], 24, 0.5f, 100, 0, "%", 0},
        {"Inertia", x[6], 24, 0.35f, 100, 0, "%", 0}, {"Quantize", x[7], 24, 0.0f, 3, 0, "", 0},
        {"Reverse", x[8], 24, 0.15f, 100, 0, "%", 0}, {"Feedback", x[9], 24, 0.0f, 97, 0, "%", 0},
        {"Spread", x[10], 24, 0.25f, 100, 0, "%", 0}, {"Mix", x[11], 24, 1.0f, 100, 0, "%", 0},
        {"Freeze", x[0], 81, 0, 1, 0, "", 0}, {"Jump", x[1], 81, 0, 1, 0, "", 0}, {"Reset", x[2], 81, 0, 1, 0, "", 0},
        {"Buffer Amt", x[0], 55, 1, 200, -100, "%", 0}, {"Speed Amt", x[1], 55, 1, 200, -100, "%", 0},
        {"Seek Amt", x[2], 55, 1, 200, -100, "%", 0}, {"Range Amt", x[3], 55, 1, 200, -100, "%", 0},
        {"Jump Rate Amt", x[4], 55, 1, 200, -100, "%", 0}, {"Jump Amt Amt", x[5], 55, 1, 200, -100, "%", 0},
        {"Inertia Amt", x[6], 55, 1, 200, -100, "%", 0}, {"Quantize Amt", x[7], 55, 1, 200, -100, "%", 0},
        {"Reverse Amt", x[8], 55, 1, 200, -100, "%", 0}, {"Feedback Amt", x[9], 55, 1, 200, -100, "%", 0},
        {"Spread Amt", x[10], 55, 1, 200, -100, "%", 0}, {"Mix Amt", x[11], 55, 1, 200, -100, "%", 0},
    }};
    constexpr std::array<J, ScrubbyModule::NumInputs> inputs{{
        {"Audio L", 13, 104}, {"Audio R", 34, 104}, {"Buffer CV", 55, 104}, {"Speed CV", 76, 104},
        {"Seek CV", 97, 104}, {"Range CV", 118, 104}, {"Jump Rate CV", 139, 104}, {"Jump Amt CV", 160, 104},
        {"Inertia CV", 13, 123}, {"Quantize CV", 34, 123}, {"Reverse CV", 55, 123}, {"Feedback CV", 76, 123},
        {"Spread CV", 97, 123}, {"Mix CV", 118, 123}, {"Freeze Gate", 139, 123}, {"Jump Trig", 160, 123},
        {"Reset Trig", 181, 123}, {"Clock", 202, 123},
    }};
    constexpr std::array<J, ScrubbyModule::NumOutputs> outputs{{{"Out L", 223, 123}, {"Out R", 244, 123}}};
    dfxmm::registration::registerModule<ScrubbyModule>(
        "DTMR-Scrubby", "Robot-DJ live buffer scrubber with clock-triggered jumps, speed constraints and full CV", 42,
        "DestroyThemMyRobots/Scrubby.png", params, inputs, outputs);
}
