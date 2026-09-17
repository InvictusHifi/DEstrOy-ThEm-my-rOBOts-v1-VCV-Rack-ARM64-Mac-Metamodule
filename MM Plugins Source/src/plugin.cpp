#include "plugin.hpp"
Plugin* pluginInstance;
extern Model* modelTransverb;
extern Model* modelScrubby;
extern Model* modelBufferOverride;
extern Model* modelSkidder;
extern Model* modelPolarizer;

void init(Plugin* p) {
    pluginInstance = p;
    p->addModel(modelTransverb);
    p->addModel(modelScrubby);
    p->addModel(modelBufferOverride);
    p->addModel(modelSkidder);
    p->addModel(modelPolarizer);
}
