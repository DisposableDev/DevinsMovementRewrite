#!/usr/bin/env python3
"""Build the Physics Lab's render and collision meshes from the same geometry.

Run from any directory. No third-party packages are needed. Generated C files
are checked in; normal ROM builds do not need to run this script.
"""

import argparse
from collections import defaultdict
from dataclasses import dataclass, replace
import json
import math
from pathlib import Path

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "levels/physics_lab"
DEFAULT = "SURFACE_DEFAULT"
AERIAL_CAMERA_HALF_WIDTH = 4096
TERRAIN_GROUP_MAX_SPAN = 8192
# Keep northern edge faces so the backface stream also covers the wide views
# inside the level boundary. Full-list interior views already have headroom.
NORTH_BACKFACE_PLANE_Z = -12000
COLORS = {
    "ground": (153, 170, 171), "runway": (195, 205, 193),
    "normal": (196, 172, 127), "slippery": (91, 161, 191),
    "ice": (155, 217, 235), "grip": (152, 177, 110),
    "wall": (132, 145, 151), "tower": (205, 180, 137),
    "pool": (107, 160, 171), "water": (55, 153, 205),
    "lava": (235, 83, 35), "sand": (215, 185, 98),
    "wind": (184, 176, 216), "vanish": (104, 131, 228),
    "platform": (211, 157, 87), "pole": (205, 86, 59),
    "very_slippery": (46, 120, 190), "route": (205, 211, 196),
    "court": (172, 182, 180), "current": (67, 133, 155),
    "label": (52, 64, 72),
}

# Walkable physics surfaces use the same palette on every fixture. Structural
# sides retain their neutral material; sculptures use contrasting facet shades.
SURFACE_COLORS = dict(zip(
    ["SURFACE_SLIPPERY", "SURFACE_VERY_SLIPPERY", "SURFACE_NOT_SLIPPERY",
     "SURFACE_ICE", "SURFACE_VANISH_CAP_WALLS", "SURFACE_FLOWING_WATER",
     "SURFACE_BURNING", "SURFACE_SHALLOW_QUICKSAND", "SURFACE_SHALLOW_MOVING_QUICKSAND",
     "SURFACE_HORIZONTAL_WIND"],
    [COLORS[k] for k in ("slippery", "very_slippery", "grip", "ice", "vanish", "current",
                        "lava", "sand", "sand", "wind")]))

# Ground-level circulation, with no raised lips or collision overlays.
ROUTES = [(-7500,13300,-7000,-6300), (-7350,-6650,-7000,6800),
          (12500,13300,-7000,14900), (-7500,13300,6200,6800),
          (-7500,13300,650,950), (-12800,-6700,5400,6000),
          (-2800,-2100,6800,14900), (-12500,-2100,10500,11000),
          (-12500,13300,14800,15100), (6200,6900,6800,14900),
          (600,1200,-12700,-7000), (-13400,-12700,-11000,5400),
          (-12700,600,-12800,-12400), (12700,13800,2200,2800),
          (-7900,-7300,11000,11650), (-6200,-5800,-12400,-10900),
          (-6950,-5800,-11100,-10700), (6900,7500,7800,8250),
          (-4400,-3800,11000,11200), (-3050,-2800,13100,13700)]

SEAM_SITES = [(-8500,11650), (-8900,-11100)]
IMPACT_BEDS = [(7500,8600,0), (8900,10000,60), (10300,11400,600)]
RELEASE_HEIGHTS = (1700,3600)

# Enemy staging uses ordinary dry ground. Raised walls only separate ground
# comparison cells and firing bays; airborne enemies keep their native ranges.
BULLY_COURTS = [(-11450,-10050,800,2200), (-11450,-10050,3500,4900)]
ENEMY_COURTS = [
    (7500,10800,-8850,-7250), (10800,13700,-12700,-7250),
    (-12600,-11750,-12350,-7500), (-11500,-9950,-4600,5300),
    (-6350,-5050,11200,14600),
]
# Each tuple describes a transverse entry line and the direction toward its
# target: 0 = north, 90 = east, 180 = south, 270 = west.
ENEMY_ENTRIES = [
    (2100,-11475,0), (3300,-11475,0), (4500,-11475,0), (5700,-11475,0),
    (3300,-10875,0), (4500,-10875,0),
    (8100,-7350,0), (9800,-8150,90),
    (11200,-10850,0), (12300,-10850,0), (13400,-10850,0),
    (13050,-8500,0), (-12200,-12300,180), (-12200,-10450,180),
    (-12100,-7550,0), (-9900,-2600,270), (-10800,-1000,180),
    (-10800,2400,0), (-10800,5100,0),
    (-13700,10650,0), (-13700,12250,180),
    (-6140,11600,90), (-6140,12400,90), (-6140,13200,90),
    (-6140,13900,90), (-6140,14300,90),
    (6200,6050,0), (5000,9950,0), (5200,14550,0),
    (2700,12100,180), (2700,14500,0),
    (-12250,6400,270), (-14600,8650,0), (13200,12850,90),
]

# Small flush bay letters use the same neutral paint as the walking routes.
BAY_MARKS = [(-7600,11350,"A"), (-6000,-12300,"B"), (7150,8025,"C"),
             (-4100,11350,"D"), (-1325,3575,"E")]
BAY_GLYPHS = {
    "A": ["010","101","111","101","101"],
    "B": ["110","101","110","101","110"],
    "C": ["011","100","100","100","011"],
    "D": ["110","101","101","101","110"],
    "E": ["111","100","110","100","111"],
}

# Flush labels share the bay letters' small block font and add no collision.
LABEL_GLYPHS = {
    **BAY_GLYPHS,
    "F": ["111", "100", "110", "100", "100"],
    "G": ["011", "100", "101", "101", "011"],
    "H": ["101", "101", "111", "101", "101"],
    "I": ["111", "010", "010", "010", "111"],
    "L": ["100", "100", "100", "100", "111"],
    "M": ["101", "111", "111", "101", "101"],
    "N": ["101", "111", "111", "111", "101"],
    "O": ["010", "101", "101", "101", "010"],
    "P": ["110", "101", "110", "100", "100"],
    "Q": ["010", "101", "101", "011", "001"],
    "R": ["110", "101", "110", "101", "101"],
    "S": ["011", "100", "010", "001", "110"],
    "T": ["111", "010", "010", "010", "010"],
    "U": ["101", "101", "101", "101", "111"],
    "V": ["101", "101", "101", "101", "010"],
    "W": ["101", "101", "111", "111", "101"],
    "0": ["111", "101", "101", "101", "111"],
    "1": ["010", "110", "010", "010", "111"],
    "2": ["110", "001", "010", "100", "111"],
    "4": ["101", "101", "111", "001", "001"],
    "6": ["011", "100", "111", "101", "111"],
    "8": ["111", "101", "111", "101", "111"],
    "-": ["000", "000", "111", "000", "000"],
    " ": ["000", "000", "000", "000", "000"],
}

# Floor directions are read facing north from the forecourt and junctions.
DIRECTORY_ARROWS = {
    "W": ["00100", "01100", "11111", "01100", "00100"],
    "SW": ["00001", "00010", "10100", "11000", "11100"],
    "E": ["00100", "00110", "11111", "00110", "00100"],
}

# Basin bounds also drive the water boxes. The cold bounds match physics_lab.c.
WATER_BOXES = [(0,2400,1000,6400,6000,-40), (1,6400,1000,11600,6000,-40),
               (2,3300,7800,5700,9700,180), (3,3300,11400,5700,14300,600),
               (4,7300,9900,12100,14600,-40), (50,-9900,3400,-7400,5300,550),
               (5,8900,8250,10000,9500,0), (6,10300,8250,11400,9500,0)]


@dataclass
class Face:
    points: tuple
    color: tuple
    surface: str | None
    force: int = 0
    layer: str = "opaque"
    checker_color: tuple | None = None # Untinted base color of the ground grid.
    lod_group: str | None = None


def normal(points):
    a, b, c = points[:3]
    u, v = [b[i] - a[i] for i in range(3)], [c[i] - a[i] for i in range(3)]
    return (u[1]*v[2] - u[2]*v[1], u[2]*v[0] - u[0]*v[2], u[0]*v[1] - u[1]*v[0])


def shaded_color(points, color):
    # Soft overhead light agrees with the vertical object shadows and stays
    # consistent when the small sculptures reuse the mesh rotated 180 degrees.
    n=normal(points)
    length=math.sqrt(sum(v*v for v in n))
    shade=.72+.28*max(0,n[1]/length)
    return tuple(round(c*shade) for c in color)


class Mesh:
    def __init__(self):
        self.faces = []
        self.boxes = [] # Authored opaque solids, used only by render cleanup.
        self.end_caps = [] # Closed slab/wedge ends and optional sealing ground bounds.
        self.lods = {} # Render-only replacements; collision always uses faces.
        self.label_textures = [] # Complete solid label footprints, render only.
        self.covered_ground = [] # Fully covered ground footprints, render only.

    def register_lod(self, name, first, distant):
        assert name not in self.lods
        for face in self.faces[first:]:
            face.lod_group = name
        self.lods[name] = distant.faces

    def face(self, points, color, surface=DEFAULT, force=0, layer="opaque"):
        points = tuple(tuple(round(v) for v in p) for p in points)
        # Collapse repeated endpoints on wedge sides to a triangle.
        points = tuple(dict.fromkeys(points))
        if len(points) < 3:
            return
        n = normal(points)
        assert sum(v*v for v in n) > 0, points
        if surface in SURFACE_COLORS and n[1] > 0 and layer == "opaque":
            color = SURFACE_COLORS[surface]
        self.faces.append(Face(points, color, surface, force, layer))
        return self.faces[-1]

    def floor(self, x0, x1, z0, z1, y=0, color="ground", surface=DEFAULT, force=0, tile=800, aligned=False):
        """Upward-facing rectangle; aligned ground tiles provide the scale grid."""
        xs=[x0]+list(range((math.floor(x0/tile)+1)*tile,math.ceil(x1),tile))+[x1]
        zs=[z0]+list(range((math.floor(z0/tile)+1)*tile,math.ceil(z1),tile))+[z1]
        if not aligned:
            nx,nz=max(1,math.ceil((x1-x0)/tile)),max(1,math.ceil((z1-z0)/tile))
            xs=[x0+(x1-x0)*i/nx for i in range(nx+1)]
            zs=[z0+(z1-z0)*i/nz for i in range(nz+1)]
        rgb = COLORS.get(color, color)
        for c,d in zip(zs,zs[1:]):
            for a,b in zip(xs,xs[1:]):
                # Keep individual fixtures solid-colored: restarting a checker
                # on every box made adjacent decks and ramps look mismatched.
                tint = .92 if aligned and (math.floor(a/tile)+math.floor(c/tile)) % 2 == 0 else 1.0
                col = tuple(round(v*tint) for v in rgb)
                self.face([(a,y,c),(a,y,d),(b,y,d),(b,y,c)], col, surface, force)

    def box(self, x0,x1,z0,z1,y0,y1,color="wall",surface=DEFAULT,bottom=False,ceiling=DEFAULT):
        if surface != "SURFACE_VANISH_CAP_WALLS" and ceiling != "SURFACE_VANISH_CAP_WALLS":
            self.boxes.append((tuple(round(v) for v in (x0,y0,z0)),
                               tuple(round(v) for v in (x1,y1,z1)),bottom))
        rgb = COLORS.get(color, color)
        self.floor(x0,x1,z0,z1,y1,rgb,surface)
        self.face([(x0,y0,z0),(x0,y1,z0),(x1,y1,z0),(x1,y0,z0)],rgb,surface)
        self.face([(x1,y0,z1),(x1,y1,z1),(x0,y1,z1),(x0,y0,z1)],rgb,surface)
        self.face([(x0,y0,z1),(x0,y1,z1),(x0,y1,z0),(x0,y0,z0)],rgb,surface)
        self.face([(x1,y0,z0),(x1,y1,z0),(x1,y1,z1),(x1,y0,z1)],rgb,surface)
        if bottom:
            self.face([(x0,y0,z0),(x1,y0,z0),(x1,y0,z1),(x0,y0,z1)],rgb,ceiling)

    def ramp(self,x0,x1,z0,z1,y0,y1,color="normal",surface=DEFAULT,axis="z",base=0,force=0):
        """Solid wedge, rising along +z or +x; base must be below its top."""
        rgb = COLORS[color]
        if axis == "z":
            top = [(x0,y0,z0),(x0,y1,z1),(x1,y1,z1),(x1,y0,z0)]
        else:
            top = [(x0,y0,z0),(x0,y0,z1),(x1,y1,z1),(x1,y1,z0)]
        self.face(top,rgb,surface,force)
        for a,b in zip(top,top[1:]+top[:1]):
            cap = self.face([b,a,(a[0],base,a[2]),(b[0],base,b[2])],rgb,DEFAULT)
            # A buried base is sealed by ground too, provided the entire
            # riding surface stays above it. Ground coverage is checked later.
            if (cap is not None and base <= 0 <= min(y0,y1)
                    and surface != "SURFACE_VANISH_CAP_WALLS"):
                self.end_caps.append((cap, (x0,x1,z0,z1)))

    def wall(self,a,b,height=900,thickness=100,color="wall",surface=DEFAULT,base=0):
        dx,dz = b[0]-a[0],b[1]-a[1]
        length = math.hypot(dx,dz)
        ox,oz = -dz/length*thickness/2,dx/length*thickness/2
        footprint = [(a[0]+ox,a[1]+oz),(b[0]+ox,b[1]+oz),
                     (b[0]-ox,b[1]-oz),(a[0]-ox,a[1]-oz)]
        # This ordering has an upward-facing top.
        top = [(x,base+height,z) for x,z in footprint]
        if normal(top)[1] < 0:
            top.reverse()
        rgb=COLORS[color]
        self.face(top,rgb,surface)
        for p,q in zip(top,top[1:]+top[:1]):
            self.face([q,p,(p[0],base,p[2]),(q[0],base,q[2])],rgb,surface)

    def triangles(self):
        for face in self.faces:
            for i in range(1,len(face.points)-1):
                yield face,(face.points[0],face.points[i],face.points[i+1])

    def slab(self,x0,x1,z0,z1,y0,y1,thickness=150,color="pool",axis="z",surface=DEFAULT):
        """Sloping solid slab with a matching, downward-facing underside."""
        if axis == "z":
            bottom=[(x0,y0,z0),(x0,y1,z1),(x1,y1,z1),(x1,y0,z0)]
        else:
            bottom=[(x0,y0,z0),(x0,y0,z1),(x1,y1,z1),(x1,y1,z0)]
        top=[(x,y+thickness,z) for x,y,z in bottom]
        self.face(top,COLORS[color],surface)
        self.face(bottom[::-1],COLORS[color])
        for i in range(4):
            j=(i+1)%4
            cap = self.face([bottom[j],top[j],top[i],bottom[i]],COLORS[color])
            if cap is not None and surface != "SURFACE_VANISH_CAP_WALLS":
                self.end_caps.append((cap, None))


def place(m, builder, dx=0, dz=0, quarter_turn=False):
    fixture=Mesh()
    builder(fixture)
    transformed = {}
    for f in fixture.faces:
        points=[((z if quarter_turn else x)+dx,y,(-x if quarter_turn else z)+dz)
                for x,y,z in f.points]
        m.faces.append(Face(tuple(points),f.color,f.surface,f.force,f.layer,f.checker_color,f.lod_group))
        transformed[id(f)] = m.faces[-1]
    for cap, ground in fixture.end_caps:
        if ground is not None:
            x0,x1,z0,z1 = ground
            corners = [((z if quarter_turn else x)+dx,(-x if quarter_turn else z)+dz)
                       for x in (x0,x1) for z in (z0,z1)]
            ground = (min(p[0] for p in corners),max(p[0] for p in corners),
                      min(p[1] for p in corners),max(p[1] for p in corners))
        m.end_caps.append((transformed[id(cap)], ground))
    for name, faces in fixture.lods.items():
        assert name not in m.lods
        m.lods[name] = [Face(tuple(((z if quarter_turn else x)+dx,y,(-x if quarter_turn else z)+dz)
                                  for x,y,z in f.points),f.color,f.surface,f.force,f.layer) for f in faces]
    for lo,hi,closed in fixture.boxes:
        corners=[((z if quarter_turn else x)+dx,y,(-x if quarter_turn else z)+dz)
                 for x in (lo[0],hi[0]) for y in (lo[1],hi[1]) for z in (lo[2],hi[2])]
        m.boxes.append((tuple(min(p[a] for p in corners) for a in range(3)),
                        tuple(max(p[a] for p in corners) for a in range(3)),closed))


