#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-only
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/tests
"${CXX:-c++}" -std=c++17 -O2 -Icompat -Iupstream/src tests/dsp_smoke.cpp \
 upstream/src/dsp/*.cpp upstream/src/metamodule/*_module.cpp -o build/tests/dsp-smoke
build/tests/dsp-smoke
