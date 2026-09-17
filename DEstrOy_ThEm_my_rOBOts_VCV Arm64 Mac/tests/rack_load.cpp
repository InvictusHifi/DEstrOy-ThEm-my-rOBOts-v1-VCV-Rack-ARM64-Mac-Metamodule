// SPDX-License-Identifier: GPL-2.0-only
#include <rack.hpp>
#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <cmath>
int main(int argc,char**argv){
 if(argc!=2)return 2;
 void* lib=dlopen(argv[1],RTLD_NOW|RTLD_LOCAL);
 if(!lib){std::cerr<<dlerror()<<'\n';return 1;}
 auto init=reinterpret_cast<void(*)(rack::plugin::Plugin*)>(dlsym(lib,"init"));
 if(!init)return 3;
 rack::Context context;rack::contextSet(&context);
 rack::plugin::Plugin p;p.slug="DestroyThemMyRobots";init(&p);
 const char* expected[]={"DTMR-Transverb","DTMR-Scrubby","DTMR-BufferOverride","DTMR-Skidder","DTMR-Polarizer"};
 int k=0;for(auto*model:p.models){
  if(k>=5||model->slug!=expected[k++])return 4;
  std::unique_ptr<rack::engine::Module> m(model->createModule());
  m->inputs[0].setChannels(1);m->inputs[1].setChannels(1);
  rack::engine::Module::ProcessArgs a{};a.sampleRate=48000;a.sampleTime=1.f/48000;
  for(int i=0;i<4800;++i){a.frame=i;m->inputs[0].setVoltage(std::sin(i*.03f));m->inputs[1].setVoltage(std::sin(i*.05f));m->process(a);
   for(auto&o:m->outputs)if(!std::isfinite(o.getVoltage()))return 5;}
  std::cout<<"PASS loaded Rack model "<<model->slug<<", processed 4800 stereo frames\n";
 }
 return k==5?0:6;
}
