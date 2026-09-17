#include "transverb_module.hpp"
#include "register_helpers.hpp"

#include <array>

void register_transverb_module() {
    using P = dfxmm::registration::ParamVisual;
    using J = dfxmm::registration::JackVisual;
    constexpr float yBase = 24.0f;
    constexpr float yAmount = 55.0f;
    constexpr std::array<float, 12> x{{13, 34, 55, 76, 97, 118, 139, 160, 181, 202, 223, 244}};
    constexpr std::array<P, TransverbModule::NumParams> params{{
        {"Buffer", x[0], yBase, 0.48f, 10.0f, 0.0f, "s", 2},
        {"Speed A", x[1], yBase, 0.625f, 8.0f, -4.0f, "x", 2},
        {"Delay A", x[2], yBase, 0.25f, 100.0f, 0.0f, "%", 0},
        {"Level A", x[3], yBase, 0.75f, 125.0f, 0.0f, "%", 0},
        {"Speed B", x[4], yBase, 0.4375f, 8.0f, -4.0f, "x", 2},
        {"Delay B", x[5], yBase, 0.65f, 100.0f, 0.0f, "%", 0},
        {"Level B", x[6], yBase, 0.75f, 125.0f, 0.0f, "%", 0},
        {"Feedback", x[7], yBase, 0.25f, 98.5f, 0.0f, "%", 0},
        {"Cross FB", x[8], yBase, 0.0f, 100.0f, 0.0f, "%", 0},
        {"Tomsound", x[9], yBase, 0.0f, 100.0f, 0.0f, "%", 0},
        {"Spread", x[10], yBase, 0.5f, 100.0f, 0.0f, "%", 0},
        {"Mix", x[11], yBase, 1.0f, 100.0f, 0.0f, "%", 0},
        {"Freeze", x[0], 81.0f, 0.0f, 1.0f, 0.0f, "", 0},
        {"Reset", x[1], 81.0f, 0.0f, 1.0f, 0.0f, "", 0},
        {"Buffer Amt", x[0], yAmount, 1.0f, 200.0f, -100.0f, "%", 0},
        {"Speed A Amt", x[1], yAmount, 1.0f, 200.0f, -100.0f, "%", 0},
        {"Delay A Amt", x[2], yAmount, 1.0f, 200.0f, -100.0f, "%", 0},
        {"Level A Amt", x[3], yAmount, 1.0f, 200.0f, -100.0f, "%", 0},
        {"Speed B Amt", x[4], yAmount, 1.0f, 200.0f, -100.0f, "%", 0},
        {"Delay B Amt", x[5], yAmount, 1.0f, 200.0f, -100.0f, "%", 0},
        {"Level B Amt", x[6], yAmount, 1.0f, 200.0f, -100.0f, "%", 0},
        {"Feedback Amt", x[7], yAmount, 1.0f, 200.0f, -100.0f, "%", 0},
        {"Cross Amt", x[8], yAmount, 1.0f, 200.0f, -100.0f, "%", 0},
        {"Tomsound Amt", x[9], yAmount, 1.0f, 200.0f, -100.0f, "%", 0},
        {"Spread Amt", x[10], yAmount, 1.0f, 200.0f, -100.0f, "%", 0},
        {"Mix Amt", x[11], yAmount, 1.0f, 200.0f, -100.0f, "%", 0},
    }};
    constexpr std::array<J, TransverbModule::NumInputs> inputs{{
        {"Audio L", 13, 104}, {"Audio R", 34, 104}, {"Buffer CV", 55, 104}, {"Speed A CV", 76, 104},
        {"Delay A CV", 97, 104}, {"Level A CV", 118, 104}, {"Speed B CV", 139, 104}, {"Delay B CV", 160, 104},
        {"Level B CV", 13, 123}, {"Feedback CV", 34, 123}, {"Cross CV", 55, 123}, {"Tomsound CV", 76, 123},
        {"Spread CV", 97, 123}, {"Mix CV", 118, 123}, {"Freeze Gate", 139, 123}, {"Reset Trig", 160, 123},
        {"Clock", 181, 123},
    }};
    constexpr std::array<J, TransverbModule::NumOutputs> outputs{{{"Out L", 223, 123}, {"Out R", 244, 123}}};
    dfxmm::registration::registerModule<TransverbModule>(
        "DTMR-Transverb", "Dual moving-head tape-loop delay with clock-synced buffer/reset, glitch mode and full CV", 42,
        "DestroyThemMyRobots/Transverb.png", params, inputs, outputs);
}
