// SPDX-License-Identifier: GPL-2.0-only
#include "CoreModules/register_module.hh"
#include <cassert>
#include <cmath>
#include <iostream>
extern "C" void init();
int count=0;
namespace MetaModule {
bool register_module(std::string_view brand,std::string_view slug,CreateModuleFunc create,const ModuleInfoView& info,std::string_view panel){
 assert(brand=="DestroyThemMyRobots"); assert(slug.starts_with("DTMR-"));
 auto core=create(); core->set_samplerate(48000);
 std::cout<<"MODULE "<<slug<<" "<<info.width_hp<<" "<<panel<<"\n";
 for(size_t e=0;e<info.elements.size();++e){
  const auto& idx=info.indices[e];
  std::visit([&](const auto& v){
   std::cout<<"ELEMENT "<<e<<" "<<v.x_mm<<" "<<v.y_mm<<" "<<v.short_name<<"\n";
   if constexpr(std::is_same_v<std::decay_t<decltype(v)>,Knob>) {
    assert(std::abs(core->get_param(idx.param_idx)-v.default_value)<0.0001f);
   }
  },info.elements[e]);
 }
 core->mark_input_patched(0); core->mark_input_patched(1);
 for(int n=0;n<192000;++n){core->set_input(0,std::sin(n*.05f));core->set_input(1,std::sin(n*.07f));core->update();assert(std::isfinite(core->get_output(0)));assert(std::isfinite(core->get_output(1)));}
 ++count; return true;
}
}
int main(){init();assert(count==5);}