def add_ground(m):
    m.floor(-15200,15200,-15200,15200,tile=1000,aligned=True)
    for face in m.faces:
        face.checker_color=COLORS["ground"]
    # Split only the affected ground tiles: no overlapping floor or paint quads.
    regions=[(2400,11600,1000,6000,None,None,0),
             (7300,12100,9900,14600,None,None,0),
             (-6600,-6200,-300,100,None,None,0),
             # The rotated uneven-terrain patch supplies its own continuous
             # floor. A second floor here z-fights with its zero-height edges.
             (-9500,-7900,-4300,500,None,None,0)]
    regions += [(a,b,c,d,"route",DEFAULT,0) for a,b,c,d in ROUTES]
    regions += [(-7000,7000,-1000,1000,"runway",DEFAULT,0),
                (-6400,-4800,4000,5800,"lava","SURFACE_BURNING",0),
                (-4400,-3000,4000,5800,"sand","SURFACE_SHALLOW_QUICKSAND",0),
                (-2600,-1000,4000,5800,"wind","SURFACE_HORIZONTAL_WIND",0x40),
                # Court paint stops at the inside faces of its surrounding walls.
                (1400,6400,-12700,-10800,"court",DEFAULT,0),
                (13700,14900,11400,14300,"court",DEFAULT,0),
                (-14900,-12700,4300,8500,"court",DEFAULT,0),
                (-14900,-12500,9000,14400,"court",DEFAULT,0),
                (7500,8600,8250,9500,"normal",DEFAULT,0),
                (-5000,-3050,11200,14600,"court",DEFAULT,0),
                (-1650,-1000,3300,3850,"court",DEFAULT,0)]
    regions += [(x0,x1,z0,z1,"court",DEFAULT,0) for x0,x1,z0,z1 in ENEMY_COURTS]
    entry_lines = set()
    for x,z,yaw in ENEMY_ENTRIES:
        if x == -6140:
            # One continuous reference shares the same approach distance at
            # every firing bay, avoiding five separate ground subdivisions.
            bounds = (-6170,-6110,11300,14500)
        elif z in (-11475,-10875):
            bounds = (1400,6400,z-30,z+30)
        elif z == -10850:
            bounds = (10800,13700,z-30,z+30)
        elif x in (-12200,-12100) and z < -7500:
            bounds = (-12600,-11750,z-30,z+30)
        elif x == -10800 and z in (-1000,2400,5100):
            bounds = (-11500,-9950,z-30,z+30)
        elif x == -13700:
            bounds = (-14900,-12500,z-30,z+30)
        elif x in (5000,5200) and z in (9950,14550):
            bounds = (3300,5700,z-30,z+30)
        elif (x,z) == (6200,6050):
            bounds = (2400,11600,z-30,z+30)
        elif yaw in (0,180):
            bounds = (x-240,x+240,z-30,z+30)
        else:
            bounds = (x-30,x+30,z-240,z+240)
        entry_lines.add(bounds)
    regions += [(*bounds,"wall",DEFAULT,0) for bounds in sorted(entry_lines)]
    # Seam specimens replace the base floor, including their coplanar edges.
    regions += [(x,x+1950,z,z+2150,None,None,0) for x,z in SEAM_SITES]
    regions += [(x0,x1,8250,9500,None,None,0) for x0,x1,depth in IMPACT_BEDS if depth]
    for x,z,letter in BAY_MARKS:
        for row,pixels in enumerate(BAY_GLYPHS[letter]):
            for column,pixel in enumerate(pixels):
                if pixel == "1":
                    left,top=x-150+column*100,z-200+row*80
                    regions.append((left,left+100,top,top+80,"wall",DEFAULT,0))
    for x0,x1,z0,z1,color,surface,force in regions:
        clipped=[]
        for f in m.faces:
            a,b=min(p[0] for p in f.points),max(p[0] for p in f.points)
            c,d=min(p[2] for p in f.points),max(p[2] for p in f.points)
            if b<=x0 or a>=x1 or d<=z0 or c>=z1:
                clipped.append(f)
                continue
            l,r,t,u=max(a,x0),min(b,x1),max(c,z0),min(d,z1)
            for left,right,top,bottom in [(a,l,c,d),(r,b,c,d),(l,r,c,t),(l,r,u,d)]:
                if left<right and top<bottom:
                    clipped.append(Face(((left,0,top),(left,0,bottom),(right,0,bottom),(right,0,top)),f.color,f.surface,f.force,checker_color=f.checker_color))
            if color is not None:
                tint=1 if (math.floor(l/1000)+math.floor(t/1000))%2 else .92
                rgb=tuple(round(v*tint) for v in COLORS[color])
                clipped.append(Face(((l,0,t),(l,0,u),(r,0,u),(r,0,t)),rgb,surface,force,checker_color=COLORS[color]))
        m.faces=clipped


def add_banked_turn(m,cx,start,end,climb,segments):
    surf="SURFACE_VERY_SLIPPERY"
    for i in range(segments):
        a=start+(end-start)*i/segments
        b=start+(end-start)*(i+1)/segments
        ya=400+1000*i/segments if climb else 400
        yb=400+1000*(i+1)/segments if climb else 400
        # Bank tapers to zero at the straight joins, keeping the seam level.
        ba=280*math.sin(math.pi*i/segments)
        bb=280*math.sin(math.pi*(i+1)/segments)
        top=[(cx+1350*math.sin(a),ya,-10000+1350*math.cos(a)),
             (cx+1350*math.sin(b),yb,-10000+1350*math.cos(b)),
             (cx+2250*math.sin(b),yb+bb,-10000+2250*math.cos(b)),
             (cx+2250*math.sin(a),ya+ba,-10000+2250*math.cos(a))]
        if normal(top)[1]<0:
            top.reverse()
        m.face(top,COLORS["very_slippery"],surf)
        # Only exposed inner/outer sides; no cross-track walls at joins.
        for p,q in [(top[0],top[1]),(top[2],top[3])]:
            m.face([q,p,(p[0],0,p[2]),(q[0],0,q[2])],COLORS["very_slippery"])


def add_cannon_recess(m):
    # Distinct flat tones reveal the pit's depth without changing its ramp out.
    m.floor(-6600,-6200,-300,100,-350,"wall")
    m.face([(-6600,-350,-300),(-6600,0,-300),(-6600,0,100),(-6600,-350,100)],(166,183,190))
    m.face([(-6200,-350,100),(-6200,0,100),(-6200,0,-300),(-6200,-350,-300)],(80,95,108))
    m.face([(-6600,-350,100),(-6600,0,100),(-6200,0,100),(-6200,-350,100)],(110,127,140))
    m.ramp(-6600,-6200,-300,100,0,-350,"grip","SURFACE_NOT_SLIPPERY",base=-350)


def add_slide_circuit(m):
    # Clockwise capsule: downhill south straight, banked turns, jump on the
    # north straight, then a climbing turn back to the starting platform.
    surf="SURFACE_VERY_SLIPPERY"
    m.ramp(-9500,-4000,-8650,-7750,1400,400,"very_slippery",surf,axis="x")
    for cx,start,end,climb in [(-4000,0,math.pi,False),(-9500,math.pi,math.tau,True)]:
        first=len(m.faces)
        add_banked_turn(m,cx,start,end,climb,16)
        distant=Mesh()
        add_banked_turn(distant,cx,start,end,climb,8)
        m.register_lod(f"banked_turn_{cx}",first,distant)
    for x0,x1,y0,y1 in [(-5300,-4000,700,400),(-5800,-5300,800,700),
                         (-6800,-6200,400,600),(-9500,-6800,400,400)]:
        m.ramp(x0,x1,-12250,-11350,y0,y1,"very_slippery",surf,axis="x")
    # A 400-unit gap follows the crest; the ordinary floor below catches falls.
    m.ramp(-9900,-9100,-7750,-5150,1400,0,"grip","SURFACE_NOT_SLIPPERY")


def add_mixed_surfaces(m):
    x0,x1=-12600,-11600
    m.ramp(x0,x1,-5000,-4500,0,80,"normal")
    m.box(x0,x1,-4500,-3300,0,80,"normal")
    m.box(x0,x1,-3300,-1000,0,80,"ice","SURFACE_ICE")
    m.ramp(x0,x1,-1000,1000,80,800,"grip","SURFACE_NOT_SLIPPERY")
    m.ramp(x0,x1,1000,2500,800,800,"sand","SURFACE_SHALLOW_MOVING_QUICKSAND",force=0x0100)
    m.ramp(x0,x1,2500,4300,800,0,"normal")


def add_mover_landings(m):
    # Shuttle end docks and walking approaches, with open ground underneath.
    for x0,x1 in [(8000,8500),(11100,11600)]:
        m.box(x0,x1,-5250,-4350,0,250,"platform")
        m.ramp(x0,x1,-4350,-3450,250,0,"grip","SURFACE_NOT_SLIPPERY")
    # Disk sits east of the shuttle dock. Leave a short boarding gap outside
    # the full rotation envelope, including its polygon's corners.
    m.ramp(12450,12950,-4100,-3200,300,0,"grip","SURFACE_NOT_SLIPPERY")
    # Rocking bridge docks: a small end gap allows the bridge to swing freely.
    for x0,x1 in [(8450,9050),(11350,11950)]:
        m.box(x0,x1,-2700,-2100,0,350,"platform")
        m.ramp(x0,x1,-2100,-1000,350,0,"grip","SURFACE_NOT_SLIPPERY")
    # Lift roof: 130 units of headroom at its high point, open on all sides.
    m.box(8600,9800,-600,600,950,1120,"wall",bottom=True)
    for x in (8600,9680):
        for z in (-600,480):
            m.box(x,x+120,z,z+120,0,950,"wall")


def add_water_annex(m):
    # Flat terraces at water depths 20, 60, 100, 200 and 400 units.
    profile=[(1000,0),(1100,-60),(1500,-60),(1600,-100),(1900,-100),
             (2000,-140),(2300,-140),(2400,-240),(2700,-240),
             (2800,-440),(3100,-440),(4100,-2000),(6000,-2000)]
    for (z0,y0),(z1,y1) in zip(profile,profile[1:]):
        for a,b in [(6400,6800),(6800,7400),(7400,9000),(9000,9600),(9600,10800)]:
            current=z0>=4100 and a in (6800,9000)
            m.face([(a,y0,z0),(a,y1,z1),(b,y1,z1),(b,y0,z0)],COLORS["pool"],
                   "SURFACE_FLOWING_WATER" if current else DEFAULT,
                   (0x0100 if a==6800 else 0x0180) if current else 0)
        # Continuous gentle exit lane along the east bank for Metal Mario.
        e0=-.4*(z0-1000)
        e1=-.4*(z1-1000)
        m.face([(10800,e0,z0),(10800,e1,z1),(11600,e1,z1),(11600,e0,z0)],
               COLORS["grip"],"SURFACE_NOT_SLIPPERY")
        # Double-sided seam joins the different floor profiles without a gap.
        seam=[(10800,y0,z0),(10800,y1,z1),(10800,e1,z1),(10800,e0,z0)]
        # The profiles cross between terraces and deep water: split any crossing
        # so neither quad is a self-intersecting bow tie.
        if (y0-e0)*(y1-e1)<0:
            t=(y0-e0)/((y0-e0)-(y1-e1))
            z=z0+t*(z1-z0)
            y=y0+t*(y1-y0)
            pieces=[[(10800,y0,z0),(10800,y,z),(10800,e0,z0)],
                    [(10800,y,z),(10800,y1,z1),(10800,e1,z1)]]
        else:
            pieces=[seam]
        for piece in pieces:
            m.face(piece,COLORS["pool"])
            m.face(piece[::-1],COLORS["pool"])
        m.face([(11600,e1,z1),(11600,0,z1),(11600,0,z0),(11600,e0,z0)],COLORS["pool"])
    m.face([(6400,-2000,6000),(6400,0,6000),(11600,0,6000),(11600,-2000,6000)],COLORS["pool"])
    m.face([(6400,-40,1000),(6400,-40,6000),(11600,-40,6000),(11600,-40,1000)],
           COLORS["water"],None,layer="water")
    # Solid 240-unit banks replace the coincident opposing wall sheets.
    # Keep the original west edge and the full 3100..4100 swimming opening.
    for z0,z1 in [(1000,3100),(4100,6000)]:
        m.box(6400,6640,z0,z1,-2000,0,"pool")
    # Close the step between the two pool floors inside the connecting opening.
    for (z0,a0,b0),(z1,a1,b1) in zip(
            [(3100,-400-800*500/1400,-440),(4000,-1200,-1844)],
            [(4000,-1200,-1844),(4100,-1275,-2000)]):
        seam=[(6400,a0,z0),(6400,a1,z1),(6400,b1,z1),(6400,b0,z0)]
        if (a0-b0)*(a1-b1)<0:
            t=(a0-b0)/((a0-b0)-(a1-b1))
            z=z0+t*(z1-z0)
            y=a0+t*(a1-a0)
            pieces=[[(6400,a0,z0),(6400,y,z),(6400,b0,z0)],
                    [(6400,y,z),(6400,a1,z1),(6400,b1,z1)]]
        else:
            pieces=[seam]
        for piece in pieces:
            m.face(piece,COLORS["pool"])
            m.face(piece[::-1],COLORS["pool"])
    # Staggered walls make alternating tight bends, with a sloping ceiling in
    # between; all have open escape routes above and at their ends.
    m.box(7650,7800,4600,5800,-2000,-1000,"pool")
    m.box(9100,9250,4200,5400,-2000,-1000,"pool")
    m.slab(7900,8950,4400,5300,-1550,-1050)
    # Open-top shaft, entered through a 600 x 700 lower opening on the west.
    m.box(9950,10100,4700,4900,-2000,-250,"pool")
    m.box(9950,10100,5500,5800,-2000,-250,"pool")
    m.box(9950,10100,4900,5500,-1300,-250,"pool",bottom=True)
    m.box(10700,10800,4700,5800,-2000,-250,"pool")
    m.box(10100,10700,4700,4800,-2000,-250,"pool")
    m.box(10100,10700,5700,5800,-2000,-250,"pool")


def add_precision_section(m):
    # One central court, approached from the same north promenade as the pools.
    m.ramp(-1600,1900,7800,8600,0,250,"grip","SURFACE_NOT_SLIPPERY")
    m.box(-1600,1900,8600,9000,0,250,"normal")
    for x,width in [(-1200,80),(-400,120),(500,180),(1400,260)]:
        m.box(x-width/2,x+width/2,9000,11000,0,250,"normal")
        m.box(x-220,x+220,11000,11400,0,250,"normal")
    for i,height in enumerate([100,125,150,175,200]):
        x=-1900+i*850
        m.box(x,x+700,11700,13000,height,height+150,"wall",bottom=True)
        m.box(x,x+100,11700,13000,0,height,"wall")
    for i,gap in enumerate([100,140,180,240,320]):
        x=-1900+i*850
        m.box(x,x+100,13400,14700,0,700,"wall")
        m.box(x+100+gap,x+200+gap,13400,14700,0,700,"wall")
    x=6100
    for width in [400,300,220,160,100]:
        m.box(x,x+width,7300,7300+width,0,250,"normal")
        x+=width+200
    m.box(1900,2900,9200,10500,450,650,"wall",bottom=True)
    m.box(1900,2050,9200,10500,0,450,"wall")


