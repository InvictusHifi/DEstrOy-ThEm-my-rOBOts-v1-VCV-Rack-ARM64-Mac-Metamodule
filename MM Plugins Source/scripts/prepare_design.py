#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Rebuild production artwork/layout from the approved, bundled concept image."""
import json, math, re, subprocess, html
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
BLUE = '#00449b'
BRAND = 'DEstrOy ThEm my rOBOts'
# Measured strip boundaries in the production header atlas. Its generated
# artwork contains no controls; all functional labels are deterministic below.
ART = {'Transverb':(0,0,1145,287), 'Scrubby':(0,290,1145,270),
       'BufferOverride':(0,563,1145,303), 'Skidder':(0,870,1145,257),
       'Polarizer':(0,1132,1145,242)}
def text(x,y,label,size=8,anchor='middle'):
    return f'<text x="{x}" y="{y}" font-family="DejaVu Sans" font-size="{size}" font-weight="bold" text-anchor="{anchor}" fill="{BLUE}">{html.escape(label)}</text>'
def fragment(rect,x,y,w,h):
    a,b,c,d=rect
    scale=min(w/c,h/d); xx=x+(w-c*scale)/2; yy=y+(h-d*scale)/2
    key='crop'+str(a)+str(b)
    return f'<defs><clipPath id="{key}" clipPathUnits="userSpaceOnUse"><rect x="{xx}" y="{yy}" width="{c*scale}" height="{d*scale}"/></clipPath></defs><g clip-path="url(#{key})"><image xlink:href="../header-atlas.png" width="1145" height="1374" transform="translate({xx-a*scale},{yy-b*scale}) scale({scale})"/></g>'
def svg(w,h,body):
    return f'<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="{w}" height="{h}" viewBox="0 0 {w} {h}">{body}</svg>'
def main():
    src=(ROOT/'src/modules.cpp').read_text()
    specs=[]
    pattern=r'static const ModuleSpec s\{"([^"]+)", "([^"]+)", \{([^}]+)\}, \{([^}]+)\}, \{([^}]+)\}, (\d+)\};'
    for match in re.finditer(pattern,src):
        slug,panel,params,inputs,outputs,hp=match.groups()
        name=slug.removeprefix('DTMR-')
        p,i,o=[re.findall(r'"([^"]+)"',x) for x in (params,inputs,outputs)]
        w=int(hp)*15; large=len(p)>16 or len(i)>12; cols=9 if large else 6; pc=10 if large else 8
        ps=[(26+(w-52)*(k%cols+.5)/cols,172+(k//cols)*44) for k in range(len(p))]
        ports=[(26+(w-52)*(k%pc+.5)/pc,312+(k//pc)*39) for k in range(len(i)+len(o))]
        body='<rect width="100%" height="100%" fill="white"/>'
        body+=f'<rect x="2" y="2" width="{w-4}" height="376" rx="5" fill="none" stroke="{BLUE}" stroke-width="1.5"/>'
        body+=text(18,23,BRAND,15,'start')
        body+=fragment(ART[name],14,30,w-28,111)
        body+=f'<path d="M12 146 H{w-12}" stroke="{BLUE}" stroke-width="1.4"/>'
        for k,((x,y),label) in enumerate(zip(ps,p)):
            body+=text(x,y-18,label,7.4 if len(label)<13 else 6.6)
            body+=f'<circle cx="{x}" cy="{y}" r="15" fill="none" stroke="{BLUE}" stroke-width="0.65"/>'
            for step in range(9):
                angle=math.radians(-225+step*270/8)
                x1=x+16*math.cos(angle);y1=y+16*math.sin(angle)
                x2=x+18*math.cos(angle);y2=y+18*math.sin(angle)
                body+=f'<path d="M{x1} {y1} L{x2} {y2}" stroke="{BLUE}" stroke-width="0.65"/>'
        body+=f'<path d="M12 283 H{w-12}" stroke="{BLUE}" stroke-width="1"/>'
        for k,((x,y),label) in enumerate(zip(ports,i+o)):
            if k>=len(i):
                body+=f'<rect x="{x-25}" y="{y-25}" width="50" height="38" rx="4" fill="#e7f0fc"/>'
            if label.endswith(' CV'):
                body+=text(x,y-21,label[:-3].upper(),6.2)
                body+=text(x,y-14,'CV',5.5)
            else:
                body+=text(x,y-15,label.upper(),6.8 if len(label)>10 else 7)
            body+=f'<circle cx="{x}" cy="{y}" r="11" fill="white" stroke="{BLUE}" stroke-width="1"/>'
        body+=text(14,373,'DTMR / '+name.upper(),6,'start')
        body+=text(w-14,373,'GPLv2 · STEREO / CV',6,'end')
        path=ROOT/'design/panels'/f'{name}.svg';path.parent.mkdir(exist_ok=True)
        path.write_text(svg(w,380,body))
        subprocess.run(['inkscape',str(path),'--export-width='+str(w*2),'--export-filename='+str(ROOT/'res'/f'{name}.png')],check=True,capture_output=True)
        specs.append(dict(slug='DTMR-'+name,name=name,hp=int(hp),params=p,inputs=i,outputs=o,param_positions=ps,port_positions=ports))
    assert len(specs)==5
    (ROOT/'design/layout.json').write_text(json.dumps(specs,indent=2)+'\n')
    # All control artwork is new; no VCV/Grayscale artwork is redistributed.
    knob='<circle cx="12" cy="12" r="11.5" fill="white" stroke="#00449b"/><circle cx="12" cy="12" r="9.7" fill="#00449b"/><path d="M12 3.8 V9" stroke="white" stroke-width="2.2" stroke-linecap="round"/>'
    port='<circle cx="10" cy="10" r="9.5" fill="#e8eef5" stroke="#00449b"/><circle cx="10" cy="10" r="6.8" fill="white" stroke="#00449b"/><circle cx="10" cy="10" r="4.7" fill="#10243c"/>'
    (ROOT/'res/components/knob.svg').write_text(svg(24,24,knob))
    (ROOT/'res/components/port.svg').write_text(svg(20,20,port))
    for k in range(2):
        body=f'<circle cx="12" cy="12" r="10.5" fill="white" stroke="{BLUE}" stroke-width="2"/><circle cx="12" cy="12" r="7.5" fill="{BLUE if k else "#dfebfc"}"/><path d="M9 12 H15" stroke="{"white" if k else BLUE}" stroke-width="2"/>'
        (ROOT/f'res/components/button-{k}.svg').write_text(svg(24,24,body))
    print('Generated five production panels, component graphics and layout manifest.')
if __name__=='__main__':main()
