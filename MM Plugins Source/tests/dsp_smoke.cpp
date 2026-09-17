// SPDX-License-Identifier: GPL-2.0-only
#include "metamodule/transverb_module.hpp"
#include "metamodule/scrubby_module.hpp"
#include "metamodule/buffer_override_module.hpp"
#include "metamodule/skidder_module.hpp"
#include "metamodule/polarizer_module.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
template<class M>void run(const char*name){
 for(float sr:{44100.f,48000.f,96000.f}){
  M m;m.set_samplerate(sr);m.mark_input_patched(0);m.mark_input_patched(1);
  for(int p=0;p<M::NumParams;++p){float v=m.get_param(p);if(!std::isfinite(v)||v<0||v>1)throw std::runtime_error("default outside normalized range");}
  double energy=0;
  // Scrubby starts fully wet and can read unwritten buffer regions initially.
  // Allow the default multi-second buffer to fill before testing audibility.
  for(int i=0;i<int(sr*4);++i){
   m.set_input(0,std::sin(i*.04f));m.set_input(1,std::sin(i*.061f));m.update();
   for(int c=0;c<2;++c){float y=m.get_output(c);if(!std::isfinite(y))throw std::runtime_error("nonfinite audio");energy+=y*y;}
  }
  if(energy<1e-6)throw std::runtime_error("silent default");
  // Exercise each parameter endpoint with settling time; no audio NaNs allowed.
  for(int p=0;p<M::NumParams;++p){float def=m.get_param(p);
   for(float value:{0.f,1.f}){m.set_param(p,value);for(int i=0;i<256;++i){m.update();for(int c=0;c<2;++c)if(!std::isfinite(m.get_output(c)))throw std::runtime_error("endpoint NaN");}}
   m.set_param(p,def);
  }
  std::cout<<"PASS "<<name<<" at "<<sr<<" Hz, default stereo signal and parameter endpoints\n";
 }
}
int main(){try{run<TransverbModule>("Transverb");run<ScrubbyModule>("Scrubby");run<BufferOverrideModule>("Buffer Override");run<SkidderModule>("Skidder");run<PolarizerModule>("Polarizer");}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
