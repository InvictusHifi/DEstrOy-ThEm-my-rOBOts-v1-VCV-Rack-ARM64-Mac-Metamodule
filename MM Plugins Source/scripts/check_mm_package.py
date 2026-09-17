#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
import argparse,tarfile,json,subprocess,tempfile
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('archive',type=Path);p.add_argument('--readelf',required=True);a=p.parse_args()
with tarfile.open(a.archive) as t:
 names=t.getnames();markers=[n for n in names if '/SDK-' in n]
 assert markers==['DestroyThemMyRobots/SDK-2.3'],markers
 # Reproduce firmware untar_contents.hh find_last_of("/SDK-").
 marker=markers[-1];version=marker[max(marker.rfind(c) for c in '/SDK-')+1:]
 assert version=='2.3',version
 prefix='DestroyThemMyRobots/'
 manifest=json.load(t.extractfile(prefix+'plugin.json'))
 assert manifest['slug']=='DestroyThemMyRobots' and manifest['version']=='2.1.1'
 assert {m['slug'] for m in manifest['modules']}=={'DTMR-'+n for n in ['Transverb','Scrubby','BufferOverride','Skidder','Polarizer']}
 data=t.extractfile(prefix+'DestroyThemMyRobots.so').read()
 assert data[:4]==b'\x7fELF' and data[4]==1 and int.from_bytes(data[18:20],'little')==40
 with tempfile.TemporaryDirectory() as d:
  f=Path(d)/'plugin.so';f.write_bytes(data)
  syms=subprocess.check_output([a.readelf,'--dyn-syms','--wide',str(f)],text=True)
  for name in ['init','_ZN10MetaModule11sdk_versionEv']:
   matches=[line.split() for line in syms.splitlines() if line.split() and line.split()[-1]==name]
   assert any(row[3:6]==['FUNC','GLOBAL','DEFAULT'] and row[6]!='UND' for row in matches),name
print('PASS firmware parses SDK 2.3; MM 2.1.1 IDs; ARM binary; exported init and sdk_version')
