#include "skidder_module.hpp"
#include "register_helpers.hpp"

#include <array>

void register_skidder_module() {
    using P = dfxmm::registration::ParamVisual;
    using J = dfxmm::registration::JackVisual;
    constexpr std::array<float, 8> x{{14, 42, 70, 98, 126, 154, 182, 210}};
    constexpr std::array<P, SkidderModule::NumParams> params{{
        {"Rate", x[0], 24, 0.32f, 50, 0, "Hz", 2}, {"Width", x[1], 24, 0.5f, 100, 0, "%", 0},
        {"Slope", x[2], 24, 0.15f, 100, 0, "%", 0}, {"Floor", x[3], 24, 0, 100, 0, "%", 0},
        {"Random", x[4], 24, 0, 100, 0, "%", 0}, {"Stereo", x[5], 24, 0, 100, 0, "%", 0},
        {"Clock Ratio", x[6], 24, 0.5f, 6, 0, "", 0}, {"Mix", x[7], 24, 1, 100, 0, "%", 0},
        {"Clock Lock", x[0], 81, 0, 1, 0, "", 0}, {"Reset", x[1], 81, 0, 1, 0, "", 0},
        {"Rate Amt", x[0], 55, 1, 200, -100, "%", 0}, {"Width Amt", x[1], 55, 1, 200, -100, "%", 0},
        {"Slope Amt", x[2], 55, 1, 200, -100, "%", 0}, {"Floor Amt", x[3], 55, 1, 200, -100, "%", 0},
        {"Random Amt", x[4], 55, 1, 200, -100, "%", 0}, {"Stereo Amt", x[5], 55, 1, 200, -100, "%", 0},
        {"Ratio Amt", x[6], 55, 1, 200, -100, "%", 0}, {"Mix Amt", x[7], 55, 1, 200, -100, "%", 0},
    }};
    constexpr std::array<J, SkidderModule::NumInputs> inputs{{
        {"Audio L", 14, 104}, {"Audio R", 42, 104}, {"Rate CV", 70, 104}, {"Width CV", 98, 104},
        {"Slope CV", 126, 104}, {"Floor CV", 154, 104}, {"Random CV", 182, 104}, {"Stereo CV", 210, 104},
        {"Ratio CV", 70, 123}, {"Mix CV", 98, 123}, {"Clock", 126, 123}, {"Lock Gate", 154, 123}, {"Reset Trig", 182, 123},
    }};
    constexpr std::array<J, SkidderModule::NumOutputs> outputs{{{"Out L", 210, 123}, {"Out R", 230, 123}}};
    dfxmm::registration::registerModule<SkidderModule>(
        "DTMR-Skidder", "Tempo-ready random stereo amplitude skidder with full CV", 42,
        "DestroyThemMyRobots/Skidder.png", params, inputs, outputs);
}
