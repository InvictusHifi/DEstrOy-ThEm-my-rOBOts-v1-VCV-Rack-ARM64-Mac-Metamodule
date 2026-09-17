#pragma once

#include "CoreModules/register_module.hh"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace dfxmm::registration {

struct ParamVisual {
    std::string_view name;
    float x;
    float y;
    float defaultValue;
    float displayMultiplier;
    float displayOffset;
    std::string_view units;
    unsigned precision;
};

struct JackVisual {
    std::string_view name;
    float x;
    float y;
};

template <typename ModuleType, std::size_t ParameterCount, std::size_t InputCount, std::size_t OutputCount>
void registerModule(
    std::string_view slug,
    std::string_view description,
    int widthHp,
    std::string_view panelImage,
    const std::array<ParamVisual, ParameterCount>& parameters,
    const std::array<JackVisual, InputCount>& inputs,
    const std::array<JackVisual, OutputCount>& outputs) {

    constexpr std::size_t elementCount = ParameterCount + InputCount + OutputCount;
    static std::array<MetaModule::Element, elementCount> elements{};
    static std::array<ElementCount::Indices, elementCount> indices{};
    static constexpr std::array<MetaModule::ModuleInfoBase::BypassRoute, 2> bypassRoutes{{{0,0},{1,1}}};
    constexpr bool large = ParameterCount > 16 || InputCount > 12;
    constexpr int cols = large ? 9 : 6, ports = large ? 10 : 8;
    const float w = widthHp * 15.f;
    constexpr float scale = 128.5f / 380.f;

    std::size_t elementIndex = 0u;
    for (std::size_t paramIndex = 0u; paramIndex < ParameterCount; ++paramIndex) {
        const auto& visual = parameters[paramIndex];
        MetaModule::Knob knob;
        knob.x_mm = (26 + (w-52)*(paramIndex%cols + .5f)/cols)*scale;
        knob.y_mm = (172 + (paramIndex/cols)*44)*scale;
        knob.coords = MetaModule::Coords::Center;
        knob.short_name = visual.name;
        knob.long_name = visual.name;
        knob.image = "DestroyThemMyRobots/components/knob.png";
        knob.default_value = visual.defaultValue;
        knob.min_value = 0.0f;
        knob.max_value = 1.0f;
        knob.display_mult = visual.displayMultiplier;
        knob.display_offset = visual.displayOffset;
        knob.units = visual.units;
        knob.display_precision = static_cast<std::uint8_t>(visual.precision);
        elements[elementIndex] = knob;
        indices[elementIndex] = ElementCount::NoElementIndices;
        indices[elementIndex].param_idx = static_cast<std::uint16_t>(paramIndex);
        ++elementIndex;
    }

    for (std::size_t inputIndex = 0u; inputIndex < InputCount; ++inputIndex) {
        const auto& visual = inputs[inputIndex];
        MetaModule::JackInput jack;
        jack.x_mm = (26 + (w-52)*(inputIndex%ports + .5f)/ports)*scale;
        jack.y_mm = (312 + (inputIndex/ports)*39)*scale;
        jack.coords = MetaModule::Coords::Center;
        jack.short_name = visual.name;
        jack.long_name = visual.name;
        jack.image = "DestroyThemMyRobots/components/jack.png";
        elements[elementIndex] = jack;
        indices[elementIndex] = ElementCount::NoElementIndices;
        indices[elementIndex].input_idx = static_cast<std::uint16_t>(inputIndex);
        ++elementIndex;
    }

    for (std::size_t outputIndex = 0u; outputIndex < OutputCount; ++outputIndex) {
        const auto& visual = outputs[outputIndex];
        MetaModule::JackOutput jack;
        jack.x_mm = (26 + (w-52)*((InputCount+outputIndex)%ports + .5f)/ports)*scale;
        jack.y_mm = (312 + ((InputCount+outputIndex)/ports)*39)*scale;
        jack.coords = MetaModule::Coords::Center;
        jack.short_name = visual.name;
        jack.long_name = visual.name;
        jack.image = "DestroyThemMyRobots/components/jack.png";
        elements[elementIndex] = jack;
        indices[elementIndex] = ElementCount::NoElementIndices;
        indices[elementIndex].output_idx = static_cast<std::uint16_t>(outputIndex);
        ++elementIndex;
    }

    const MetaModule::ModuleInfoView info{
        .description = description,
        .width_hp = static_cast<std::uint32_t>(widthHp),
        .elements = elements,
        .indices = indices,
        .bypass_routes = bypassRoutes,
    };
    MetaModule::register_module<ModuleType>("DestroyThemMyRobots", slug, info, panelImage);
}

} // namespace dfxmm::registration
