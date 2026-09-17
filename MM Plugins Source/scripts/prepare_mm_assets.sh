#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-only
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p MM/assets/components
for name in Transverb Scrubby BufferOverride Skidder Polarizer; do
  inkscape "design/panels/$name.svg" --export-height=240 --export-background=white --export-background-opacity=1 --export-png-color-mode=RGB_8 --export-filename="MM/assets/$name.png"
done
inkscape res/components/knob.svg --export-height=15 --export-filename=MM/assets/components/knob.png
inkscape res/components/port.svg --export-height=13 --export-filename=MM/assets/components/jack.png
