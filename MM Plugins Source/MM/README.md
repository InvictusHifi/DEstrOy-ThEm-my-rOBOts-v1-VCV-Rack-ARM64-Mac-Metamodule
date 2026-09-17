# MetaModule build

This folder adds the MetaModule implementation matching the VCV 2.1.0 source
in the parent folder. Root VCV files are unchanged. The MM plugin is now
included in the outer distribution's metamodule-plugins/ folder.
The root README and VALIDATION describe the earlier VCV-only release;
this document and MM/VALIDATION.md cover this addition.

Use the manual **Build robots - MetaModule** workflow in GitHub Actions.
The existing Apple Silicon workflow still builds only VCV.

For a local build, obtain SDK commit
793a552f250826e39374fa493c9ec844de7356b8 with its submodules from
https://github.com/4ms/metamodule-plugin-sdk and ARM GNU Toolchain 12.3.Rel1.
Then, from the repository root:

```
cmake -S MM -B build-mm -G Ninja -DMETAMODULE_SDK_DIR=/absolute/path/to/sdk -DTOOLCHAIN_BASE_DIR=/absolute/path/to/arm-toolchain/bin
cmake --build build-mm -j3
```

Output is ../metamodule-plugins/DestroyThemMyRobots.mmplugin.
To regenerate the MM artwork run `bash scripts/prepare_mm_assets.sh`
(requires Inkscape). All exported images are already supplied.

The 193 controls and ports retain the VCV order and labels, with coordinates
scaled to millimetres. Parameter values are normalized 0–1 in both versions.
The native MM interface displays parameters as knobs, including parameters
that the VCV front end displays as buttons. Reset/trigger controls must be
returned low before retriggering, or driven by their trigger inputs.
This is the existing portable processor behavior, not a new DSP change.

Sharing: include the corresponding source and all notices; see LICENSING.md.