def add_uneven_terrain(m, distant=False):
    # Longitudinal triangular ridges, rounded humps and an oblique ridge. The
    # patch tapers to floor level on every edge, giving several approach angles.
    first=len(m.faces)
    xs=list(range(-6500,-1699,400))
    zs=list(range(6500,8101,400 if distant else 200))
    def height(x,z):
        edge=min(1,(x+6500)/400,(-1700-x)/400,(z-6500)/200,(8100-z)/200)
        ridge=120*(1-abs(((x+6500)%800)/400-1))
        hump=100*(1+math.sin((x+z)*math.pi/1200))/2
        diagonal=90*(1-abs(((x-z)%1200)/600-1))
        return max(0,edge)*(ridge+hump+diagonal)
    for a,b in zip(xs,xs[1:]):
        for c,d in zip(zs,zs[1:]):
            # Nonplanar cells are explicitly triangulated to match collision.
            p=[(a,height(a,c),c),(a,height(a,d),d),(b,height(b,d),d),(b,height(b,c),c)]
            for tri in [(p[0],p[1],p[2]),(p[0],p[2],p[3])]:
                m.face(tri,COLORS["normal"])
    if distant:
        return
    coarse=Mesh()
    add_uneven_terrain(coarse,True)
    m.register_lod("uneven_hills",first,coarse)
    for i,h in enumerate([30,60,90,120,150,120,90,60,30]):
        m.box(-7800,-6900,6400+i*160,6560+i*160,0,h,"normal")


def add_flight_course(m):
    # Arches north of the tower, with clear air between launch and tunnel.
    for x,z,y in [(1600,-6100,2200),(2500,-8200,1900),(3500,-9800,1650)]:
        m.box(x-850,x-650,z-150,z+150,0,y+950,"tower")
        m.box(x+650,x+850,z-150,z+150,0,y+950,"tower")
        m.box(x-650,x+650,z-150,z+150,y+750,y+950,"tower",bottom=True)
        m.box(x-650,x+650,z-150,z+150,y-150,y,"tower",bottom=True)
    # Fly east through a descending tunnel, 900 units tall and 1200 wide.
    m.ramp(4500,7500,-10500,-9100,1500,900,"tower",axis="x")
    m.slab(4500,7500,-10500,-9100,2400,1800,color="tower",axis="x")
    for z0,z1 in [(-10650,-10500),(-9100,-8950)]:
        m.ramp(4500,7500,z0,z1,2550,1950,"tower",axis="x")
    m.box(7900,10000,-10500,-8950,0,700,"tower")
    m.ramp(10000,12200,-10500,-8950,700,0,"grip","SURFACE_NOT_SLIPPERY",axis="x")
    m.box(6500,7400,-8000,-7100,0,450,"tower")


SCULPTURES = [
    (-11100,7900,DEFAULT,COLORS["normal"]),
    (-7600,7900,"SURFACE_SLIPPERY",COLORS["slippery"]),
    (-4100,7900,"SURFACE_VERY_SLIPPERY",COLORS["very_slippery"]),
    (-11100,12000,"SURFACE_NOT_SLIPPERY",COLORS["grip"]),
]
# Each material has its large, half and quarter specimens together. Preserve
# the requested 180-degree turn of both smaller copies.
SMALL_SCULPTURES = [(i,x-600,z+2100,.5) for i,(x,z,*_) in enumerate(SCULPTURES)] + [
    (i,x+650,z+2100,.25) for i,(x,z,*_) in enumerate(SCULPTURES)]


def build_sculpture(distant=False, parts=None):
    """Closed, deliberately hostile shapes: good triangles, bad clearances.

    Each fin is triangulated between irregular rings. The combined shape has
    inward-leaning faces, intersecting fins, a narrowing cleft and a low beak.
    """
    m=Mesh()
    parts=[] if parts is None else parts
    def fin(rings):
        first=len(m.faces)
        if distant:
            rings=[rings[0],rings[-1]]
        # Ring vertices run clockwise as viewed from above (positive Y normal).
        m.face(rings[0][::-1],COLORS["wall"])
        m.face(rings[-1],COLORS["wall"])
        for lower,upper in zip(rings,rings[1:]):
            for i in range(4):
                j=(i+1)%4
                m.face([lower[j],upper[j],upper[i]],COLORS["wall"])
                m.face([lower[j],upper[i],lower[i]],COLORS["wall"])
        parts.append((first,len(m.faces),None))

    m.box(-930,930,-950,950,0,40,"wall")
    parts.append((0,len(m.faces),0))
    left=[
        [(-820,40,-650),(-740,40,750),(-280,40,680),(-40,40,-650)],
        [(-780,650,-600),(-560,500,700),(-70,520,650),(20,720,-570)],
        [(-520,1640,-430),(-630,1380,450),(-250,1550,350),(-180,1720,-350)],
    ]
    fin(left)
    # Mirroring reverses winding. Unequal heights and a rearward twist prevent
    # the cleft from being a simple pair of parallel vertical walls.
    right=[[( -x,y+(100 if y>1000 else 0),z-50) for x,y,z in ring][::-1] for ring in left]
    fin(right)
    fin([
        [(-450,40,-900),(-440,40,-600),(430,40,-520),(500,40,-890)],
        [(-290,1050,-830),(-350,950,-560),(310,1100,-420),(430,1200,-750)],
        [(-190,1870,-720),(-120,1950,-500),(130,1760,-520),(260,1860,-650)],
    ])
    # Sloped floor feeds the narrowing rear cleft. Its low roof makes sideways
    # approaches, crouching, landing and standing up encounter different faces.
    if not distant:
        first=len(m.faces)
        m.ramp(-280,280,-700,780,460,80,"wall",base=40)
        parts.append((first,len(m.faces),40))
        first=len(m.faces)
        m.ramp(-280,280,780,950,80,40,"wall",base=40)
        parts.append((first,len(m.faces),40))
        first=len(m.faces)
        m.slab(-220,330,-100,480,450,340,150,"wall")
        parts.append((first,len(m.faces),None))
    # A sideways spike and shallow undercut on the outer edge.
    fin([
        [(-910,40,-300),(-900,40,180),(-730,40,270),(-680,40,-280)],
        [(-910,370,-350),(-870,280,150),(-380,730,100),(-420,900,-310)],
    ])
    return m


def sculpture_render_triangles(distant=False):
    """Simplify only static visuals; the collision template stays exact.

    A piece's inward half-spaces give a conservative solid core, even for
    concave fins. Remove a triangle only when all its vertices lie in another
    core. On a shared plane, its outward side must enter the other piece;
    this preserves exposed coincident faces. All tests use integer arithmetic.
    """
    parts=[]
    mesh=build_sculpture(distant,parts)
    triangles=list(mesh.triangles())
    planes=[]
    owners=[]
    for owner,(first,end,base) in enumerate(parts):
        bounds=[]
        for face in mesh.faces[first:end]:
            for i in range(1,len(face.points)-1):
                tri=(face.points[0],face.points[i],face.points[i+1])
                n=normal(tri)
                bounds.append((n,-sum(a*b for a,b in zip(n,tri[0]))))
                owners.append(owner)
        if base is not None:
            bounds.append(((0,-1,0),base)) # Close ground-supported bases.
        planes.append(bounds)
    assert len(owners)==len(triangles)
    # One plain plinth top instead of nine alternating-color rectangles.
    result=[(0,((-930,40,-950),(-930,40,950),(930,40,950))),
            (0,((-930,40,-950),(930,40,950),(930,40,-950)))]
    for index,(_,tri) in enumerate(triangles):
        n=normal(tri)
        if owners[index]==0 and n[1]>0:
            continue
        buried=False
        for owner,bounds in enumerate(planes):
            if owner==owners[index]:
                continue
            for plane,offset in bounds:
                distances=[sum(a*b for a,b in zip(plane,p))+offset for p in tri]
                if max(distances)>0 or (min(distances)==max(distances)==0
                        and sum(a*b for a,b in zip(plane,n))>=0):
                    break
            else:
                buried=True
                break
        if not buried:
            result.append((index,tri))
    return result


def add_sculptures(m):
    template=build_sculpture()
    for index,(cx,cz,surface,rgb) in enumerate(SCULPTURES):
        for i,(_,tri) in enumerate(template.triangles()):
            points=[(x+cx,y,z+cz) for x,y,z in tri]
            # Alternating facet shades emphasize the awkward triangle layout.
            shade=(.84,.95,1.0)[i%3]
            color=tuple(round(c*shade) for c in rgb)
            # Keep the exact collision and a full-resolution color reference.
            # The render exporter later removes covered faces and plinth tiles.
            m.face(points,color,surface,layer="collision_only")
            m.face(points,color,None,layer=f"sculpture_{index}")
    for index,cx,cz,scale in SMALL_SCULPTURES:
        surface,rgb=SCULPTURES[index][2:]
        for _,tri in template.triangles():
            # Reuse the scaled full-size display list; these faces supply only
            # the exactly corresponding static collision at each smaller site.
            m.face([(-x*scale+cx,y*scale,-z*scale+cz) for x,y,z in tri],rgb,surface,layer="collision_only")


def add_shallow_pool(m):
    # Raised basins sit above the annex floor. Their water boxes and visible
    # surfaces share these exact bounds; every pool has a walking exit.
    # Shallow water: 20, 60 and 100 deep, with smooth joins between terraces.
    profile=[(7200,200),(7500,160),(7800,160),(8000,120),
             (8300,120),(8500,80),(8800,80),(9100,200)]
    for (z0,y0),(z1,y1) in zip(profile,profile[1:]):
        m.ramp(6800,9200,z0,z1,y0,y1,"pool")
    for x0,x1 in [(6650,6800),(9200,9350)]:
        m.box(x0,x1,7200,9100,0,240,"pool")
    m.ramp(6800,9200,6650,7200,0,200,"normal")
    m.ramp(6800,9200,9100,9650,200,0,"normal")
    m.face([(6800,180,7200),(6800,180,9100),(9200,180,9100),(9200,180,7200)],
           COLORS["water"],None,layer="water")



def add_cold_pool(m):
    # Cold pool: 480 deep, ice rim, gently sloping access at the north end.
    m.box(9900,12300,7600,10000,0,120,"pool")
    m.ramp(9900,12300,7100,7600,660,120,"pool",base=0)
    for x0,x1 in [(9750,9900),(12300,12450)]:
        m.box(x0,x1,7100,10150,0,660,"ice","SURFACE_ICE")
    m.box(9900,12300,10000,10150,0,660,"ice","SURFACE_ICE")
    m.ramp(9900,12300,6300,7100,0,660,"grip","SURFACE_NOT_SLIPPERY")
    m.face([(9900,600,7100),(9900,600,10000),(12300,600,10000),(12300,600,7100)],
           (167,228,245),None,layer="water")



def add_poison(m):
    # Poison gas is the native HMC gas mechanic, not a damaging floor. The
    # green volume's top is the real gas level. Raised refuges clear the gas.
    m.box(7000,9500,10100,12000,0,20,(124,146,69))
    m.face([(7000,550,10100),(7000,550,12000),(9500,550,12000),(9500,550,10100)],
           (151,191,70),None,layer="water")
    for a,b in [((7000,10100),(9500,10100)),((9500,10100),(9500,12000)),
                ((9500,12000),(7000,12000)),((7000,12000),(7000,10100))]:
        m.face([(a[0],20,a[1]),(a[0],550,a[1]),(b[0],550,b[1]),(b[0],20,b[1])],
               (151,191,70),None,layer="water")
    m.box(7300,7800,11100,11600,20,600,"grip","SURFACE_NOT_SLIPPERY")
    m.ramp(8700,9300,10600,11700,20,600,"grip","SURFACE_NOT_SLIPPERY")


def add_inertia_sections(m):
    # Three matched 22,000-unit strips: identical downhill starts and flat
    # runouts make normal, ice and high-grip coasting directly comparable.
    for z,surface,color in [(-14700,DEFAULT,"normal"),(-13950,"SURFACE_ICE","ice"),
                            (-13200,"SURFACE_NOT_SLIPPERY","grip")]:
        m.ramp(-11900,-8200,z-300,z+300,1200,200,color,surface,axis="x")
        m.box(-8200,5200,z-300,z+300,0,200,color,surface)
        # A final opposing slope measures momentum retained up a hill.
        m.ramp(5200,10500,z-300,z+300,200,1200,color,surface,axis="x")
        m.box(10500,11900,z-300,z+300,0,1200,color,surface)
    # Connected start deck and ground access clear of the enemy-court wall.
    m.box(-12500,-11900,-15000,-12900,0,1200,"grip","SURFACE_NOT_SLIPPERY")
    m.ramp(-13300,-12500,-12100,-11100,1200,0,"grip","SURFACE_NOT_SLIPPERY",axis="x")
    m.box(-13300,-12500,-15000,-12100,0,1200,"grip","SURFACE_NOT_SLIPPERY")
    m.ramp(500,1200,-12900,-12000,200,0,"grip","SURFACE_NOT_SLIPPERY")
    m.box(500,1200,-15000,-12900,0,180,"normal")

    # Broad repeated valleys and crests, then an ice-to-grip braking strip.
    profile=[(-11900,200),(-10500,1100),(-8800,200),(-7100,900),
             (-5400,200),(-3700,700),(-2000,200)]
    for (z0,y0),(z1,y1) in zip(profile,profile[1:]):
        m.ramp(-15000,-13500,z0,z1,y0,y1,"very_slippery","SURFACE_VERY_SLIPPERY")
    for z0,z1,surface,color in [(-2000,500,"SURFACE_ICE","ice"),
                               (500,1700,DEFAULT,"normal"),
                               (1700,2900,"SURFACE_NOT_SLIPPERY","grip")]:
        m.box(-15000,-13500,z0,z1,0,200,color,surface)
    # Return to the west walking route before reaching the mixed-surface
    # descent. The former bridge crossed straight through that test slope.
    m.box(-15000,-13500,2900,3900,0,200,"normal")
    m.ramp(-13500,-12800,2900,3900,200,0,"grip","SURFACE_NOT_SLIPPERY",axis="x")
    # Low oblique walls at the end let slides graze a wall and carry tangential
    # speed. The open outside edge is a safe flat recovery strip.
    m.wall((-14900,2500),(-14100,3400),400,70,"wall",base=200)


def add_aquatic_habitat(m):
    # A basin beside the other pools. Its footprint and depth preserve the
    # shark orbit and eel route; no unrelated courses cover the swimming space.
    x0,x1,z0,z1=7300,12100,9900,14600
    m.floor(x0,x1,z0,z1,-2200,"pool")
    ring=[(x0,z0),(x1,z0),(x1,z1),(x0,z1)]
    for a,b in zip(ring,ring[1:]+ring[:1]):
        # Face the swimming space, matching both backface culling and collision.
        m.face([(a[0],0,a[1]),(a[0],-2200,a[1]),(b[0],-2200,b[1]),(b[0],0,b[1])],COLORS["pool"])
    m.ramp(11500,12100,10900,14600,-2200,0,"grip","SURFACE_NOT_SLIPPERY",base=-2200)
    m.face([(x0,-40,z0),(x0,-40,z1),(x1,-40,z1),(x1,-40,z0)],COLORS["water"],None,layer="water")


def add_blj_activation(m):
    # A compact staircase catches backward long jumps early in their arc.
    # Stand on the orange pad, face south (away from the stairs), and travel
    # backward north up the steps. No speed or action rules are changed here.
    for i in range(20):
        near=1000-i*128
        m.box(13800,15000,near-128,near,0,(i+1)*64,"normal")
    m.box(13800,15000,-14900,-1560,0,1280,"normal")
    m.box(13800,15000,-15000,-14900,0,1900,"wall")
    m.box(13800,15000,1000,2200,0,20,"tower")
    m.box(13800,15000,2200,10000,0,20,"runway")
    m.ramp(13800,15000,10000,10800,20,0,"normal")
    # Flush side access from the main promenade, outside the activation steps.
    m.ramp(13400,13800,2200,2800,0,20,"normal",axis="x")


