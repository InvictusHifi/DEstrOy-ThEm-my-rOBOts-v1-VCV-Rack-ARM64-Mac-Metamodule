#include "polarizer_module.hpp"
#include "register_helpers.hpp"

#include <array>

void register_polarizer_module() {
    using P = dfxmm::registration::ParamVisual;
    using J = dfxmm::registration::JackVisual;
    constexpr std::array<float, 7> x{{14, 44, 74, 104, 134, 164, 194}};
    constexpr std::array<P, PolarizerModule::NumParams> params{{
        {"Interval", x[0], 24, 7.0f/511.0f, 511, 1, "samples", 0}, {"Burst", x[1], 24, 0, 511, 1, "samples", 0},
        {"Probability", x[2], 24, 1, 100, 0, "%", 0}, {"Mode", x[3], 24, 0, 2, 0, "", 0},
        {"Stereo", x[4], 24, 0, 100, 0, "%", 0}, {"Drive", x[5], 24, 0, 100, 0, "%", 0},
        {"Mix", x[6], 24, 1, 100, 0, "%", 0}, {"Reset", x[0], 81, 0, 1, 0, "", 0},
        {"Interval Amt", x[0], 55, 1, 200, -100, "%", 0}, {"Burst Amt", x[1], 55, 1, 200, -100, "%", 0},
        {"Probability Amt", x[2], 55, 1, 200, -100, "%", 0}, {"Mode Amt", x[3], 55, 1, 200, -100, "%", 0},
        {"Stereo Amt", x[4], 55, 1, 200, -100, "%", 0}, {"Drive Amt", x[5], 55, 1, 200, -100, "%", 0},
        {"Mix Amt", x[6], 55, 1, 200, -100, "%", 0},
    }};
    constexpr std::array<J, PolarizerModule::NumInputs> inputs{{
        {"Audio L", 14, 104}, {"Audio R", 44, 104}, {"Interval CV", 74, 104}, {"Burst CV", 104, 104},
        {"Probability CV", 134, 104}, {"Mode CV", 164, 104}, {"Stereo CV", 194, 104},
        {"Drive CV", 74, 123}, {"Mix CV", 104, 123}, {"Reset Trig", 134, 123},
    }};
    constexpr std::array<J, PolarizerModule::NumOutputs> outputs{{{"Out L", 194, 123}, {"Out R", 224, 123}}};
    dfxmm::registration::registerModule<PolarizerModule>(
        "DTMR-Polarizer", "Crispy sample-pattern polarity inverter with probability, drive and full CV", 30,
        "DestroyThemMyRobots/Polarizer.png", params, inputs, outputs);
}
