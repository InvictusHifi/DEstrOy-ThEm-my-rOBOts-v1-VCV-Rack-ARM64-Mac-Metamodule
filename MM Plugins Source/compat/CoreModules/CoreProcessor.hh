#pragma once

class CoreProcessor {
public:
    virtual ~CoreProcessor() = default;
    virtual void update() = 0;
    virtual void set_samplerate(float) = 0;
    virtual void set_param(int, float) = 0;
    virtual float get_param(int) const = 0;
    virtual void set_input(int, float) = 0;
    virtual float get_output(int) const = 0;
    virtual void mark_all_inputs_unpatched() = 0;
    virtual void mark_input_unpatched(int) = 0;
    virtual void mark_input_patched(int) = 0;
    bool bypassed = false;
};
