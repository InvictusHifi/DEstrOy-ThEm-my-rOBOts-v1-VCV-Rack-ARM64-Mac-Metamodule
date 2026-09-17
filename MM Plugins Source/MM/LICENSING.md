# MetaModule licensing and source

The effects and new registration/layout changes are GPL-2.0-only under the
root LICENSE. Retain the root NOTICE.md and upstream/ORIGINAL_NOTICE.md.
Changes dated 2026-09-17: new MM identifiers, matching VCV control geometry,
240-pixel opaque panel exports and MetaModule packaging. DSP is unchanged.

The SDK is 4ms/metamodule-plugin-sdk commit
793a552f250826e39374fa493c9ec844de7356b8 (SDK 2.3.0).
https://github.com/4ms/metamodule-plugin-sdk/tree/793a552f250826e39374fa493c9ec844de7356b8
Its GPLv3-or-later licence includes the MetaModule Plugin License Exception,
permitting plugins to use the API and link under other licence terms.
Copies of the SDK licence and exception are in assets/licenses/.
The SDK is fetched separately by the build workflow, not relicensed here.

The SDK links its prebuilt GCC 12 runtime library. Standard libstdc++ and
libgcc use GPLv3 plus the GCC Runtime Library Exception; newlib components
retain their individual permissive notices. The runtime's SDK-supplied
licence notice is also preserved. These components are not GPLv2 code.
https://gcc.gnu.org/onlinedocs/libstdc++/manual/license.html
https://sourceware.org/newlib/COPYING.NEWLIB

Share the complete distribution with the exact corresponding source and
build instructions, not the binary alone. This package includes the source
of the effects, registration adapter, graphics inputs and build scripts.
Third-party SDK/runtime source locations are pinned or documented above.
No permission or endorsement from original effect authors is claimed.
No claim of a comprehensive legal audit is made.
