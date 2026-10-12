#!/usr/bin/env python3
"""Verify all statue meshes and retained RSP bounds against source geometry.

These are host geometry checks, not N64 rendering or performance measurements.
"""
import math
import re
import unittest
from test_physics_lab_rendering import ROOT, MODEL, ARRAYS, decode, lab


class StatueGeometry(unittest.TestCase):
    def test_render_mesh_matches_original_transforms(self):
        terrain=lab.build_terrain()
        # Independently reconstruct the four full collision templates, then
        # apply the original object transforms and compare every vertex/color.
        templates=[]
        for material,(cx,cz,*_) in enumerate(lab.SCULPTURES):
            faces=[f for f in terrain.faces if f.layer==f"sculpture_{material}"]
            templates.append([tuple((*[p[0]-cx,p[1],p[2]-cz],0,0,0,*lab.shaded_color(f.points,f.color),255)
                                    for p in f.points) for f in faces])
        placements=[(i,x,z,1) for i,(x,z,*_) in enumerate(lab.SCULPTURES)]
        for i,(x,z,*_) in enumerate(lab.SCULPTURES):
            placements.extend([(i,x-600,z+2100,.5),(i,x+650,z+2100,.25)])
        for index,(material,x,z,scale) in enumerate(placements):
            sign=1 if scale==1 else -1
            original=[tuple((x+v[0]*scale*sign,v[1]*scale,z+v[2]*scale*sign,*v[3:]) for v in tri)
                      for tri in templates[material]]
            baked=decode(f"physics_lab_sculpture_{index}")
            restored=[tuple((-8000+v[0]*.25,v[1]*.25,10500+v[2]*.25,*v[3:]) for v in tri) for tri in baked]
            self.assertEqual(len(restored),92)
            # The plain plinth replaces the original 18 tiled top triangles.
            # Every other surviving triangle retains exact world coordinates
            # and color from its collision-template counterpart.
            self.assertEqual(sum(all(v[1] == 40*scale for v in tri) for tri in restored), 2)
            for tri in restored[2:]:
                self.assertIn(tri,original[18:])
            self.assertEqual(len(set(original[18:])-set(restored[2:])),18)
        script=(ROOT/"levels/physics_lab/script.c").read_text()
        self.assertNotIn("bhvStaticObject",script)
        geo=(ROOT/"levels/physics_lab/geo.c").read_text()
        self.assertIn("GEO_SCALE(LAYER_OPAQUE, 0x4000)",geo)
        self.assertIn("GEO_TRANSLATE_NODE(LAYER_OPAQUE, LAB_SCULPTURE_ORIGIN_X, 0, LAB_SCULPTURE_ORIGIN_Z)",geo)

    def test_removed_faces_are_inside_other_closed_pieces(self):
        # Independent solid-angle containment, using complete source pieces,
        # rather than repeating the exporter's half-space elimination.
        def dot(a,b): return sum(x*y for x,y in zip(a,b))
        def inside(point, triangles):
            angle=0.0
            for tri in triangles:
                a,b,c=[tuple(v-p for v,p in zip(vertex,point)) for vertex in tri]
                la,lb,lc=[math.sqrt(dot(v,v)) for v in (a,b,c)]
                cross=(b[1]*c[2]-b[2]*c[1],b[2]*c[0]-b[0]*c[2],b[0]*c[1]-b[1]*c[0])
                angle+=2*math.atan2(dot(a,cross),la*lb*lc+dot(a,b)*lc+dot(b,c)*la+dot(c,a)*lb)
            return abs(angle)>2*math.pi
        for far,expected in ((False,18),(True,8)):
            parts = []
            source = lab.build_sculpture(far, parts)
            pieces=[]
            for first,end,base in parts:
                faces=source.faces[first:end]
                tris=[(f.points[0],f.points[i],f.points[i+1]) for f in faces for i in range(1,len(f.points)-1)]
                if base is not None:
                    pts=[p for tri in tris for p in tri]
                    x0,x1=min(p[0] for p in pts),max(p[0] for p in pts)
                    z0,z1=min(p[2] for p in pts),max(p[2] for p in pts)
                    tris.extend([((x0,base,z0),(x1,base,z0),(x1,base,z1)),
                                 ((x0,base,z0),(x1,base,z1),(x0,base,z1))])
                pieces.append(tris)
            full=[t for _,t in source.triangles()]
            kept={t for _,t in lab.sculpture_render_triangles(far)}
            removed=[t for t in full[18:] if t not in kept]
            self.assertEqual(len(removed),expected)
            for tri in removed:
                n = lab.normal(tri)
                length = math.sqrt(dot(n, n))
                for u,v in ((.02,.02),(.96,.02),(.02,.96),(.33,.33),(.49,.49),(.49,.02),(.02,.49)):
                    # Just outside the removed face must still lie inside
                    # another solid, including both sides of joined ramps.
                    point=tuple(tri[0][a]*(1-u-v)+tri[1][a]*u+tri[2][a]*v+n[a]/length*.001 for a in range(3))
                    self.assertTrue(any(inside(point,p) for p in pieces if tri not in p),(far,tri,point))

    def test_precomputed_world_bounds_and_packed_lists(self):
        rows=MODEL.split("physics_lab_sculptures[] = {",1)[1].split("};",1)[0].strip().splitlines()
        self.assertEqual(len(rows),12)
        for index,row in enumerate(rows):
            numbers=list(map(int,re.findall(r"-?\d+",row)))
            lo,hi=numbers[:3],numbers[3:6]
            for suffix in ("","_distant"):
                name=f"physics_lab_sculpture_{index}{suffix}"
                body=ARRAYS[name]
                vertices=sum(map(int,re.findall(r"gsSPVertex\([^,]+, (\d+), 0\)",body)))
                self.assertLessEqual(vertices,280 if not suffix else 160)
                self.assertNotIn("gsSPDisplayList",body)
                for tri in decode(name):
                    for v in tri:
                        world=(-8000+v[0]*.25,v[1]*.25,10500+v[2]*.25)
                        self.assertTrue(all(lo[a]<world[a]<hi[a] for a in range(3)))


if __name__ == "__main__":
    unittest.main()