def add_enemy_courts(m):
    # Ground enemies have a bay beside the speed courses, away from the runway.
    m.box(1300,1400,-12700,-10700,0,240,"wall")
    m.box(6400,6500,-12700,-10700,0,240,"wall")
    m.box(1300,6500,-12800,-12700,0,240,"wall")
    for a,b in [(1400,1600),(2600,6400)]:
        m.box(a,b,-10800,-10700,0,240,"wall")
    # Equal Goomba/Floomba cells leave the shared southern approach open.
    for x0,x1 in [(1400,1540),(6260,6400)]:
        m.box(x0,x1,-12700,-11550,0,350,"wall")
    for x in (2700,3900,5100):
        m.box(x-40,x+40,-12700,-11550,0,350,"wall")
    # A side barrier keeps the crusher row outside the east Tox Box sweep.
    m.box(10700,10800,-12700,-10850,0,600,"wall")
    # Both Bully variants use the same dry floor, walls and southern entrance.
    for x0,x1,z0,z1 in BULLY_COURTS:
        m.box(x0-100,x0,z0-100,z1+100,0,350,"wall")
        m.box(x1,x1+100,z0-100,z1+100,0,350,"wall")
        m.box(x0,x1,z0-100,z0,0,350,"wall")
        middle = (x0+x1)//2
        for left,right in [(x0,middle-300),(middle+300,x1)]:
            m.box(left,right,z1,z1+100,0,350,"wall")
    # A real ceiling for Swoop; haunted targets use the surrounding side court.
    m.box(-14700,-12700,9000,10300,1000,1150,"wall",bottom=True)
    m.box(-14700,-14550,9000,10300,0,1000,"wall")


def add_boulder_lane(m):
    # Use the empty southeast bay beyond the BLJ exit, clear of the promenade
    # and aquatic basin. Gentle ends reverse its rolling motion; the perimeter
    # also catches sideways travel. Only the flat restores the lab's speed.
    m.ramp(13700,14900,11400,12000,300,0,"normal")
    m.ramp(13700,14900,13700,14300,0,300,"normal")
    # Thick barriers keep opposite wall faces out of the rolling sphere's
    # next collision probe, even after an angled 70-unit movement step.
    m.box(13450,15150,11150,11400,0,750,"wall")
    m.box(13450,15150,14300,14550,0,750,"wall")
    m.box(14900,15150,11400,14300,0,750,"wall")
    # A flat 280-unit walk-in entrance fits Mario, but not the boulder's
    # 630-unit-wide native hitbox. No ramp gives it a route over the walls.
    for z0,z1 in [(11400,12710),(12990,14300)]:
        m.box(13450,13700,z0,z1,0,750,"wall")


def build_seam_strip(m):
    """One template supplies both four-lane specimens and their recovery pads."""
    for x0,x1 in [(0,75),(375,575),(875,1075),(1375,1575),(1875,1950)]:
        m.floor(x0,x1,0,2150,color="court",tile=2150)
    for lane in range(4):
        x0,x1=75+lane*500,375+lane*500
        m.floor(x0,x1,0,300,color="route",tile=2150)
        m.floor(x0,x1,1800,2150,color="route",tile=2150)
        if lane == 1:
            # A thin, irregular triangle fan changes topology alone, not grade.
            for z0 in range(300,1800,300):
                z1=z0+300
                center=(x0+(70 if z0%600 else 230),0,z0+155)
                ring=[(x0,0,z0),(x0,0,z1),(x1,0,z1),(x1,0,z0)]
                for a,b in zip(ring,ring[1:]+ring[:1]):
                    m.face([center,a,b],COLORS["normal"])
        elif lane == 2:
            m.floor(x0,x1,300,900,color="normal",tile=2150)
            m.box(x0,x1,900,1000,0,8,"normal")
            m.floor(x0,x1,1000,1800,color="normal",tile=2150)
        elif lane == 3:
            m.floor(x0,x1,300,1050,color="normal",tile=2150)
            m.floor(x0,x1,1050,1800,color="ice",surface="SURFACE_ICE",tile=2150)
        else:
            m.floor(x0,x1,300,1800,color="normal",tile=2150)


def add_impact_bed(m,x0,x1,depth,ramp_width,ramp_length):
    z0,z1=8250,9500
    rx,rz=x1-ramp_width,z1-ramp_length
    m.floor(x0,rx,z0,z1,-depth,"pool")
    if rz > z0:
        m.floor(rx,x1,z0,rz,-depth,"pool")
    m.ramp(rx,x1,rz,z1,-depth,0,"grip","SURFACE_NOT_SLIPPERY",base=-depth)
    # Inward-facing banks follow the exit ramp and leave its dry end open.
    m.face([(x0,0,z0),(x0,-depth,z0),(x1,-depth,z0),(x1,0,z0)],COLORS["pool"])
    m.face([(x0,0,z1),(rx,0,z1),(rx,-depth,z1),(x0,-depth,z1)],COLORS["pool"])
    m.face([(x0,0,z0),(x0,0,z1),(x0,-depth,z1),(x0,-depth,z0)],COLORS["pool"])
    if rz > z0:
        m.face([(x1,0,z0),(x1,-depth,z0),(x1,-depth,rz),(x1,0,rz)],COLORS["pool"])
    m.face([(x1,0,rz),(x1,-depth,rz),(x1,0,z1)],COLORS["pool"])
    m.face([(x0,0,z0),(x0,0,z1),(x1,0,z1),(x1,0,z0)],COLORS["water"],None,layer="water")


def add_impact_bay(m):
    # The dry bed keeps the existing ground at Y=0; water entries share Y=0.
    add_impact_bed(m,8900,10000,60,200,250)
    add_impact_bed(m,10300,11400,600,300,1200)
    for height in RELEASE_HEIGHTS:
        m.box(7400,11500,7800,8150,height-60,height,"wall",bottom=True)
        m.box(7000,7400,8100,8350,height-60,height,"wall",bottom=True)
    # Slender supports stay north of the release edge and outside every bed.
    for x in (7500,11400):
        m.box(x-40,x+40,7870,7950,0,RELEASE_HEIGHTS[-1]-60,"wall")


def add_projectile_bay(m):
    # Keep the comparison lanes apart without altering native projectile logic.
    m.box(-5000,-4900,11200,14600,0,350,"wall")
    for z0,z1 in [(11200,13100),(13700,14600)]:
        m.box(-3150,-3050,z0,z1,0,350,"wall")
    for x0,x1 in [(-4900,-4400),(-3800,-3150)]:
        m.box(x0,x1,11200,11300,0,350,"wall")
    m.box(-4900,-3150,14500,14600,0,350,"wall")
    m.box(-4010,-3930,11800,14400,0,350,"wall")
    # Neighboring explosion/shot/fire bays share a western side corridor.
    # The western side stays open; the eastern backstop separates the Bill lane.
    m.box(-5150,-5050,11200,14600,0,1500,"wall")
    for z0,z1 in [(11200,11300),(14500,14600)]:
        m.box(-5900,-5050,z0,z1,0,1500,"wall")
    for z in (12000,12800,13600):
        m.box(-5900,-5150,z-50,z+50,0,1500,"wall")
    m.box(-5900,-5150,14100,14180,0,1500,"wall")


def add_northern_terrace(m):
    # Slabs cross above the promenade; their closed undersides leave at least
    # 400 units over ground. The existing docks support the southern ends.
    m.box(-2800,600,-7800,-6900,700,800,"wall","SURFACE_NOT_SLIPPERY",bottom=True)
    m.slab(-2800,-2200,-6900,-6200,700,1192,100,"wall",surface="SURFACE_NOT_SLIPPERY")
    m.slab(-1900,-1400,-6900,-6000,700,450,100,"wall",surface="SURFACE_NOT_SLIPPERY")
    m.slab(600,1500,-7800,-6900,700,500,thickness=100,color="wall",axis="x",
           surface="SURFACE_NOT_SLIPPERY")
    m.box(1500,1900,-7800,-6900,500,600,"wall","SURFACE_NOT_SLIPPERY",bottom=True)
    m.slab(1500,1900,-6900,-5900,500,400,100,"wall",surface="SURFACE_NOT_SLIPPERY")

    # Every column is north of the east/west walk and outside the northbound
    # path at X=600..1200, the flight arch and both moving-platform sweeps.
    for x,underside in [(-2740,700),(-1790,700),(-110,700),(1830,500)]:
        m.box(x-40,x+40,-7380,-7300,0,underside,"wall")

    # A front-facing return overlooks the bowl and ends beside the commons.
    # The tilter approach remains usable below the thin upper landing.
    m.box(-2000,-1600,-5700,-3700,1212,1292,"wall","SURFACE_NOT_SLIPPERY",bottom=True)
    m.ramp(-2000,-1600,-3700,-1300,1292,0,"wall","SURFACE_NOT_SLIPPERY")
    m.box(-1980,-1900,-5580,-5500,0,1212,"wall")


def add_finishing_galleries(m):
    # The bridge and disk share a thin gallery at bridge-dock height. Its
    # ground return stops before the original eastern promenade.
    m.box(11050,12000,-3150,-1600,270,350,"wall","SURFACE_NOT_SLIPPERY",bottom=True)
    # The disk link runs beside its original ramp, then crosses above the low
    # end with over 200 units of clearance. Covering the ramp's high end would
    # trap a ground approach between its floor and the new slab's underside.
    m.slab(12050,12400,-4100,-3400,220,270,80,"wall",surface="SURFACE_NOT_SLIPPERY")
    m.box(11600,12400,-3400,-3150,270,350,"wall","SURFACE_NOT_SLIPPERY",bottom=True)
    m.ramp(12000,12500,-3000,-2200,350,0,"wall","SURFACE_NOT_SLIPPERY",axis="x")
    for x,z in [(11120,-3060),(11930,-3060),(11120,-1700),(11930,-1700)]:
        m.box(x-40,x+40,z-40,z+40,0,270,"wall")

    # A complete return loop joins the finish pads back to the start deck.
    # It stops before the low-roof row and outside the measured beam widths.
    m.box(-2000,1620,11400,11650,0,250,"wall","SURFACE_NOT_SLIPPERY")
    m.box(-2000,-1600,8600,11400,0,250,"wall","SURFACE_NOT_SLIPPERY")
    m.ramp(-2000,-1600,7800,8600,0,250,"wall","SURFACE_NOT_SLIPPERY")

    # A level side crossing links beam practice to the shallow-water rim.
    # The independent hanging-ceiling specimen begins 200 units farther south.
    m.ramp(1900,2000,8600,9000,250,240,"wall","SURFACE_NOT_SLIPPERY",axis="x")
    m.box(2000,3150,8600,9000,0,240,"wall","SURFACE_NOT_SLIPPERY")
    m.ramp(2300,2700,7800,8600,0,240,"wall","SURFACE_NOT_SLIPPERY")


def add_water_quays(m):
    # Dry side routes meet the existing pool rims. The ground promenade at
    # X=6200..6900 and supply crossing Z=10250..10600 retain their full width.
    m.ramp(5850,6200,7800,8500,0,240,"wall","SURFACE_NOT_SLIPPERY")
    m.box(5850,6200,8500,9900,0,240,"wall","SURFACE_NOT_SLIPPERY")
    m.ramp(5850,6200,9900,10250,240,0,"wall","SURFACE_NOT_SLIPPERY")
    m.ramp(5850,6200,10600,11400,0,660,"wall","SURFACE_NOT_SLIPPERY")
    m.box(5850,6200,11400,13900,0,660,"wall","SURFACE_NOT_SLIPPERY")
    m.ramp(5850,6200,13900,14700,660,0,"wall","SURFACE_NOT_SLIPPERY")


def add_quay_shoulders(m):
    # Chamfered inward shoulders occupy the dry gaps beyond the pool rims.
    # Their tops stay below the quay ramps; x >= 5850 remains untouched.
    for z0,z1,y0,y1,corner in [(9750,10100,160,0,70),
                             (10600,11000,0,240,60),
                             (14450,14700,160,0,50)]:
        footprint = [(5850,z0),(5770,z0),(5710,z0+corner),
                     (5710,z1-corner),(5770,z1),(5850,z1)]
        top = [(x,y0+(y1-y0)*(z-z0)/(z1-z0),z) for x,z in footprint]
        base = [(x,0,z) for x,z in footprint]
        for i in range(1,len(top)-1):
            m.face([top[0],top[i],top[i+1]],COLORS["wall"],"SURFACE_NOT_SLIPPERY")
            m.face([base[0],base[i+1],base[i]],COLORS["wall"])
        for a,b in zip(top,top[1:]+top[:1]):
            m.face([b,a,(a[0],0,a[2]),(b[0],0,b[2])],COLORS["wall"])


def paint_floor(m,bounds,height,color,solid=False):
    """Split the rendered floor while retaining its exact collision triangles."""
    x0,x1,z0,z1=bounds
    painted=[]
    for face in m.faces:
        rectangle=flat_rectangle(face) if face.layer == "opaque" else None
        if rectangle is None or rectangle[0:2] != (1,1) or rectangle[2][1] != height:
            painted.append(face)
            continue
        _,_,lo,hi=rectangle
        l,r,t,u=max(lo[0],x0),min(hi[0],x1),max(lo[2],z0),min(hi[2],z1)
        if l >= r or t >= u:
            painted.append(face)
            continue
        assert face.lod_group is None
        if face.surface is not None:
            painted.append(Face(face.points,face.color,face.surface,face.force,"collision_only"))
        parts=[(lo[0],l,lo[2],hi[2]),(r,hi[0],lo[2],hi[2]),
               (l,r,lo[2],t),(l,r,u,hi[2]),(l,r,t,u)]
        for index,(left,right,top,bottom) in enumerate(parts):
            if left < right and top < bottom:
                points=((left,height,top),(left,height,bottom),(right,height,bottom),(right,height,top))
                rgb = color if index == 4 else face.color
                checker = (color if index == 4 else face.checker_color) if face.checker_color else None
                # A missing colour removes only the rendered patch, leaving
                # its exact collision face above intact.
                if index == 4 and color is None:
                    continue
                if index == 4 and solid:
                    checker = None
                if index == 4 and checker is not None:
                    dark = (math.floor((left+right)/2000)+math.floor((top+bottom)/2000)) % 2 == 0
                    rgb = tuple(round(value*(.92 if dark else 1)) for value in color)
                painted.append(Face(points,rgb,None,checker_color=checker))
    m.faces=painted


def label_pixels(text,direction=None):
    rows=["0".join(LABEL_GLYPHS[letter][row] for letter in text) for row in range(5)]
    if direction is not None:
        rows=[pixels+"000"+DIRECTORY_ARROWS[direction][row] for row,pixels in enumerate(rows)]
    return rows


def paint_label(m,text,x,z,height=0,cell=40,direction=None):
    pixels=label_pixels(text,direction)
    footprint=(x,x+len(pixels[0])*cell,z,z+5*cell)
    covered=[]
    colors=set()
    for face in m.faces:
        rect=flat_rectangle(face) if face.layer=="opaque" else None
        if rect is None or rect[:2]!=(1,1) or rect[2][1]!=height:
            continue
        lo,hi=rect[2:]
        l,r,t,u=max(lo[0],footprint[0]),min(hi[0],footprint[1]),max(lo[2],footprint[2]),min(hi[2],footprint[3])
        if l<r and t<u:
            covered.append((l,r,t,u))
            colors.add((face.checker_color or face.color,face.checker_color is not None))
    if len(colors)==1 and rectangle_covered(footprint,covered):
        color,checker=colors.pop()
        m.label_textures.append((footprint,height,pixels,color,cell,checker))
    # Merge each horizontal stroke before cutting the floor, retaining its
    # original collision triangles and the world's repeating scale grid.
    for row in range(5):
        pixels = "0".join(LABEL_GLYPHS[letter][row] for letter in text)
        if direction is not None:
            pixels += "000"+DIRECTORY_ARROWS[direction][row]
        column = 0
        while column < len(pixels):
            if pixels[column] == "0":
                column += 1
                continue
            end = column + 1
            while end < len(pixels) and pixels[end] == "1":
                end += 1
            bounds = (x+column*cell,x+end*cell,z+row*cell,z+(row+1)*cell)
            paint_floor(m,bounds,height,COLORS["label"])
            column = end


