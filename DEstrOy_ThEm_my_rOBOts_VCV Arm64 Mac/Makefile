RACK_DIR ?= ../Rack-SDK
FLAGS += -std=c++17 -Icompat -Iupstream/src -Iupstream/src/metamodule
SOURCES += src/plugin.cpp src/modules.cpp
SOURCES += $(wildcard upstream/src/dsp/*.cpp)
SOURCES += upstream/src/metamodule/transverb_module.cpp
SOURCES += upstream/src/metamodule/scrubby_module.cpp
SOURCES += upstream/src/metamodule/buffer_override_module.cpp
SOURCES += upstream/src/metamodule/skidder_module.cpp
SOURCES += upstream/src/metamodule/polarizer_module.cpp
DISTRIBUTABLES += res README.md NOTICE.md CLOCK_SYNC_GUIDE.md CONTROL_GUIDE.md
DISTRIBUTABLES += LICENSE LICENSING.md
include $(RACK_DIR)/plugin.mk
