cmake_minimum_required(VERSION 3.22)
if(NOT DEFINED METAMODULE_SDK_DIR)
  set(METAMODULE_SDK_DIR "$ENV{METAMODULE_SDK_DIR}")
endif()
include(${METAMODULE_SDK_DIR}/plugin.cmake)
project(DestroyThemMyRobots VERSION 2.1.1 LANGUAGES C CXX ASM)
add_library(DestroyThemMyRobots STATIC)
set(CORE ${CMAKE_CURRENT_LIST_DIR}/../upstream/src)
target_compile_features(DestroyThemMyRobots PRIVATE cxx_std_20)
target_include_directories(DestroyThemMyRobots PRIVATE ${CORE} ${CORE}/metamodule)
file(GLOB DSP CONFIGURE_DEPENDS ${CORE}/dsp/*.cpp)
file(GLOB PROCESSORS CONFIGURE_DEPENDS ${CORE}/metamodule/*_module.cpp)
file(GLOB REGISTRATION CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
target_sources(DestroyThemMyRobots PRIVATE ${DSP} ${PROCESSORS} ${REGISTRATION})
create_plugin(SOURCE_LIB DestroyThemMyRobots
 PLUGIN_NAME DestroyThemMyRobots
 PLUGIN_JSON ${CMAKE_CURRENT_LIST_DIR}/plugin.json
 SOURCE_ASSETS ${CMAKE_CURRENT_LIST_DIR}/assets
 DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../metamodule-plugins)