def add_commons(m):
    # A broad stopping terrace faces the flat commons. It stays beside the
    # launch tower, south of its final ramp, and outside the measured runway.
    # Chamfered slopes leave every edge walkable without separate connectors.
    outer = [(3450,0,-2000),(3150,0,-1700),(3150,0,-950),(3450,0,-650),
             (6050,0,-650),(6350,0,-950),(6350,0,-1700),(6050,0,-2000)]
    inner = [(3550,120,-1750),(3350,120,-1550),(3350,120,-1100),(3550,120,-900),
             (5950,120,-900),(6150,120,-1100),(6150,120,-1550),(5950,120,-1750)]
    for i in range(1,len(inner)-1):
        m.face([inner[0],inner[i],inner[i+1]],COLORS["grip"],"SURFACE_NOT_SLIPPERY")
    for i in range(8):
        j = (i+1) % 8
        m.face([inner[i],outer[i],outer[j],inner[j]],
               COLORS["grip"],"SURFACE_NOT_SLIPPERY")

    # Keep the rear curb low; the larger directory now lies on the forecourt.
    m.box(3550,5950,-1740,-1640,120,240,"court")

    # District silhouettes echo their experiments without entering specimens:
    # an uphill shoulder, angled collision cheeks and two offset landing forms.
    m.ramp(-6550,-6350,-3200,-1650,500,0,"wall","SURFACE_NOT_SLIPPERY")
    m.wall((-6530,1050),(-6400,1275),480,100)
    m.wall((-2120,1275),(-1990,1050),480,100)
    m.ramp(7000,7100,-2150,-1400,300,0,"wall","SURFACE_NOT_SLIPPERY")
    m.box(8250,8550,-1800,-1500,480,560,"wall","SURFACE_NOT_SLIPPERY",bottom=True)
    m.box(8330,8410,-1775,-1695,0,480,"wall")


def paint_commons(m):
    # One wide forecourt joins the starting area to the terrace and water edge.
    # Its flat collision and the world-aligned measuring grid remain exact.
    for bounds in [(-750,6350,-650,1000),(-1400,2450,-1000,-650)]:
        paint_floor(m,bounds,0,COLORS["court"])
    paint_floor(m,(6900,8550,-1850,-1250),0,COLORS["court"])
    for bounds in [(-6500,-5700,1000,1400),(1050,2350,1000,1400),
                   (2050,2350,2500,6200),(2300,2700,6800,7800)]:
        paint_floor(m,bounds,0,COLORS["route"])


def paint_direction_panel(m,text,direction,bounds,cell):
    # Solid, flush placards replace their ground pixels instead of overlaying
    # them. The measuring grid around each placard keeps its original phase.
    x0,x1,z0,z1=bounds
    width=(4*len(text)+7)*cell
    assert width <= x1-x0 and 5*cell <= z1-z0
    paint_floor(m,bounds,0,COLORS["runway"],solid=True)
    paint_label(m,text,round((x0+x1-width)/2),round((z0+z1-5*cell)/2),
                cell=cell,direction=direction)


def paint_forecourt_directory(m):
    # One neutral backing groups four separate panels before the terrace.
    # Thirty-unit cells replace the old seventeen-unit curb lettering.
    paint_floor(m,(3150,6350,-650,500),0,COLORS["court"],solid=True)
    for text,direction,bounds in [
            ("MOTION","W",(3200,4600,-550,-200)),
            ("PLATFORMS","E",(4850,6250,-550,-200)),
            ("COLLISION","SW",(3200,4600,0,350)),
            ("WATER","SW",(4850,6250,0,350))]:
        paint_direction_panel(m,text,direction,bounds,30)


def paint_outer_directions(m):
    # These junctions lead along clear existing ground: west to the garden
    # gap and hill approach, east to the aquatic basin's northern shore.
    for text,direction,bounds in [
            ("GARDEN","W",(-7500,-6200,6200,6500)),
            ("HILLS","W",(-8000,-6700,5750,6000)),
            ("AQUATIC","E",(7300,8600,9500,9900))]:
        paint_direction_panel(m,text,direction,bounds,25)


def add_outer_entrances(m):
    # Matched Bully forecourts retain their original reference lines and
    # 600-unit openings. Low markers stay beyond the comparison cells.
    for z in (2400,5100):
        for z0,z1 in ((z-100,z-30),(z+30,z+300)):
            paint_floor(m,(-11500,-9950,z0,z1),0,COLORS["route"])
        for x in (-11470,-9990):
            m.box(x-60,x+60,z+170,z+290,0,120,"wall",bottom=True)

    # Shallow west entry, cold north entry, and aquatic south walking exit.
    # The posts leave the ramps, supply crossing and promenade unobstructed.
    for bounds,posts in [
            ((2200,2800,7500,7800),((2150,7650),(2850,7650))),
            ((3300,5700,10250,10600),((3070,10200),(5770,10200))),
            ((11300,12300,14600,14800),((11260,14680),(12340,14680)))]:
        paint_floor(m,bounds,0,COLORS["route"])
        for x,z in posts:
            m.box(x-60,x+60,z-60,z+60,0,120,"wall",bottom=True)


def add_combat_landmark(m):
    # One broad stepped silhouette marks both Bully courts from the western
    # link. It sits south of the path, beyond their matched entrance aprons.
    for i,height in enumerate((120,240,360,240,120)):
        x = -12100+i*300
        m.box(x,x+300,6100,6460,0,height,"wall",bottom=True)


def add_district_markings(m):
    # All four entrance names now face the commons; specimen measurements stay
    # beside their original test. Paint does not alter collision or friction.
    for text,x,z in [("MOTION",-6100,-1375),("PLATFORMS",6900,-900),
                     ("COLLISION",-6000,1100),("WATER",1050,1100)]:
        width = (4*len(text)-1)*40
        paint_floor(m,(x-50,x+width+50,z-50,z+250),0,COLORS["runway"])
        paint_label(m,text,x,z)

    for x,text in [(-5750,"NORMAL"),(-4650,"SLIP"),(-3550,"V-SLIP"),(-2450,"GRIP")]:
        width = (4*len(text)-1)*25
        paint_label(m,text,round(x-width/2),-4375,1292,25)
    for x,text in [(-1200,"80"),(-400,"120"),(500,"180"),(1400,"260")]:
        width = (4*len(text)-1)*25
        paint_label(m,text,round(x-width/2),8750,250,25)
    paint_label(m,"20 60 100",5890,10300,cell=25)
    paint_label(m,"480",5885,11450,660,25)


def add_movement_markings(m):
    for z in (-14700,-13950,-13200):
        paint_floor(m,(-12230,-12170,z-220,z+220),1200,COLORS["wall"])
        paint_floor(m,(-8200,-8160,z-300,z+300),200,COLORS["wall"])
        # 1000-unit divisions measure stopping distance from the ramp exit.
        for distance in range(1000,13400,1000):
            x=-8200+distance
            paint_floor(m,(x-10,x+10,z-300,z-190),200,COLORS["wall"])


def paint_practice_marks(m):
    # One bar marks the start; two mark the finish and ground/loop return.
    # Paint stays on flat staging floors, outside the measured slopes/beams.
    for a,b in [(-6200,-5300),(-5100,-4200),(-4000,-3100),(-2900,-2000)]:
        paint_floor(m,(a,b,-3860,-3830),1292,COLORS["label"])
    # A shared finish makes the identical slope endpoints easy to compare.
    for z in (-1485,-1445):
        paint_floor(m,(-6200,-2000,z,z+20),0,COLORS["label"])
    for x,width in [(-1200,80),(-400,120),(500,180),(1400,260)]:
        a,b=x-width/2,x+width/2
        paint_floor(m,(a,b,8920,8950),250,COLORS["label"])
        for z in (11120,11160):
            paint_floor(m,(a,b,z,z+20),250,COLORS["label"])


def build_terrain():
    m=Mesh()
    add_ground(m)

    # Four identical compound slopes: approximately 15, 30 and 45 degrees.
    # The nonslippery lane is also an easy route to the shared high landing.
    lanes=[(-6200,-5300,"normal",DEFAULT),(-5100,-4200,"slippery","SURFACE_SLIPPERY"),
           (-4000,-3100,"very_slippery","SURFACE_VERY_SLIPPERY"),(-2900,-2000,"grip","SURFACE_NOT_SLIPPERY")]
    profile=[(-1600,0),(-2300,188),(-3000,592),(-3700,1292)]
    for a,b,col,surf in lanes:
        for (z0,y0),(z1,y1) in zip(profile,profile[1:]):
            m.ramp(a,b,z1,z0,y1,y0,col,surf)
        # Flat strips at ground height use raised slabs to avoid overlapping floors.
        m.box(a,b,-5700,-3900,0,1292,col,surf)
        m.box(a,b,-3900,-3700,0,1292,col,surf)
    m.box(-6200,-2000,-6200,-5700,0,1292,"normal","SURFACE_NOT_SLIPPERY")

    # A faceted U-shaped bowl, with gentle roll-in lips at both ends.
    # Its trough meets Y=0: the underlying checker floor otherwise leaks through
    # the antialiased join. Keep that floor for collision and sealing tests.
    m.covered_ground.append((-1200,800,-4900,-1100))
    profile=[(-1100,0),(-1900,600),(-2200,330),(-2500,125),(-2800,30),
             (-3100,0),(-3400,30),(-3700,125),(-4000,330),(-4300,600),(-4900,0)]
    for i,((z0,y0),(z1,y1)) in enumerate(zip(profile,profile[1:])):
        outside=i in (0,len(profile)-2)
        m.ramp(-1200,800,z1,z0,y1,y0,"grip" if outside else "very_slippery",
               "SURFACE_NOT_SLIPPERY" if outside else "SURFACE_VERY_SLIPPERY",base=-20)

    # The six original ramp profiles and turn landings form an open tower.
    # Raised slabs and outer columns leave the ground and views clear below.
    for i in range(6):
        z=-5600+i*600
        lo,hi=i*500,(i+1)*500
        y0,y1=(lo,hi) if i%2==0 else (hi,lo)
        if i==0:
            m.ramp(3000,5600,z,z+400,y0,y1,"tower","SURFACE_NOT_SLIPPERY",axis="x")
        else:
            m.slab(3000,5600,z,z+400,y0-100,y1-100,100,
                   "tower",axis="x",surface="SURFACE_NOT_SLIPPERY")
        if i<5:
            a,b=(5600,6100) if i%2==0 else (2500,3000)
            m.box(a,b,z,z+1000,hi-100,hi,"tower","SURFACE_NOT_SLIPPERY",bottom=True)
            x=b-120 if i%2==0 else a
            for end in (z,z+880):
                m.box(x,x+120,end,end+120,0,hi-100,"tower")
    m.box(1200,3000,-2600,-1600,2840,3000,"tower","SURFACE_NOT_SLIPPERY",bottom=True)
    for x in (1200,2860):
        for z in (-2600,-1740):
            m.box(x,x+140,z,z+140,0,2840,"tower")
    # Lower landing shelves are outside the walking route.
    m.box(6500,6900,-5200,-4400,0,600,"tower")
    m.box(6500,6900,-3600,-2800,0,1400,"tower")

    # Wall-kick alleys, thin wall, corners and a sloping wall approach.
    for x in [-6300,-5830,-5160,-4290]:
        m.wall((x,1400),(x,3200),1100,120)
    m.wall((-3750,1400),(-2700,1400),900,32)
    m.wall((-3750,1400),(-3750,3200),900,120)
    m.wall((-2700,1400),(-1900,2200),900,120)
    m.ramp(-3400,-2900,2600,3200,0,300,"normal")
    # Low ceiling (140 clearance), overhang, and a separate hangable ceiling.
    m.box(-1800,-800,1500,2800,140,300,"wall",bottom=True)
    m.box(-1800,-1640,1500,2800,0,140,"wall")
    m.box(-400,1000,1400,2300,330,450,"platform",bottom=True,ceiling="SURFACE_HANGABLE")
    m.box(-400,-240,1400,2300,0,330,"platform")

    # Increasing steps and a line of elevated jump gaps, all above a safe floor.
    for i,h in enumerate([80,160,240,360,520]):
        m.box(1000,2000,2800+i*480,3280+i*480,0,h,"normal")
    x=-600
    for width,gap in [(500,200),(500,350),(400,500),(350,0)]:
        m.box(x,x+width,7000,7500,0,400,"tower")
        x+=width+gap
    m.ramp(-1600,-600,7000,7500,0,400,"grip","SURFACE_NOT_SLIPPERY",axis="x")

    # Shoreline: wading at 40 units, then progressively deeper slopes.
    profile=[(1000,0),(1800,-80),(2600,-400),(4000,-1200),(4800,-1800),(6000,-1800)]
    for (z0,y0),(z1,y1) in zip(profile,profile[1:]):
        # Subdivide crosswise to keep a separate current lane at the east edge.
        for a,b in [(2400,5200),(5200,6400)]:
            surface="SURFACE_FLOWING_WATER" if a==5200 and z0>=2600 else DEFAULT
            force=0x0100 if surface=="SURFACE_FLOWING_WATER" else 0
            m.face([(a,y0,z0),(a,y1,z1),(b,y1,z1),(b,y0,z0)],COLORS["pool"],surface,force)
        # Vertical banks face into the water, following the bottom profile.
        m.face([(2400,y0,z0),(2400,0,z0),(2400,0,z1),(2400,y1,z1)],COLORS["pool"])
    m.face([(2400,-1800,6000),(2400,0,6000),(6400,0,6000),(6400,-1800,6000)],COLORS["pool"])
    # Low submerged tunnel, 48-unit wall, and steps for Metal Mario.
    m.box(2700,3900,5050,5750,-1420,-1250,"pool",bottom=True)
    m.box(2700,2860,5050,5750,-1800,-1420,"pool")
    m.box(3740,3900,5050,5750,-1800,-1420,"pool")
    m.box(4250,4298,4900,5700,-1800,-850,"wall")
    for i in range(4):
        m.box(4550,5000,5500-i*240,5740-i*240,-1800,-1650+i*150,"tower")
    # Water is visual only; the matching water box controls its physics.
    m.face([(2400,-40,1000),(2400,-40,6000),(6400,-40,6000),(6400,-40,1000)],
           COLORS["water"],None,layer="water")

    # Vanish barriers: both a grounded passage and a jumping passage.
    m.wall((-1000,-900),(-1000,500),650,80,"vanish","SURFACE_VANISH_CAP_WALLS")
    m.box(800,1200,-900,-300,0,260,"wall")
    m.box(1500,1900,-900,-300,0,260,"wall")
    m.wall((1350,-900),(1350,-300),650,60,"vanish","SURFACE_VANISH_CAP_WALLS",base=120)

    # A proper cannon recess, with a ramp out if Mario lands off its hitbox.
    add_cannon_recess(m)
    add_slide_circuit(m)
    add_mixed_surfaces(m)
    # Elevated boarding ramps keep the original tilting/flipping fixtures
    # reachable while their entire moving meshes stay above the ground.
    m.box(-1800,-1300,-6000,-5700,0,550,"platform")
    m.ramp(-1800,-1300,-5700,-4950,550,0,"grip","SURFACE_NOT_SLIPPERY")
    m.box(1500,1900,-5900,-5500,0,500,"platform")
    m.ramp(1500,1900,-5500,-4800,500,0,"grip","SURFACE_NOT_SLIPPERY")
    place(m,add_mover_landings,dx=-900)
    add_water_annex(m)
    add_precision_section(m)
    place(m,add_uneven_terrain,dx=-16000,dz=-6000,quarter_turn=True)
    add_flight_course(m)
    add_sculptures(m)
    place(m,add_shallow_pool,dx=-3500,dz=600)
    place(m,add_cold_pool,dx=-6600,dz=4300)
    place(m,add_poison,dx=-16900,dz=-6700)
    add_inertia_sections(m)
    add_aquatic_habitat(m)
    add_blj_activation(m)
    # These 20-unit wedges need a clear render footprint: the underlying
    # checker otherwise wins depth precision near their ground-level toes.
    paint_floor(m,(13400,13800,2200,2800),0,None)
    paint_floor(m,(13800,15000,10000,10800),0,None)
    add_enemy_courts(m)
    add_boulder_lane(m)
    for x,z in SEAM_SITES:
        place(m,build_seam_strip,x,z)
    add_impact_bay(m)
    add_projectile_bay(m)
    add_movement_markings(m)
    add_northern_terrace(m)
    add_finishing_galleries(m)
    add_water_quays(m)
    add_quay_shoulders(m)
    add_commons(m)
    paint_commons(m)
    add_district_markings(m)
    paint_practice_marks(m)
    paint_forecourt_directory(m)
    paint_outer_directions(m)
    add_outer_entrances(m)
    add_combat_landmark(m)
    # One outer boundary; the old perimeter no longer divides the playground.
    m.box(-15300,-15200,-15200,15200,-300,120,"normal")
    m.box(15200,15300,-15200,15200,-300,120,"normal")
    m.box(-15300,15300,-15300,-15200,-300,120,"normal")
    m.box(-15300,15300,15200,15300,-300,120,"normal")
    # Out-of-bounds falls return through the ordinary level death warp.
    m.face([(-15500,-3600,-15500),(-15500,-3600,15500),(15500,-3600,15500),(15500,-3600,-15500)],
           COLORS["ground"],"SURFACE_DEATH_PLANE",layer="hidden")
    return m


