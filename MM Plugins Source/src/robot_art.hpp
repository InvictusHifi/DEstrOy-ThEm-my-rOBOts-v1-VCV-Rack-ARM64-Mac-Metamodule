// SPDX-License-Identifier: GPL-2.0-only
#pragma once
#include <rack.hpp>

// Raster panel artwork is loaded by Rack's image API. SVG <image> is not
// supported by Rack's panel renderer; do not replace this with createPanel().
struct RobotPanel : rack::widget::Widget {
    std::shared_ptr<rack::window::Image> image;
    explicit RobotPanel(const std::string& filename) {
        image = APP->window->loadImage(rack::asset::plugin(pluginInstance,filename));
    }
    void draw(const DrawArgs& args) override {
        nvgBeginPath(args.vg);
        nvgRect(args.vg,0,0,box.size.x,box.size.y);
        nvgFillColor(args.vg,nvgRGB(255,255,255));nvgFill(args.vg);
        if (image && image->handle >= 0) {
            nvgFillPaint(args.vg,nvgImagePattern(args.vg,0,0,box.size.x,box.size.y,0,image->handle,1));
            nvgFill(args.vg);
        }
        Widget::draw(args);
    }
};
struct RobotKnob : rack::app::SvgKnob {
    RobotKnob() {
        minAngle=-0.75f*M_PI;maxAngle=0.75f*M_PI;
        setSvg(APP->window->loadSvg(rack::asset::plugin(pluginInstance,"res/components/knob.svg")));
    }
};
struct RobotPort : rack::app::SvgPort {
    RobotPort(){setSvg(APP->window->loadSvg(rack::asset::plugin(pluginInstance,"res/components/port.svg")));}
};
struct RobotToggle : rack::app::SvgSwitch {
    RobotToggle(){
        addFrame(APP->window->loadSvg(rack::asset::plugin(pluginInstance,"res/components/button-0.svg")));
        addFrame(APP->window->loadSvg(rack::asset::plugin(pluginInstance,"res/components/button-1.svg")));
    }
};
struct RobotTrigger : RobotToggle {RobotTrigger(){momentary=true;}};
