# Validation — 17 September 2026

This is a source release, not a precompiled macOS binary.

Completed locally:

- Compiled the complete plugin against Rack SDK 2.6.6 on Linux x86-64.
- Loaded that binary against libRack, called its actual initialization entry point, verified all five new model identifiers, instantiated each module and processed audio with finite outputs.
- Exercised all five portable audio processors at 44.1, 48 and 96 kHz for four seconds each. Outputs were finite with nonzero energy. Also exercised every parameter at both normalized endpoints with finite-output checks.
- Ran `make dist` and checked the distribution manifest, all five panel PNG dimensions, custom control resources and expected parameter/input/output counts.
- Rendered and visually inspected the production panel composition with the live-control artwork overlaid at the source-defined coordinates. This is a composed preview, not a Rack application screenshot.

The original DSP and portable processor wrappers are unchanged. Parameter and port enumeration order is preserved. Plugin/model identifiers, presentation, asset loading and widget layout are changed.

Not completed here:

- A macOS ARM64 build or interactive Rack session on a Mac. The included GitHub Actions workflow performs the ARM64 compile, binary architecture check and packaging when run in your repository.
- Exhaustive audio-quality, long-duration, patch migration, or every possible clock/CV interaction test. Smoke tests are not a guarantee of musical quality or complete functional correctness.
- MetaModule compilation or device testing. This package is VCV-only; its new identifiers do not match an unchanged MetaModule package.

## Repeatable checks

Run `python3 scripts/validate.py` and `bash scripts/test-dsp.sh` from the repository root. Run `make dist RACK_DIR=/absolute/path/to/Rack-SDK` with the SDK for your platform, then `python3 scripts/validate.py --dist dist/DestroyThemMyRobots`.

`tests/rack_load.cpp` is an additional Linux-only headless loading harness; link it against the SDK's libRack and libdl, and pass the built plugin.so path. It does not exercise GUI rendering.