def build_pole(height):
    m=Mesh()
    for i in range(8):
        a,b=i*math.tau/8,(i+1)*math.tau/8
        p=(32*math.sin(a),32*math.cos(a))
        q=(32*math.sin(b),32*math.cos(b))
        m.face([(p[0],0,p[1]),(q[0],0,q[1]),(q[0],height,q[1]),(p[0],height,p[1])],COLORS["pole"],None)
    m.floor(-32,32,-32,32,height,"pole",surface=None)
    return m


def build_tilter():
    m=Mesh()
    m.box(-450,450,-450,450,-100,0,"platform",bottom=True)
    return m


def build_movers():
    platform=Mesh()
    platform.box(-400,400,-400,400,-100,0,"platform",bottom=True)
    bridge=Mesh()
    bridge.box(-1100,1100,-250,250,-120,0,"platform",bottom=True)
    wall=Mesh()
    wall.box(-50,50,-500,500,0,800,"wall",bottom=True)
    disk=Mesh()
    for i in range(16):
        a=i*math.tau/16
        b=(i+1)*math.tau/16
        p=(650*math.sin(a),650*math.cos(a))
        q=(650*math.sin(b),650*math.cos(b))
        rgb=COLORS["platform" if i%2 else "tower"]
        disk.face([(0,0,0),(p[0],0,p[1]),(q[0],0,q[1])],rgb)
        disk.face([(q[0],-100,q[1]),(q[0],0,q[1]),(p[0],0,p[1]),(p[0],-100,p[1])],rgb)
        disk.face([(0,-100,0),(q[0],-100,q[1]),(p[0],-100,p[1])],rgb)
    return {"shuttle":platform,"disk":disk,"bridge":bridge,"closing_wall":wall}


def face_bounds(faces):
    points=[p for f in faces for p in f.points]
    return tuple(min(p[i] for p in points) for i in range(3)), tuple(max(p[i] for p in points) for i in range(3))


def flat_rectangle(face):
    """Recognize axis-aligned rectangles, retaining their winding."""
    if len(face.points)!=4:
        return None
    lo,hi=face_bounds([face])
    flat=[a for a in range(3) if lo[a]==hi[a]]
    if len(flat)!=1:
        return None
    axes=[a for a in range(3) if a!=flat[0]]
    if {(p[axes[0]],p[axes[1]]) for p in face.points}!={
            (u,v) for u in (lo[axes[0]],hi[axes[0]]) for v in (lo[axes[1]],hi[axes[1]])}:
        return None
    if any(sum(p[a]!=q[a] for a in axes)!=1 for p,q in zip(face.points,face.points[1:]+face.points[:1])):
        return None
    return flat[0],1 if normal(face.points)[flat[0]]>0 else -1,lo,hi


def ground_patches(mesh):
    """Coalesce the disjoint checker floor without changing its openings.

    Its original tiled faces still supply collision. Sorting is confined to
    this single, nonoverlapping plane, before all other terrain is drawn.
    """
    groups=defaultdict(list)
    for face in mesh.faces:
        if face.checker_color is None:
            continue
        axis,winding,lo,hi=flat_rectangle(face)
        assert axis==1 and winding==1 and lo[1]==0
        visible = subtract_rectangles((lo[0],hi[0],lo[2],hi[2]),mesh.covered_ground)
        groups[(face.checker_color,face.surface,face.force)].extend(visible)
    patches=[]
    for (color,surface,force),rectangles in groups.items():
        while True:
            previous=len(rectangles)
            for axis in (0,2):
                other=2-axis
                rows=defaultdict(list)
                for r in rectangles:
                    rows[(r[other],r[other+1])].append((r[axis],r[axis+1]))
                rectangles=[]
                for span,intervals in sorted(rows.items()):
                    joined=[]
                    for lo,hi in sorted(intervals):
                        if joined and joined[-1][1]==lo:
                            joined[-1]=(joined[-1][0],hi)
                        else:
                            joined.append((lo,hi))
                    for interval in joined:
                        rectangles.append((*interval,*span) if axis==0 else (*span,*interval))
            if len(rectangles)==previous:
                break
        for x0,x1,z0,z1 in rectangles:
            patches.append(Face(((x0,0,z0),(x0,0,z1),(x1,0,z1),(x1,0,z0)),color,surface,force,"checker",color))
    return sorted(patches,key=lambda f:(f.points[0][2],f.points[0][0]))


def checker_uv(point):
    # 16 texels per 1000-unit square, s10.5 vertex coordinates. Anchor all
    # patches to world space; both negative coordinates and seams wrap alike.
    uv=tuple(round(point[a]*512/1000) for a in (0,2))
    assert all(-32768<=v<=32767 for v in uv)
    return uv


def subtract_rectangles(rect, covers):
    """Exact rectangle subtraction: no center-only or area-sum assumptions."""
    remaining=[rect]
    for a,b,c,d in covers:
        next_remaining=[]
        for x0,x1,z0,z1 in remaining:
            l,r,t,u=max(a,x0),min(b,x1),max(c,z0),min(d,z1)
            if l>=r or t>=u:
                next_remaining.append((x0,x1,z0,z1))
                continue
            next_remaining.extend((left,right,top,bottom) for left,right,top,bottom in
                ((x0,l,z0,z1),(r,x1,z0,z1),(l,r,z0,t),(l,r,u,z1))
                if left<right and top<bottom)
        remaining=next_remaining
        if not remaining:
            break
    return remaining


def rectangle_covered(rect, covers):
    return not subtract_rectangles(rect, covers)


def closed_terrain_boxes(mesh):
    # Boxes without a bottom need a complete, opaque floor at their base.
    # Include submerged floors, but exclude gaps, LOD and Vanish fixtures.
    # Moving meshes never use this export path.
    floors = defaultdict(list)
    for face in mesh.faces:
        if (face.layer != "opaque" or face.lod_group is not None
                or face.surface == "SURFACE_VANISH_CAP_WALLS"):
            continue
        rect = flat_rectangle(face)
        if rect is not None:
            axis, sign, lo, hi = rect
            if axis == 1 and sign == 1:
                floors[lo[1]].append((lo[0],hi[0],lo[2],hi[2]))
    return [(lo,hi) for lo,hi,closed in mesh.boxes if closed or
            rectangle_covered((lo[0],hi[0],lo[2],hi[2]),floors[lo[1]])]


def exposed_terrain_faces(mesh):
    """Remove covered portions of static rectangles facing into opaque solids.

    Exact box footprints and closed end caps prove coverage. Retain exposed
    edges, attributes and ordering; collision never passes through this path.
    """
    boxes=closed_terrain_boxes(mesh)
    ground = [(min(p[0] for p in f.points),max(p[0] for p in f.points),
               min(p[2] for p in f.points),max(p[2] for p in f.points))
              for f in mesh.faces if f.checker_color is not None]
    caps = defaultdict(list)
    sealed_faces = defaultdict(list)
    for face, seal in mesh.end_caps:
        if (face.lod_group is not None or face.layer != "opaque"
                or face.surface == "SURFACE_VANISH_CAP_WALLS"):
            continue
        if seal is not None and not rectangle_covered(seal, ground):
            continue
        sealed_faces[frozenset(face.points)].append(face)
        rect = flat_rectangle(face)
        if rect is None:
            continue
        axis, sign, lo, hi = rect
        a,b = [i for i in range(3) if i != axis]
        caps[axis,sign,lo[axis]].append((lo[a],hi[a],lo[b],hi[b]))
    # Exact opposing ends are internal even when their sloped outline is not
    # rectangular. Both ends must belong to sealed solids; never weld vertices
    # or change the exposed top, collision, materials or alternate LOD faces.
    hidden_ends = set()
    for faces in sealed_faces.values():
        for face in faces:
            n = normal(face.points)
            if any(sum(a*b for a,b in zip(n,normal(other.points))) < 0 for other in faces):
                hidden_ends.add(id(face))
    visible=[]
    for face in mesh.faces:
        if id(face) in hidden_ends:
            continue
        if face.layer!="opaque" or face.checker_color is not None:
            continue
        rect=flat_rectangle(face)
        if rect is not None and face.surface!="SURFACE_VANISH_CAP_WALLS":
            axis,sign,lo,hi=rect
            a,b=[i for i in range(3) if i!=axis]
            # Require solid material immediately on the face's FRONT side.
            # Its own box, and merely coplanar exposed faces, cannot hide it.
            covers=[(bl[a],bh[a],bl[b],bh[b]) for bl,bh in boxes
                    if (bl[axis]<=lo[axis]<bh[axis] if sign>0 else bl[axis]<lo[axis]<=bh[axis])]
            if face.lod_group is None:
                covers += caps[axis,-sign,lo[axis]]
            footprint = (lo[a],hi[a],lo[b],hi[b])
            remaining = subtract_rectangles(footprint, covers)
            if remaining != [footprint]:
                for left,right,top,bottom in remaining:
                    lower,upper = list(lo),list(hi)
                    lower[a],upper[a] = left,right
                    lower[b],upper[b] = top,bottom
                    points = tuple(tuple(lower[i] if p[i] == lo[i] else upper[i]
                                         for i in range(3)) for p in face.points)
                    visible.append(replace(face, points=points))
                continue
        visible.append(face)
    return visible


def merge_terrain_faces(faces):
    """Render-only joins of consecutive, equally shaded rectangles.

    Requiring a complete shared edge proves the union is a rectangle. Never
    cross an intervening primitive or alter the collision/source mesh.
    """
    merged=[]
    for face in faces:
        merged.append(face)
        while len(merged)>1:
            a,b=merged[-2:]
            if (a.color,a.surface,a.force,a.layer,a.lod_group)!=(b.color,b.surface,b.force,b.layer,b.lod_group):
                break
            ra,rb=flat_rectangle(a),flat_rectangle(b)
            if ra is None or rb is None or ra[:2]!=rb[:2]:
                break
            axis,_,alo,ahi=ra
            _,_,blo,bhi=rb
            if alo[axis]!=blo[axis]:
                break
            join=[i for i in range(3) if i!=axis and (alo[i],ahi[i])!=(blo[i],bhi[i])]
            if len(join)!=1 or not (ahi[join[0]]==blo[join[0]] or bhi[join[0]]==alo[join[0]]):
                break
            lo=tuple(min(alo[i],blo[i]) for i in range(3))
            hi=tuple(max(ahi[i],bhi[i]) for i in range(3))
            points=tuple(tuple(lo[i] if p[i]==alo[i] else hi[i] for i in range(3)) for p in a.points)
            merged[-2:]=[Face(points,a.color,a.surface,a.force,a.layer,lod_group=a.lod_group)]
    return merged


def terrain_sections(faces):
    """Keep one prebuilt list per contiguous LOD run, preserving primitive order.

    Terrain no longer uses section bounds for visibility. Spatial/face-count
    splits only add list calls and discard reusable vertices between lists.
    Vertex uploads still obey render_body's 24-entry cache limit.
    """
    sections=[]
    current=[]
    for face in faces:
        if current and face.lod_group!=current[-1].lod_group:
            sections.append(current)
            current=[]
        current.append(face)
    if current:
        sections.append(current)
    return sections


def merge_clear_terrain_faces(faces):
    """Join separated coplanar rectangles only across provably unrelated draws.

    A union replaces the later face. Intervening bounds cannot cross its plane
    inside the footprint, including zero-width walls. Border contacts remain
    the same shared edges. Preserve interior depth ties and intersections.
    The same color, winding and complete shared edge requirements apply.
    """
    output=[]
    bounds=[]
    groups=defaultdict(list)
    for face in faces:
        rect=flat_rectangle(face)
        if rect:
            axis,sign,lo,_=rect
            axes=[a for a in range(3) if a!=axis]
            key=(axis,sign,lo[axis],face.color,face.surface,face.force,face.layer,face.lod_group)
            for i in reversed(groups[key]):
                if output[i] is None:
                    continue
                joined=merge_terrain_faces([output[i],face])
                if len(joined)!=1:
                    continue
                low,high=face_bounds(joined)
                if any(l[axis]<=lo[axis]<=h[axis] and
                       all(l[a]<high[a] and h[a]>low[a] for a in axes)
                       for f,(l,h) in zip(output[i+1:],bounds[i+1:]) if f is not None):
                    continue
                face=joined[0]
                output[i]=None
            groups[key].append(len(output))
        output.append(face)
        bounds.append(face_bounds([face]))
    return [f for f in output if f is not None]


def exposed_pool_banks(faces):
    """Share the pool floor's actual edge instead of crossing a buried wall."""
    banks=[(6400,6640,1000,3100),(6400,6640,4100,6000)]
    floors=[]
    edges=defaultdict(list)
    for face in faces:
        xs=sorted({p[0] for p in face.points})
        zs=sorted({p[2] for p in face.points})
        ys=[p[1] for p in face.points]
        if (len(xs)!=2 or len(zs)!=2 or len(face.points)!=4 or normal(face.points)[1]<=0
                or min(ys)>=0 or max(ys)>0 or face.lod_group is not None
                or not (2400<=xs[0]<xs[1]<=11600 and 1000<=zs[0]<zs[1]<=6000)):
            floors.append(face)
            continue
        heights={z:{p[1] for p in face.points if p[2]==z} for z in zs}
        if any(len(values)!=1 for values in heights.values()):
            floors.append(face)
            continue
        y0,y1=[next(iter(heights[z])) for z in zs]
        def height(z):
            return round(y0+(y1-y0)*(z-zs[0])/(zs[1]-zs[0]))
        # Both sides use the same integer stations at the swimming opening.
        stops=[zs[0]]+[z for z in (3100,4100) if zs[0]<z<zs[1]]+[zs[1]]
        for z0,z1 in zip(stops,stops[1:]):
            for x0,x1,a,b in subtract_rectangles((xs[0],xs[1],z0,z1),banks):
                points=((x0,height(a),a),(x0,height(b),b),
                        (x1,height(b),b),(x1,height(a),a))
                floors.append(replace(face,points=points))
                for x in (x0,x1):
                    if x in (6400,6640):
                        edges[x].append((a,b,height(a),height(b)))
    visible=[]
    for face in floors:
        rect=flat_rectangle(face)
        if (rect is not None and rect[0]==0 and rect[2][0] in edges
                and rect[2][1]==-2000 and rect[3][1]==0
                and (rect[2][2],rect[3][2]) in ((1000,3100),(4100,6000))):
            _,sign,lo,hi=rect
            segments=sorted({edge for edge in edges[lo[0]] if lo[2]<=edge[0]<edge[1]<=hi[2]})
            if (not segments or segments[0][0]!=lo[2] or segments[-1][1]!=hi[2]
                    or any(a[1]!=b[0] or a[3]!=b[2] for a,b in zip(segments,segments[1:]))):
                visible.append(face)
                continue
            for z0,z1,y0,y1 in segments:
                points=((lo[0],y1,z1),(lo[0],0,z1),(lo[0],0,z0),(lo[0],y0,z0))
                if sign>0:
                    points=points[::-1]
                points=tuple(dict.fromkeys(points))
                if len(points)>=3:
                    visible.append(replace(face,points=points))
            continue
        visible.append(face)
    return visible


def exposed_flight_tunnel_roof_joins(faces):
    """Trim the sealed roof/side-strip joins without changing collision."""
    replacements={}
    candidates={frozenset(face.points):face for face in faces
                if face.layer=="opaque" and face.lod_group is None
                and face.surface==DEFAULT and face.color==COLORS["tower"]}
    for z in (-10500,-9100):
        roof=frozenset(((4500,2400,z),(4500,2550,z),(7500,1950,z),(7500,1800,z)))
        side=frozenset(((4500,0,z),(4500,2550,z),(7500,1950,z),(7500,0,z)))
        if roof not in candidates or side not in candidates:
            continue
        roof_face,side_face=candidates[roof],candidates[side]
        if sum(a*b for a,b in zip(normal(roof_face.points),normal(side_face.points)))>=0:
            continue
        # The slab fills the top 150 units of the strip's inward face. Its
        # opposite cap is buried completely; the tunnel wall below stays visible.
        replacements[id(roof_face)]=None
        replacements[id(side_face)]=replace(side_face,points=tuple(
            (x,y-150 if y>0 else y,z) for x,y,z in side_face.points))
    return [replacements.get(id(face),face) for face in faces
            if id(face) not in replacements or replacements[id(face)] is not None]


