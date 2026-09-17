# MM 2.1.1 loading correction

The original 2.1.0 archive reproduced the reported SDK version E using the
firmware untar_contents.hh parser. A licence filename containing /SDK-
overrode the real version marker. Renamed the notice without changing its
contents. The repaired archive has exactly one /SDK- path: SDK-2.3.
Explicitly exported init() under the SDK hidden-visibility compiler options.

Package regression check: scripts/check_mm_package.py verifies the actual
archive marker with the firmware parsing algorithm, plugin/module slugs,
ARM ELF type, and exported init/sdk_version dynamic symbols.
Source DSP and registration mappings remain unchanged.
Physical hardware loading and performance remain untested locally.

Firmware source examined: 4ms/metamodule commit
5a9280ec752cf5afa11d2ea64e9cb168676af826,
firmware/src/dynload/untar_contents.hh.
SDK remains 793a552f250826e39374fa493c9ec844de7356b8 (2.3.0).
