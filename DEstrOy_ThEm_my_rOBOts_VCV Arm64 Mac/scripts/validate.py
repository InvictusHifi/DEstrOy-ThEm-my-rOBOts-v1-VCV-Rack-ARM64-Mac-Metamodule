#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
import argparse,json,re,struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
ap=argparse.ArgumentParser();ap.add_argument('--dist',type=Path);args=ap.parse_args()
base=args.dist or ROOT
m=json.loads((base/'plugin.json').read_text())
assert m['slug']=='DestroyThemMyRobots'
assert m['name']==m['brand']=='DEstrOy ThEm my rOBOts'
assert m['version']=='2.1.0' and m['license']=='GPL-2.0-only'
names=['Transverb','Scrubby','BufferOverride','Skidder','Polarizer']
assert [x['slug'] for x in m['modules']]==['DTMR-'+n for n in names]
layout=json.loads((ROOT/'design/layout.json').read_text())
assert [x['name'] for x in layout]==names
src=(ROOT/'src/modules.cpp').read_text()
counts={'Transverb':(26,17,2),'Scrubby':(27,18,2),'BufferOverride':(23,16,2),'Skidder':(18,13,2),'Polarizer':(15,10,2)}
for item in layout:
 name=item['name']; assert f'>>("DTMR-{name}")' in src
 assert tuple(len(item[k]) for k in ('params','inputs','outputs'))==counts[name]
 png=(base/'res'/f'{name}.png').read_bytes()
 assert png[:8]==b'\x89PNG\r\n\x1a\n'
 w,h=struct.unpack('>II',png[16:24]);assert (w,h)==(item['hp']*30,760)
 assert len(item['param_positions'])==len(item['params'])
 assert len(item['port_positions'])==len(item['inputs'])+len(item['outputs'])
 pts=item['param_positions']+item['port_positions']
 for x,y in pts:assert 12<=x<=w/2-12 and 146<y<366
 for i,(x,y) in enumerate(pts):
  for xx,yy in pts[i+1:]:assert (x-xx)**2+(y-yy)**2>=24**2
 print('PASS',item['slug'],'controls/inputs/outputs',counts[name],'panel',w,h)
for f in ['knob.svg','port.svg','button-0.svg','button-1.svg']:
 assert (base/'res/components'/f).is_file()
assert (base/'LICENSE').is_file()
print('PASS brand, new IDs, Rack 2 version, GPLv2 manifest, panels and component files')