def exposed_slide_circuit_joins(faces):
    """Omit straight end walls buried inside the adjoining banked turns.

    Both turn LODs share these complete riding edges with the straights. The
    walls' top edges otherwise bleed through the riding surface. Collision
    retains the original closed wedges, as do the circuit's exposed gap ends.
    """
    buried = {
        frozenset(((x,0,z0),(x,0,z1),(x,y,z0),(x,y,z1)))
        for x,y,z0,z1 in ((-9500,400,-12250,-11350),
                         (-4000,400,-12250,-11350),
                         (-9500,1400,-8650,-7750),
                         (-4000,400,-8650,-7750))
    }
    return [face for face in faces if frozenset(face.points) not in buried]


def exposed_runway_and_boundary_joins(faces):
    """Trim only audited wall portions inside the connected ramps or wall corners."""
    covered = {
        (2,10000): [(13800,15000,0,20)],
        (0,13800): [(0,20,2200,2800)],
        (2,-15200): [(-15300,-15200,-300,120),(15200,15300,-300,120)],
        (2,15200): [(-15300,-15200,-300,120),(15200,15300,-300,120)],
    }
    visible=[]
    for face in faces:
        rect=flat_rectangle(face)
        if rect is None or face.layer!="opaque" or face.lod_group is not None or face.surface!=DEFAULT:
            visible.append(face)
            continue
        axis,_,lo,hi=rect
        covers=covered.get((axis,lo[axis]))
        if covers is None:
            visible.append(face)
            continue
        a,b=[i for i in range(3) if i!=axis]
        for left,right,top,bottom in subtract_rectangles((lo[a],hi[a],lo[b],hi[b]),covers):
            lower,upper=list(lo),list(hi)
            lower[a],upper[a]=left,right
            lower[b],upper[b]=top,bottom
            points=tuple(tuple(lower[i] if p[i]==lo[i] else upper[i]
                               for i in range(3)) for p in face.points)
            visible.append(replace(face,points=points))
    return visible


def static_terrain_faces(mesh):
    faces=merge_clear_terrain_faces(merge_terrain_faces(exposed_terrain_faces(mesh)))
    faces=exposed_slide_circuit_joins(exposed_flight_tunnel_roof_joins(exposed_pool_banks(faces)))
    return exposed_runway_and_boundary_joins(faces)


def render_body(faces, name, layer="opaque", cull=False, cull_padding=8, vertex_colors=None, north_variant=False):
    vertices = []
    cmds = []
    north_cmds = []
    aerial_cmds = []
    cache = {}
    draws = []

    def flush():
        if not cache:
            return
        cmds.append(f"    gsSPVertex({name}_vertices + {len(vertices)}, {len(cache)}, 0),")
        for p, uv, rgba in cache:
            vertices.append("    {{{ %d, %d, %d }, 0, { %d, %d }, { %d, %d, %d, %d }}}," % (*p,*uv,*rgba))
        cmds.extend(command for command,_,_,_ in draws)
        retained=[(command,indices) for command,indices,back,_ in draws if not back]
        if north_variant and retained:
            used=sorted({i for _,indices in retained for i in indices})
            offset=len(vertices)-len(cache)+used[0]
            north_cmds.append(f"    gsSPVertex({name}_vertices + {offset}, {used[-1]-used[0]+1}, {used[0]}),")
            north_cmds.extend(command for command,_ in retained)
        retained=[(command,indices) for command,indices,_,back in draws if not back]
        if north_variant and retained:
            used=sorted({i for _,indices in retained for i in indices})
            offset=len(vertices)-len(cache)+used[0]
            aerial_cmds.append(f"    gsSPVertex({name}_vertices + {offset}, {used[-1]-used[0]+1}, {used[0]}),")
            aerial_cmds.extend(command for command,_ in retained)
        cache.clear()
        draws.clear()

    # Preserve face/triangle order and share only byte-identical attributes.
    # Retain the existing 24-vertex limit for every supported microcode.
    for face in faces:
        rgba = (*shaded_color(face.points, face.color), 110 if layer == "water" else 255)
        keys = [(p, checker_uv(p) if layer == "checker" else (0, 0),
                 (*vertex_colors[p, rgba[:3]], rgba[3]) if vertex_colors is not None else rgba)
                for p in face.points]
        if len(cache.keys() | set(keys)) > 24:
            flush()
        indices = [cache.setdefault(key, len(cache)) for key in keys]
        n=normal(face.points)
        back=(n[0]==0 and n[1]==0 and n[2]>0
              and face.points[0][2]>=NORTH_BACKFACE_PLANE_Z
              and all(p[2]==face.points[0][2] for p in face.points))
        side=(n[1]==0 and n[2]==0 and all(p[0]==face.points[0][0] for p in face.points)
              and ((n[0]>0 and face.points[0][0]>=AERIAL_CAMERA_HALF_WIDTH) or (n[0]<0 and face.points[0][0]<=-AERIAL_CAMERA_HALF_WIDTH)))
        bottom=n[0]==0 and n[2]==0 and n[1]<0 and all(p[1]==face.points[0][1] for p in face.points)
        aerial_back=back or side or bottom
        if len(indices) == 4:
            a, b, c, d = indices
            draws.append((f"    gsSP2Triangles({a}, {b}, {c}, 0, {a}, {c}, {d}, 0),",indices,back,aerial_back))
        else:
            draws.append(("    gsSP1Triangle(%d, %d, %d, 0)," % tuple(indices),indices,back,aerial_back))
    flush()
    bounds = []
    cull_commands = []
    if cull:
        lo, hi = face_bounds(faces)
        # Outward padding covers fixed-point transforms at screen edges.
        bounds = [f"static const Vtx {name}_bounds[] = {{"]
        for mask in range(8):
            p = tuple(hi[i] + cull_padding if mask & (1 << i) else lo[i] - cull_padding for i in range(3))
            bounds.append("    {{{ %d, %d, %d }, 0, { 0, 0 }, { 255, 255, 255, 255 }}}," % p)
        bounds.append("};")
        cull_commands = [f"    gsSPVertex({name}_bounds, 8, 0),", "    gsSPCullDisplayList(0, 7),"]
    alternate=([f"static const Gfx {name}_north[] = {{",*cull_commands,*north_cmds,
                "    gsSPEndDisplayList(),","};", "",
                f"static const Gfx {name}_aerial[] = {{",*cull_commands,*aerial_cmds,
                "    gsSPEndDisplayList(),","};",""] if north_variant else [])
    return "\n".join([*bounds,f"static const Vtx {name}_vertices[] = {{",*vertices,"};",
        f"static const Gfx {name}[] = {{",*cull_commands,*cmds,"    gsSPEndDisplayList(),","};",*alternate,""])


def material_begin(layer):
    return ["    gsDPPipeSync(),",
        "    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK)," if layer=="water" else "    gsSPClearGeometryMode(G_LIGHTING),",
        "    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),", "    gsSPTexture(0, 0, 0, G_TX_RENDERTILE, G_OFF),"]


def material_end():
    return ["    gsDPPipeSync(),", "    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),",
        "    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),"]


def render_c(mesh,name,layer="opaque",cull=False):
    faces=[f for f in mesh.faces if f.layer==layer]
    body=name+"_body"
    # A culled child returns to this wrapper, so material restoration still runs.
    return render_body(faces,body,layer,cull)+"\n".join([f"const Gfx {name}[] = {{",
        *material_begin(layer),f"    gsSPDisplayList({body}),",*material_end(),
        "    gsSPEndDisplayList(),","};",""])


def render_bounded_terrain_run(faces, name, layer):
    """Bound consecutive groups without reordering any surviving primitive."""
    groups=[]
    current=[]
    triangles=0
    for face in faces:
        candidate=current+[face]
        lo,hi=face_bounds(candidate)
        count=len(face.points)-2
        if current and (triangles+count>256 or any(hi[a]-lo[a]>TERRAIN_GROUP_MAX_SPAN for a in (0,2))):
            groups.append(current)
            current=[]
            triangles=0
        current.append(face)
        triangles+=count
    if current:
        groups.append(current)
    names=[f"{name}_group_{i}" for i in range(len(groups))]
    lines=[render_body(group,child,layer,cull=True,north_variant=True) for group,child in zip(groups,names)]
    for suffix in ("", "_north", "_aerial"):
        lines.append(f"static const Gfx {name}{suffix}[] = {{")
        for group,child in zip(groups,names):
            lo,hi=face_bounds(group)
            oversized=any(hi[a]-lo[a]>TERRAIN_GROUP_MAX_SPAN for a in (0,2))
            # A single long face cannot satisfy the grouping span. Guard-band
            # clipping can stretch it across the screen near the camera plane.
            # Set normal clipping before transforming vertices; restore outside
            # the child so its early cull return cannot leak the setting.
            if oversized:
                lines.append("    gsSPClipRatio(FRUSTRATIO_1),")
            lines.append(f"    gsSPDisplayList({child}{suffix}),")
            if oversized:
                lines.append("    gsSPClipRatio(FRUSTRATIO_2),")
        lines.extend(["    gsSPEndDisplayList(),","};",""])
    return "\n".join(lines)


def render_section_batches(sections,prefix,layer="opaque",lods=None):
    names=[f"physics_lab_{prefix}_section_{i}" for i in range(len(sections))]
    lods=lods or {}
    lod_names=list(lods)
    assert len(lod_names)<=8
    distant_names=[]
    masks=[]
    exported_far=defaultdict(list)
    # All structure runs use consecutive padded RSP groups, including LODs.
    # Ground keeps its existing stream; no CPU visibility work is added.
    lines=[(render_bounded_terrain_run(faces,name,layer)
            if prefix=="terrain" else render_body(faces,name,layer))
           for faces,name in zip(sections,names)]
    for faces,name in zip(sections,names):
        group=faces[0].lod_group
        distant_names.append(name+"_distant" if group else name)
        masks.append(1<<lod_names.index(group) if group else 0)
        if group:
            # All low-detail vertices must remain inside the near bounds, so
            # both choices retain the same fixture extent.
            lo,hi=face_bounds(faces)
            far=[f for f in lods[group] if all(all(lo[a]<=p[a]<=hi[a] for a in range(3)) for p in f.points)]
            assert far, group
            exported_far[group].extend(far)
            lines.append(render_bounded_terrain_run(far,name+"_distant",layer)
                         if prefix=="terrain" else render_body(far,name+"_distant",layer))
    # Reject an authored LOD that straddles a section boundary or would be
    # submitted twice. Near meshes retain their original ordered chunks.
    for group,faces in lods.items():
        assert sorted(map(id,faces))==sorted(map(id,exported_far[group])), group
    if prefix=="terrain":
        # The static final run has no LOD; labels retain their exact opaque depth.
        assert not masks[-1]
        last=names[-1]
        old=f"static const Gfx {last}[] = {{"
        north_old=f"static const Gfx {last}_north[] = {{"
        aerial_old=f"static const Gfx {last}_aerial[] = {{"
        lines=[part.replace(old,old+"\n    gsSPDisplayList(physics_lab_labels_dl),")
                   .replace(north_old,north_old+"\n    gsSPDisplayList(physics_lab_labels_dl),")
                   .replace(aerial_old,aerial_old+"\n    gsSPDisplayList(physics_lab_labels_dl),") for part in lines]
    lines.append(f"const u32 physics_lab_{prefix}_section_count = {len(sections)};")
    if prefix == "terrain":
        runs=[]
        for i,mask in enumerate(masks):
            if runs and runs[-1][2]==mask:
                runs[-1][1]=i+1
            else:
                runs.append([i,i+1,mask])
        for n,(first,end,mask) in enumerate(runs):
            for distant in (False,True) if mask else (False,):
                name=f"physics_lab_terrain_batch_{n}"+("_distant" if distant else "")
                lists=distant_names if distant else names
                lines.append(f"static const Gfx {name}[] = {{")
                lines.extend(f"    gsSPDisplayList({name})," for name in lists[first:end])
                lines.extend(["    gsSPEndDisplayList(),","};"])
        lines.append("const struct PhysicsLabTerrainBatch physics_lab_terrain_batches[] = {")
        for n,(_,_,mask) in enumerate(runs):
            name=f"physics_lab_terrain_batch_{n}"
            far=name+"_distant" if mask else name
            lines.append(f"    {{ {name}, {far}, {mask} }},")
        lines.extend(["};",f"const u32 physics_lab_terrain_batch_count = {len(runs)};",""])
    if prefix=="terrain":
        # Reuse the original vertices and padded bounds. The alternate commands
        # omit only +Z planar faces when the camera is beyond the whole mesh.
        for n,(first,end,mask) in enumerate(runs):
            for distant in (False,True) if mask else (False,):
                name=f"physics_lab_terrain_batch_{n}"+("_distant" if distant else "")+"_north"
                lists=distant_names if distant else names
                lines.append(f"static const Gfx {name}[] = {{")
                lines.extend(f"    gsSPDisplayList({child}_north)," for child in lists[first:end])
                lines.extend(["    gsSPEndDisplayList(),","};"])
        lines.append("const struct PhysicsLabTerrainBatch physics_lab_terrain_north_batches[] = {")
        for n,(_,_,mask) in enumerate(runs):
            name=f"physics_lab_terrain_batch_{n}_north"
            far=f"physics_lab_terrain_batch_{n}_distant_north" if mask else name
            lines.append(f"    {{ {name}, {far}, {mask} }},")
        lines.extend(["};",""])
    if prefix=="terrain":
        for n,(first,end,mask) in enumerate(runs):
            for distant in (False,True) if mask else (False,):
                name=f"physics_lab_terrain_batch_{n}"+("_distant" if distant else "")+"_aerial"
                lists=distant_names if distant else names
                lines.append(f"static const Gfx {name}[] = {{")
                lines.extend(f"    gsSPDisplayList({child}_aerial)," for child in lists[first:end])
                lines.extend(["    gsSPEndDisplayList(),","};"])
        lines.append("const struct PhysicsLabTerrainBatch physics_lab_terrain_aerial_batches[] = {")
        for n,(_,_,mask) in enumerate(runs):
            name=f"physics_lab_terrain_batch_{n}_aerial"
            far=f"physics_lab_terrain_batch_{n}_distant_aerial" if mask else name
            lines.append(f"    {{ {name}, {far}, {mask} }},")
        lines.extend(["};",""])
    if lods:
        lines.append("const struct PhysicsLabRenderSection physics_lab_terrain_lod_bounds[] = {")
        for group in lod_names:
            faces=[f for section in sections for f in section if f.lod_group==group]
            lo,hi=face_bounds(faces)
            lines.append("    { { %d, %d, %d }, { %d, %d, %d }, NULL }," % (*lo,*hi))
        lines.extend(["};",f"const u32 physics_lab_terrain_lod_count = {len(lods)};",""])
    return "\n".join(lines)


def without_textured_labels(mesh,faces):
    """Subtract only proven complete flat footprints from the render mesh."""
    for (x0,x1,z0,z1),height,*_ in mesh.label_textures:
        output=[]
        for face in faces:
            rect=flat_rectangle(face)
            if rect is None or rect[:2]!=(1,1) or rect[2][1]!=height:
                output.append(face)
                continue
            lo,hi=rect[2:]
            l,r,t,u=max(lo[0],x0),min(hi[0],x1),max(lo[2],z0),min(hi[2],z1)
            if l>=r or t>=u:
                output.append(face)
                continue
            for a,b,c,d in [(lo[0],l,lo[2],hi[2]),(r,hi[0],lo[2],hi[2]),
                            (l,r,lo[2],t),(l,r,u,hi[2])]:
                if a<b and c<d:
                    output.append(Face(((a,height,c),(a,height,d),(b,height,d),(b,height,c)),
                                       face.color,face.surface,face.force,face.layer,
                                       face.checker_color,face.lod_group))
        faces=output
    return faces


