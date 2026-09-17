void register_transverb_module();
void register_scrubby_module();
void register_buffer_override_module();
void register_skidder_module();
void register_polarizer_module();

extern "C" __attribute__((visibility("default"))) void init() {
    register_transverb_module();
    register_scrubby_module();
    register_buffer_override_module();
    register_skidder_module();
    register_polarizer_module();
}
