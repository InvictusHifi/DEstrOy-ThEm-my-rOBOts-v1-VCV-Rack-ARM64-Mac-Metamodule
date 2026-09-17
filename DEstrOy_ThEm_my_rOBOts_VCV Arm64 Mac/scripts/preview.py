#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Preview the actual raster panels with the actual widget SVGs overlaid."""
from pathlib import Path
import json,subprocess
ROOT=Path(__file__).resolve().parents[1]
layout=json.loads((ROOT/'design/layout.json').read_text())
parts=['<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="1340" height="1240"><rect width="1340" height="1240" fill="#edf3fb"/>']
for n,item in enumerate(layout):
 x=25+(n%2)*655;y=20+(n//2)*408;w=item['hp']*15
 parts.append(f'<g transform="translate({x},{y})"><image xlink:href="../res/{item["name"]}.png" width="{w}" height="380"/>')
 for name,(cx,cy) in zip(item['params'],item['param_positions']):
  f='button-0.svg' if name in ['Freeze','Clock Lock','Capture','Jump','Reset'] else 'knob.svg'
  parts.append(f'<image x="{cx-12}" y="{cy-12}" width="24" height="24" xlink:href="../res/components/{f}"/>')
 for cx,cy in item['port_positions']:
  parts.append(f'<image x="{cx-10}" y="{cy-10}" width="20" height="20" xlink:href="../res/components/port.svg"/>')
 parts.append('</g>')
parts.append('</svg>')
path=ROOT/'design/production-overview.svg';path.write_text(''.join(parts))
subprocess.run(['inkscape',str(path),'--export-filename='+str(ROOT/'design/production-overview.png')],check=True,capture_output=True)