def render_label_textures(mesh):
    lines=[]
    names=[]
    for index,((x0,x1,z0,z1),height,pixels,background,cell,checker) in enumerate(mesh.label_textures):
        width=(len(pixels[0])+7)//8*8
        assert width*8*4<=4096
        xs=[x0]+([x for x in range(math.floor(x0/1000)*1000+1000,x1,1000)] if checker else [])+[x1]
        zs=[z0]+([z for z in range(math.floor(z0/1000)*1000+1000,z1,1000)] if checker else [])+[z1]
        # A label within one checker cell needs only that cell's palette.
        intensities = {
            235 if (math.floor((left + right) / 2000) + math.floor((top + bottom) / 2000)) % 2 == 0 else 255
            for left, right in zip(xs, xs[1:])
            for top, bottom in zip(zs, zs[1:])
        } if checker else {None}
        for intensity in (sorted(intensities) if checker else (None,)):
            name=f"physics_lab_label_{index}"+(f"_{intensity}" if checker else "")
            lines.append(f"ALIGNED8 static const u8 {name}_texture[] = {{")
            for t in range(8):
                row=[]
                for x in range(width):
                    color=COLORS["label"] if t<5 and x<len(pixels[0]) and pixels[t][x]=="1" else background
                    # Match the one-cycle checker combiner's 8-bit product.
                    if intensity is not None:
                        color=tuple((c*intensity+128)//256 for c in color)
                    row.extend((*color,255))
                lines.append("    "+", ".join(map(str,row))+",")
            lines.append("};")
        for x,left in enumerate(xs[:-1]):
            right=xs[x+1]
            for z,top in enumerate(zs[:-1]):
                bottom=zs[z+1]
                name=f"physics_lab_label_{index}_{x}_{z}"
                names.append(name)
                intensity=235 if (math.floor((left+right)/2000)+math.floor((top+bottom)/2000))%2==0 else 255
                texture=f"physics_lab_label_{index}"+(f"_{intensity}" if checker else "")
                lines.append(f"static const Vtx {name}_vertices[] = {{")
                for point in [(left,height,top),(left,height,bottom),(right,height,bottom),(right,height,top)]:
                    uv=(round((point[0]-x0)*32/cell),round((point[2]-z0)*32/cell))
                    lines.append("    {{{ %d, %d, %d }, 0, { %d, %d }, { 255, 255, 255, 255 }}}," % (*point,*uv))
                lines.extend(["};",f"static const Gfx {name}[] = {{",
                    f"    gsSPVertex({name}_vertices, 4, 0),",
                    "    gsSPCullDisplayList(0, 3),",
                    "    gsDPPipeSync(),",
                    f"    gsDPLoadTextureBlock({texture}_texture, G_IM_FMT_RGBA, G_IM_SIZ_32b, {width}, 8, 0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),",
                    "    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),",
                    "    gsSPEndDisplayList(),","};"])
    lines.extend(["static const Gfx physics_lab_labels_dl[] = {",
        "    gsDPPipeSync(),",
        "    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),",
        "    gsDPSetTextureFilter(G_TF_POINT),",
        "    gsDPSetTexturePersp(G_TP_PERSP),",
        "    gsDPSetTextureLUT(G_TT_NONE),",
        "    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),"])
    lines.extend(f"    gsSPDisplayList({name})," for name in names)
    lines.extend(["    gsDPPipeSync(),","    gsSPTexture(0, 0, 0, G_TX_RENDERTILE, G_OFF),",
        "    gsDPSetTextureFilter(G_TF_BILERP),","    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),",
        "    gsSPEndDisplayList(),","};"])
    return "\n".join(lines)


def render_terrain(mesh):
    ground=terrain_sections(without_textured_labels(mesh,ground_patches(mesh)))
    terrain=terrain_sections(without_textured_labels(mesh,static_terrain_faces(mesh)))
    # A single 1 KiB intensity texture supplies all ground/path colors via
    # vertex shading. Point sampling keeps checker boundaries crisp.
    lines=["#ifdef USE_FRUSTRATIO2",
           "#define PHYSICS_LAB_BASE_CLIP_RATIO FRUSTRATIO_2",
           "#else",
           "#define PHYSICS_LAB_BASE_CLIP_RATIO FRUSTRATIO_1",
           "#endif",
           "// Expand the selected baseline before GBI token pasting.",
           "#define PHYSICS_LAB_SET_CLIP_RATIO(ratio) gsSPClipRatio(ratio)",
           "ALIGNED8 static const u8 physics_lab_checker_texture[] = {"]
    for t in range(32):
        lines.append("    "+", ".join(str(235 if (s//16+t//16)%2==0 else 255) for s in range(32))+",")
    lines.append("};")
    complete_faces=[face for section in terrain for face in section]
    complete_faces.extend(face for faces in mesh.lods.values() for face in faces)
    north_limit=min((face.points[0][2] for face in complete_faces
                     if (n:=normal(face.points))[0]==n[1]==0 and n[2]>0
                     and face.points[0][2]>=NORTH_BACKFACE_PLANE_Z
                     and all(p[2]==face.points[0][2] for p in face.points)),
                    default=NORTH_BACKFACE_PLANE_Z)
    ground_lo,ground_hi=face_bounds([face for section in ground for face in section])
    lines.append("const struct PhysicsLabRenderSection physics_lab_ground_clip_bounds = { "
                 + "{ "+", ".join(map(str,ground_lo))+" }, { "
                 + ", ".join(map(str,ground_hi))+" }, NULL };")
    lines.append(f"const f32 physics_lab_north_view_z = {north_limit}.0f;")
    high_limit=max(p[1] for section in terrain for face in section for p in face.points)
    lines.append(f"const f32 physics_lab_aerial_view_y = {high_limit}.0f;")
    lines.append(f"const f32 physics_lab_aerial_view_half_width = {AERIAL_CAMERA_HALF_WIDTH}.0f;")
    lines.extend([render_label_textures(mesh),render_section_batches(ground,"ground","checker"),render_section_batches(terrain,"terrain",lods=mesh.lods)])
    # Merged checker patches can straddle the camera plane in close views.
    # Keep normal clipping for their vertices; a wider guard can leave holes.
    begin=["    gsSPClipRatio(FRUSTRATIO_1),", "    gsDPPipeSync(),", "    gsSPClearGeometryMode(G_LIGHTING),",
        "    gsDPSetCombineMode(G_CC_MODULATEI, G_CC_MODULATEI),",
        "    gsDPSetTextureFilter(G_TF_POINT),", "    gsDPSetTexturePersp(G_TP_PERSP),",
        "    gsDPSetTextureLUT(G_TT_NONE),",
        "    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),",
        "    gsDPLoadTextureBlock(physics_lab_checker_texture, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0, G_TX_WRAP, G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),"]
    end=["    PHYSICS_LAB_SET_CLIP_RATIO(PHYSICS_LAB_BASE_CLIP_RATIO),", "    gsDPPipeSync(),", "    gsSPTexture(0, 0, 0, G_TX_RENDERTILE, G_OFF),",
         "    gsDPSetTextureFilter(G_TF_BILERP),", *material_end()[1:]]
    for prefix,sections,opening,closing in [("ground",ground,begin,end),
                                           ("terrain",terrain,
                                            ["    gsSPClipRatio(FRUSTRATIO_2),", *material_begin("opaque")],
                                            ["    PHYSICS_LAB_SET_CLIP_RATIO(PHYSICS_LAB_BASE_CLIP_RATIO),", *material_end()])]:
        for suffix,commands in [("begin",opening),("end",closing)]:
            lines.extend([f"const Gfx physics_lab_{prefix}_{suffix}[] = {{",*commands,"    gsSPEndDisplayList(),","};"])
        lines.append(f"const Gfx physics_lab_{prefix}_dl[] = {{")
        if prefix=="terrain":
            lines.append("    gsSPDisplayList(physics_lab_ground_dl),")
        lines.append(f"    gsSPDisplayList(physics_lab_{prefix}_begin),")
        lines.extend(f"    gsSPDisplayList(physics_lab_{prefix}_section_{i})," for i in range(len(sections)))
        lines.extend([f"    gsSPDisplayList(physics_lab_{prefix}_end),","    gsSPEndDisplayList(),","};"])
        if prefix=="ground":
            # The camera callback permits this alternate only when the entire
            # floor is well in front of its validated camera plane.
            wide_opening=["    gsSPClipRatio(FRUSTRATIO_2),",*opening[1:]]
            lines.extend(["static const Gfx physics_lab_ground_wide_begin[] = {",
                          *wide_opening,"    gsSPEndDisplayList(),","};"])
            lines.extend(["const Gfx physics_lab_ground_wide_dl[] = {",
                          "    gsSPDisplayList(physics_lab_ground_wide_begin),"])
            lines.extend(f"    gsSPDisplayList(physics_lab_ground_section_{i}),"
                         for i in range(len(sections)))
            lines.extend(["    gsSPDisplayList(physics_lab_ground_end),",
                          "    gsSPEndDisplayList(),","};"])
    return "\n".join(lines)+"\n"


# One shared basis retains exact quarter-unit coordinates in signed 16-bit
# vertices. All specimens fit; collision remains in its existing world mesh.
SCULPTURE_ORIGIN=(-8000,0,10500)
SCULPTURE_SCALE=.25


def sculpture_instances():
    instances=[(i,x,z,1,8+i) for i,(x,z,*_) in enumerate(SCULPTURES)]
    for i,(x,z,*_) in enumerate(SCULPTURES):
        instances.extend([(i,x-600,z+2100,.5,14+i),(i,x+650,z+2100,.25,20+i)])
    return instances


def render_sculptures(terrain):
    lines=[]
    records=[]
    for instance,(material,cx,cz,scale,slot) in enumerate(sculpture_instances()):
        meshes=[]
        for distant in (False,True):
            visual=Mesh()
            rgb=SCULPTURES[material][3]
            for i,tri in sculpture_render_triangles(distant):
                visual.face(tri,tuple(round(c*(.84,.95,1.0)[i%3]) for c in rgb),None)
            direction=1 if scale==1 else -1
            world=[tuple((cx+direction*x*scale,y*scale,cz+direction*z*scale) for x,y,z in f.points) for f in visual.faces]
            # Preserve the original facet colors and exact positions, including
            # fractional coordinates of the half/quarter-size rotated copies.
            baked=[Face(tuple(tuple(round((p[a]-SCULPTURE_ORIGIN[a])/SCULPTURE_SCALE) for a in range(3)) for p in points),f.color,None)
                   for points,f in zip(world,visual.faces)]
            assert all(-32736<=v<=32735 for f in baked for p in f.points for v in p)
            assert all(shaded_color(f.points,f.color)==shaded_color(old.points,old.color) for f,old in zip(baked,visual.faces))
            name=f"physics_lab_sculpture_{instance}"+("_distant" if distant else "")
            vertex_colors = None
            if distant:
                # Keep the angular shading: share only incident corner shades
                # whose channel ranges fit within 8/255. Every position and
                # primitive is retained, and nearby facets remain untouched.
                groups = defaultdict(list)
                shared = {}
                for face in baked:
                    color = shaded_color(face.points, face.color)
                    for point in face.points:
                        for shades in groups[point]:
                            candidate = shades + [color]
                            if all(max(c[a] for c in candidate) - min(c[a] for c in candidate) <= 8
                                   for a in range(3)):
                                shades.append(color)
                                break
                        else:
                            shades = [color]
                            groups[point].append(shades)
                        shared[point, color] = shades
                vertex_colors = {
                    key: tuple(round(sum(c[a] for c in colors) / len(colors)) for a in range(3))
                    for key, colors in shared.items()
                }
            lines.append(render_body(baked,name,cull=True,cull_padding=32,
                                     vertex_colors=vertex_colors))
            meshes.append(name)
            if not distant:
                lo=tuple(math.floor(min(p[a] for points in world for p in points))-9 for a in range(3))
                hi=tuple(math.ceil(max(p[a] for points in world for p in points))+9 for a in range(3))
        records.append((lo,hi,*meshes,slot))
    lines.append("const struct PhysicsLabSculpture physics_lab_sculptures[] = {")
    for lo,hi,near,far,slot in records:
        lines.append("    { { { %d, %d, %d }, { %d, %d, %d }, %s }, %s, %d }," % (*lo,*hi,near,far,slot))
    lines.extend(["};",f"const u32 physics_lab_sculpture_count = {len(records)};",""])
    lines.extend(["const Gfx physics_lab_sculptures_dl[] = {", "    gsSPDisplayList(physics_lab_terrain_begin),"])
    lines.extend(f"    gsSPDisplayList({near})," for _,_,near,_,_ in records)
    lines.extend(["    gsSPDisplayList(physics_lab_terrain_end),","    gsSPEndDisplayList(),","};",""])
    return "\n".join(lines)


def collision_c(mesh,name,water=False):
    vertices=[]
    indices={}
    groups=defaultdict(list)
    for f,tri in mesh.triangles():
        if f.surface is None:
            continue
        ids=[]
        for p in tri:
            if p not in indices:
                indices[p]=len(vertices)
                vertices.append(p)
            ids.append(indices[p])
        groups[f.surface].append((ids,f.force))
    lines=[f"const Collision {name}[] = {{","    COL_INIT(),",f"    COL_VERTEX_INIT({len(vertices)}),"]
    lines += ["    COL_VERTEX(%d, %d, %d)," % p for p in vertices]
    for surf,tris in groups.items():
        lines.append(f"    COL_TRI_INIT({surf}, {len(tris)}),")
        special=surf in {"SURFACE_FLOWING_WATER","SURFACE_HORIZONTAL_WIND","SURFACE_SHALLOW_MOVING_QUICKSAND"}
        for ids,force in tris:
            if special:
                lines.append("    COL_TRI_SPECIAL(%d, %d, %d, 0x%04X)," % (*ids,force))
            else:
                lines.append("    COL_TRI(%d, %d, %d)," % tuple(ids))
    lines.append("    COL_TRI_STOP(),")
    if water:
        lines.append(f"    COL_WATER_BOX_INIT({len(WATER_BOXES)}),")
        lines += ["    COL_WATER_BOX(%d, %d, %d, %d, %d, %d)," % box for box in WATER_BOXES]
    lines += ["    COL_END(),","};",""]
    return "\n".join(lines)


def outputs():
    terrain = build_terrain()
    tilter = build_tilter()
    banner = "// Generated by tools/generate_physics_lab.py; edit the generator.\n\n"

    model = banner + render_terrain(terrain)
    model += render_c(terrain, "physics_lab_water_dl", "water")
    model += render_c(build_pole(1200), "physics_lab_pole_short_dl")
    model += render_c(build_pole(2000), "physics_lab_pole_tall_dl")
    model += render_c(build_pole(3800), "physics_lab_pole_impact_dl")
    model += render_c(build_pole(3200), "physics_lab_pole_launch_dl")
    model += render_c(tilter, "physics_lab_tilter_dl")
    collision = banner + collision_c(terrain, "physics_lab_collision", True)
    collision += collision_c(tilter, "physics_lab_tilter_collision")
    result = {
        "model.inc.c": model,
        "collision.inc.c": collision,
    }
    for name, mesh in build_movers().items():
        result["model.inc.c"] += render_c(mesh, f"physics_lab_{name}_dl")
        result["collision.inc.c"] += collision_c(mesh, f"physics_lab_{name}_collision")
    result["model.inc.c"] += render_sculptures(terrain)
    return result


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check",action="store_true",help="verify checked-in outputs without changing them")
    parser.add_argument("--mesh-json",type=Path,help="optional triangle export for offline inspection")
    args=parser.parse_args()
    OUT.mkdir(exist_ok=True)
    for name,data in outputs().items():
        path=OUT/name
        if args.check:
            if not path.exists() or read_source(path)!=data:
                raise SystemExit(f"Stale generated geometry: {path}")
        else:
            path.write_text(data)
    mesh=build_terrain()
    tris=list(mesh.triangles())
    if args.mesh_json:
        args.mesh_json.write_text(json.dumps([dict(points=t,color=f.color,surface=f.surface,force=f.force,layer=f.layer) for f,t in tris]))
    print(f"Physics Lab: {len(tris)} triangles; generated geometry {'verified' if args.check else 'written'}.")


if __name__=="__main__":
    main()
