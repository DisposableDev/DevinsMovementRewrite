#!/usr/bin/env python3
"""Decode shipped display lists and exercise production LOD and distance cutoffs.

Checks ordered vertex attributes, bounds, state restoration and conservative
fallbacks. No RSP rasterization or emulator performance is simulated.
"""
import importlib.util
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import unittest

from regression_support import Source

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("lab_generator", ROOT / "tools/generate_physics_lab.py")
lab = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = lab
spec.loader.exec_module(lab)
MODEL = read_source(ROOT / "levels/physics_lab/model.inc.c")
ARRAYS = dict(re.findall(r"(?:static )?const (?:Vtx|Gfx) (\w+)\[\] = \{(.*?)\n\};", MODEL, re.S))


def decode(name, culled=False, seen=None, skip_labels=False):
    """Interpret actual generated commands, including the cull's early return."""
    vertices = {}
    triangles = []
    state = [] if seen is None else seen
    for op, args in re.findall(r"(gs\w+|PHYSICS_LAB_SET_CLIP_RATIO)\(([^)]*)\)", ARRAYS[name]):
        if op == "PHYSICS_LAB_SET_CLIP_RATIO":
            op = "gsSPClipRatio"
        fields = [a.strip() for a in args.split(",")]
        if op == "gsSPVertex":
            match = re.fullmatch(r"(\w+)(?: \+ (\d+))?", fields[0])
            array, offset = match[1], int(match[2] or 0)
            data = [tuple(map(int, re.findall(r"-?\d+", line))) for line in ARRAYS[array].splitlines() if "{{{" in line]
            count, start = map(int, fields[1:])
            if count > 24 or start<0 or start+count>24 or offset + count > len(data):
                raise AssertionError((name, count, offset))
            vertices.update(enumerate(data[offset:offset + count],start))
        elif op == "gsSP1Triangle":
            triangles.append(tuple(vertices[int(i)] for i in fields[:3]))
        elif op == "gsSP2Triangles":
            triangles.extend(tuple(vertices[int(i)] for i in group) for group in (fields[:3], fields[4:7]))
        elif op == "gsSPDisplayList":
            if not (skip_labels and fields[0]=="physics_lab_labels_dl"):
                triangles.extend(decode(fields[0], culled, state, skip_labels))
        elif op == "gsSPCullDisplayList":
            if culled:
                break
        elif op == "gsSPEndDisplayList":
            break
        else:
            state.append((op, args))
    return triangles


def expected(faces, water=False):
    triangles = []
    for face in faces:
        rgba = (*lab.shaded_color(face.points, face.color), 110 if water else 255)
        vertices = [(*p, 0, *(lab.checker_uv(p) if face.layer == "checker" else (0, 0)), *rgba) for p in face.points]
        triangles.append(tuple(vertices[:3]))
        if len(vertices) == 4:
            triangles.append(tuple(vertices[i] for i in (0, 2, 3)))
    return triangles


class PhysicsLabRendering(unittest.TestCase):
    def test_landing_caps_keep_only_exposed_walls_at_slope_joins(self):
        def fixture(mesh):
            mesh.box(0,500,0,1000,100,200,"wall",bottom=True)
            mesh.slab(500,1000,100,500,100,400,100,"wall",axis="x")

        for rotated in (False, True):
            mesh = lab.Mesh()
            lab.place(mesh, fixture, 1000, 2000, rotated)
            collision = lab.collision_c(mesh, "landing")
            exposed = lab.exposed_terrain_faces(mesh)
            axis, sign, coordinate = (2,-1,1500) if rotated else (0,1,1500)
            walls = []
            for face in exposed:
                rect = lab.flat_rectangle(face)
                if rect and rect[:2] == (axis,sign) and rect[2][axis] == coordinate:
                    walls.append(face)
            self.assertEqual(len(walls), 2)
            area = 0
            for face in walls:
                _,_,lo,hi = lab.flat_rectangle(face)
                area += (hi[1]-lo[1]) * (hi[0 if rotated else 2]-lo[0 if rotated else 2])
                self.assertEqual(face.color, lab.COLORS["wall"])
                self.assertEqual(face.surface, lab.DEFAULT)
                self.assertEqual(lab.shaded_color(face.points,face.color),
                                 tuple(round(c*.72) for c in face.color))
            self.assertEqual(area, 60000)
            self.assertEqual(lab.collision_c(mesh, "landing"), collision)

    def test_exact_sloped_join_removes_only_sealed_opposing_ends(self):
        for kind in ("sealed", "open", "vanish", "lod", "different_profile"):
            mesh = lab.Mesh()
            if kind != "open":
                mesh.floor(0,1000,0,800)
                for face in mesh.faces:
                    face.checker_color = face.color
            surface = "SURFACE_VANISH_CAP_WALLS" if kind == "vanish" else lab.DEFAULT
            mesh.ramp(0,500,0,800,0,250,"grip",surface)
            mesh.ramp(500,1000,0,800,0,251 if kind == "different_profile" else 250,"grip",surface)
            joins = [face for face in mesh.faces if all(p[0] == 500 for p in face.points)]
            self.assertEqual(len(joins),2)
            if kind == "lod":
                joins[0].lod_group = "independent"
            collision = lab.collision_c(mesh,"sloped_join")
            exposed = lab.exposed_terrain_faces(mesh)
            remaining = [face for face in exposed if all(p[0] == 500 for p in face.points)]
            self.assertEqual(len(remaining),0 if kind == "sealed" else 2)
            tops = [face for face in mesh.faces if lab.normal(face.points)[1] > 0
                    and face.checker_color is None]
            self.assertTrue(all(any(face is top for face in exposed) for top in tops))
            self.assertEqual(lab.collision_c(mesh,"sloped_join"),collision)

    def test_buried_half_pipe_ends_require_sealed_above_ground_tops(self):
        for kind in ("sealed", "open", "below_ground", "vanish"):
            mesh = lab.Mesh()
            if kind != "open":
                mesh.floor(0,2000,0,600)
                for face in mesh.faces:
                    face.checker_color = face.color
            low = -10 if kind == "below_ground" else 0
            surface = "SURFACE_VANISH_CAP_WALLS" if kind == "vanish" else lab.DEFAULT
            mesh.ramp(0,2000,0,300,30,low,"very_slippery",surface,base=-20)
            mesh.ramp(0,2000,300,600,low,30,"very_slippery",surface,base=-20)
            collision = lab.collision_c(mesh,"half_pipe")
            exposed = lab.exposed_terrain_faces(mesh)
            ends = [face for face in exposed if all(p[2] == 300 for p in face.points)]
            self.assertEqual(len(ends),0 if kind == "sealed" else 2)
            tops = [face for face in mesh.faces if lab.normal(face.points)[1] > 0
                    and face.checker_color is None]
            self.assertTrue(all(any(face is top for face in exposed) for top in tops))
            self.assertEqual(lab.collision_c(mesh,"half_pipe"),collision)

    def test_open_wedges_and_vanish_slabs_cannot_hide_landing_caps(self):
        for kind in ("open", "sealed", "vanish", "lod"):
            mesh = lab.Mesh()
            if kind == "sealed":
                mesh.floor(500,1000,100,500,tile=1000)
                for face in mesh.faces:
                    face.checker_color = lab.COLORS["ground"]
            mesh.box(0,500,0,1000,0,500,"wall",bottom=True)
            if kind in ("open", "sealed"):
                mesh.ramp(500,1000,100,500,500,0,"wall",axis="x")
            else:
                surface = "SURFACE_VANISH_CAP_WALLS" if kind == "vanish" else lab.DEFAULT
                first = len(mesh.faces)
                mesh.slab(500,1000,100,500,0,300,500,"wall",axis="x",surface=surface)
                if kind == "lod":
                    for face in mesh.faces[first:]:
                        face.lod_group = "separate_detail"
            area = 0
            for face in lab.exposed_terrain_faces(mesh):
                rect = lab.flat_rectangle(face)
                if rect and rect[:2] == (0,1) and rect[2][0] == 500:
                    _,_,lo,hi = rect
                    area += (hi[1]-lo[1]) * (hi[2]-lo[2])
            self.assertEqual(area, 300000 if kind == "sealed" else 500000, kind)

    def test_forecourt_directory_is_flat_and_fits_its_panels(self):
        mesh = lab.Mesh()
        mesh.floor(3150,6350,-650,500,tile=1000,aligned=True)
        for face in mesh.faces:
            face.checker_color=lab.COLORS["ground"]
        collision=lab.collision_c(mesh,"directory")
        lab.paint_forecourt_directory(mesh)
        self.assertEqual(lab.collision_c(mesh,"directory"),collision)
        letters = [face for face in mesh.faces if face.color == lab.COLORS["label"]]
        self.assertTrue(letters)
        panels=[(3200,4600,-550,-200),(4850,6250,-550,-200),
                (3200,4600,0,350),(4850,6250,0,350)]
        for face in letters:
            n = lab.normal(face.points)
            self.assertGreater(n[1],0)
            self.assertEqual((n[0],n[2]),(0,0))
            self.assertIsNone(face.surface)
            self.assertTrue(all(y==0 for x,y,z in face.points))
            self.assertTrue(any(all(x0<x<x1 and z0<z<z1 for x,y,z in face.points)
                                for x0,x1,z0,z1 in panels))
        for x0,x1,z0,z1 in panels:
            points=[p for face in letters for p in face.points
                    if x0<p[0]<x1 and z0<p[2]<z1]
            self.assertEqual(max(p[2] for p in points)-min(p[2] for p in points),150)
        # Ninety-unit text/arrow gaps are plain backing, clear of glyphs.
        for x,z in [(4171,-374),(5991,-374),(4351,176),(5761,176)]:
            colors=[face.color for face in mesh.faces if face.layer=="opaque"
                    and (rect:=lab.flat_rectangle(face)) is not None
                    and rect[2][0]<x<rect[3][0] and rect[2][2]<z<rect[3][2]]
            self.assertEqual(colors,[lab.COLORS["runway"]])

    def test_low_access_ramps_and_platform_label_clearance(self):
        mesh=lab.build_terrain()
        ground=lab.ground_patches(mesh)
        ramps=[(13400,13800,2200,2800),(13800,15000,10000,10800)]
        for x0,x1,z0,z1 in ramps:
            for face in ground:
                _,_,lo,hi=lab.flat_rectangle(face)
                self.assertFalse(lo[0]<x1 and hi[0]>x0 and lo[2]<z1 and hi[2]>z0)
        # Removing render coverage retains the original ground and ramp collision.
        sample=lab.Mesh()
        sample.floor(13000,15000,2000,3000,tile=1000,aligned=True)
        for face in sample.faces:
            face.checker_color=lab.COLORS["ground"]
        sample.ramp(13400,13800,2200,2800,0,20,"normal",axis="x")
        collision=lab.collision_c(sample,"ramp")
        lab.paint_floor(sample,ramps[0],0,None)
        self.assertEqual(lab.collision_c(sample,"ramp"),collision)
        pixels=lab.label_pixels("PLATFORMS")
        label=next(bounds for bounds,height,rows,_,_,_ in mesh.label_textures
                   if rows==pixels and height==0)
        x0,x1,z0,z1=label
        # No raised terrain projects across any of the lettering.
        for face in mesh.faces:
            if face.layer!="opaque" or max(p[1] for p in face.points)<=0:
                continue
            lo,hi=lab.face_bounds([face])
            self.assertFalse(lo[0]<x1 and hi[0]>x0 and lo[2]<z1 and hi[2]>z0,
                             face.points)

    def test_covered_ground_preserves_collision_and_exposed_floor(self):
        mesh=lab.Mesh()
        mesh.floor(-2000,2000,-2000,2000,tile=1000,aligned=True)
        for face in mesh.faces:
            face.checker_color=lab.COLORS["ground"]
        collision=lab.collision_c(mesh,"covered")
        mesh.covered_ground.append((-1200,800,-1500,1000))
        patches=lab.ground_patches(mesh)
        rectangles=[]
        for face in patches:
            _,_,lo,hi=lab.flat_rectangle(face)
            rectangles.append((lo[0],hi[0],lo[2],hi[2]))
            self.assertFalse(lo[0]<800 and hi[0]>-1200 and
                             lo[2]<1000 and hi[2]>-1500)
        self.assertEqual(sum((b-a)*(d-c) for a,b,c,d in rectangles),11000000)
        self.assertEqual(lab.collision_c(mesh,"covered"),collision)

    def test_submerged_boxes_need_a_complete_opaque_sealing_floor(self):
        def sealed(width=200, surface=lab.DEFAULT):
            mesh=lab.Mesh()
            mesh.floor(0,width,0,100,-2000,"pool",surface)
            mesh.box(0,100,0,100,-2000,-250,"pool")
            mesh.box(100,200,0,100,-2000,-250,"pool")
            return mesh
        self.assertEqual(len(lab.closed_terrain_boxes(sealed())),2)
        self.assertEqual(len(lab.closed_terrain_boxes(sealed(199))),1)
        self.assertEqual(lab.closed_terrain_boxes(sealed(surface="SURFACE_VANISH_CAP_WALLS")),[])
        mesh=sealed()
        collision=lab.collision_c(mesh,"sealed")
        faces=lab.exposed_terrain_faces(mesh)
        self.assertFalse(any(all(p[0]==100 for p in face.points) for face in faces))
        self.assertEqual(lab.collision_c(mesh,"sealed"),collision)

    def test_pool_bank_wall_and_floor_share_the_opening_vertex(self):
        mesh=lab.Mesh()
        mesh.face(((5200,0,1000),(5200,-400,2600),
                   (6400,-400,2600),(6400,0,1000)),lab.COLORS["pool"])
        mesh.face(((5200,-400,2600),(5200,-1200,4000),
                   (6400,-1200,4000),(6400,-400,2600)),lab.COLORS["pool"])
        mesh.face(((6400,-240,2700),(6400,-440,2800),
                   (6800,-440,2800),(6800,-240,2700)),lab.COLORS["pool"])
        mesh.box(6400,6640,1000,3100,-2000,0,"pool")
        collision=lab.collision_c(mesh,"bank")
        faces=lab.static_terrain_faces(mesh)
        floor=[face for face in faces if lab.normal(face.points)[1]>0
               and min(p[1] for p in face.points)<0]
        wall=[face for face in faces if all(p[0]==6400 for p in face.points)
              and max(p[1] for p in face.points)==0]
        join=(6400,-686,3100)
        self.assertTrue(any(join in face.points for face in floor))
        self.assertTrue(any(join in face.points for face in wall))
        self.assertTrue(all(min(p[1] for p in face.points)>=-686 for face in wall))
        self.assertTrue(any((6640,-440,2800) in face.points for face in floor))
        self.assertTrue(all(lab.normal(face.points)!=(0,0,0) for face in faces))
        self.assertEqual(lab.collision_c(mesh,"bank"),collision)

    def test_slide_circuit_joins_preserve_riding_edges_and_collision(self):
        mesh=lab.Mesh()
        lab.add_slide_circuit(mesh)
        collision=lab.collision_c(mesh,"circuit")
        visible=lab.exposed_slide_circuit_joins(mesh.faces)
        removed=[face for face in mesh.faces if face not in visible]
        self.assertEqual(len(removed),4)
        for wall in removed:
            edge={p for p in wall.points if p[1]>0}
            self.assertEqual(len(edge),2)
            # The nearby turn and its distant replacement share the exact top edge.
            turn=next(face.lod_group for face in mesh.faces
                      if face.lod_group and lab.normal(face.points)[1]>0
                      and edge.issubset(face.points))
            self.assertTrue(any(lab.normal(face.points)[1]>0 and edge.issubset(face.points)
                                for face in mesh.lods[turn]))
            self.assertTrue(any(face.lod_group is None and lab.normal(face.points)[1]>0
                                and edge.issubset(face.points) for face in visible))
        # Exposed ends at both sides of the jump gap remain visible.
        for x in (-6200,-5800):
            self.assertTrue(any(all(p[0]==x for p in face.points)
                                and lab.normal(face.points)[1]==0 for face in visible))
        self.assertEqual(lab.collision_c(mesh,"circuit"),collision)

    def test_runway_and_boundary_joins_keep_exposed_walls(self):
        mesh=lab.Mesh()
        lab.add_blj_activation(mesh)
        for bounds in ((-15300,-15200,-15200,15200),
                       (15200,15300,-15200,15200),
                       (-15300,15300,-15300,-15200),
                       (-15300,15300,15200,15300)):
            mesh.box(*bounds,-300,120,"normal")
        collision=lab.collision_c(mesh,"joins")
        visible=lab.exposed_runway_and_boundary_joins(mesh.faces)
        # Exit-ramp cap disappears; the side ramp trims only its own 600-unit span.
        self.assertFalse(any(all(p[2]==10000 for p in face.points)
                             and min(p[0] for p in face.points)==13800
                             and max(p[1] for p in face.points)==20 for face in visible))
        self.assertTrue(any(all(p[0]==13800 for p in face.points)
                            and min(p[2] for p in face.points)==2800
                            and max(p[2] for p in face.points)==10000 for face in visible))
        # Outer faces remain complete; inner faces stop at the adjoining side walls.
        for z in (-15300,15300):
            self.assertTrue(any(all(p[2]==z for p in face.points)
                                and min(p[0] for p in face.points)==-15300
                                and max(p[0] for p in face.points)==15300 for face in visible))
        for z in (-15200,15200):
            walls=[face for face in visible if all(p[2]==z for p in face.points)
                   and lab.normal(face.points)[1]==0]
            self.assertTrue(walls)
            self.assertTrue(all(min(p[0] for p in face.points)>=-15200
                                and max(p[0] for p in face.points)<=15200 for face in walls))
        self.assertEqual(lab.collision_c(mesh,"joins"),collision)

    def test_cannon_recess_has_distinct_flat_wall_tones(self):
        mesh=lab.Mesh()
        lab.add_cannon_recess(mesh)
        walls=mesh.faces[1:4]
        self.assertEqual(len({lab.shaded_color(face.points,face.color) for face in walls}),3)
        for face in walls:
            colors={vertex[-4:] for tri in expected([face]) for vertex in tri}
            self.assertEqual(len(colors),1)
        ramp=next(face for face in mesh.faces if face.surface=="SURFACE_NOT_SLIPPERY")
        self.assertEqual(set(ramp.points),{(-6600,0,-300),(-6200,0,-300),
                                          (-6600,-350,100),(-6200,-350,100)})

    def test_flight_tunnel_roof_joins_remove_only_buried_caps(self):
        mesh=lab.Mesh()
        lab.add_flight_course(mesh)
        collision=lab.collision_c(mesh,"flight")
        before=lab.exposed_pool_banks(lab.merge_clear_terrain_faces(
            lab.merge_terrain_faces(lab.exposed_terrain_faces(mesh))))
        faces=lab.static_terrain_faces(mesh)
        # Every exposed roof and underside remains byte-identical.
        self.assertEqual([f for f in before if lab.normal(f.points)[1]!=0],
                         [f for f in faces if lab.normal(f.points)[1]!=0])
        for z in (-10500,-9100):
            joined=[f for f in faces if all(p[2]==z for p in f.points)
                    and {p[0] for p in f.points}=={4500,7500}]
            self.assertTrue(joined)
            self.assertTrue(all(y<=3300-x/5 for f in joined for x,y,_ in f.points))
            self.assertTrue(any({(4500,2400,z),(7500,1800,z)}<=set(f.points)
                                for f in joined))
        changed=[f for f in before if f not in faces]
        self.assertEqual(len(changed),4)
        # Without the matching solid strip, the roof edge is exposed.
        slab=lab.Mesh()
        slab.slab(4500,7500,-10500,-9100,2400,1800,color="tower",axis="x")
        self.assertEqual(lab.exposed_flight_tunnel_roof_joins(slab.faces),slab.faces)
        self.assertEqual(lab.collision_c(mesh,"flight"),collision)

    def test_solid_direction_panel_preserves_collision_and_disjoint_floor(self):
        mesh=lab.Mesh()
        mesh.floor(0,3000,0,1000,tile=1000,aligned=True)
        for face in mesh.faces:
            face.checker_color=lab.COLORS["ground"]
        collision=lab.collision_c(mesh,"panel")
        lab.paint_direction_panel(mesh,"PLATFORMS","E",(700,2300,200,650),30)
        self.assertEqual(lab.collision_c(mesh,"panel"),collision)
        ground=lab.ground_patches(mesh)
        rendered=ground+lab.static_terrain_faces(mesh)
        rectangles=[lab.flat_rectangle(face)[2:] for face in rendered]
        self.assertEqual(sum((hi[0]-lo[0])*(hi[2]-lo[2]) for lo,hi in rectangles),3000000)
        for i,(lo,hi) in enumerate(rectangles):
            for other_lo,other_hi in rectangles[i+1:]:
                self.assertTrue(hi[0]<=other_lo[0] or other_hi[0]<=lo[0] or
                                hi[2]<=other_lo[2] or other_hi[2]<=lo[2])
        for face in ground:
            _,_,lo,hi=lab.flat_rectangle(face)
            self.assertTrue(hi[0]<=700 or lo[0]>=2300 or hi[2]<=200 or lo[2]>=650)

    def test_shipped_triangle_order_and_attributes(self):
        terrain = lab.build_terrain()
        meshes = [("terrain", lab.without_textured_labels(terrain,lab.ground_patches(terrain))+lab.without_textured_labels(terrain,lab.static_terrain_faces(terrain)), False),
                  ("ground", lab.without_textured_labels(terrain,lab.ground_patches(terrain)), False),
                  ("water", [f for f in terrain.faces if f.layer == "water"], True),
                  ("pole_short", lab.build_pole(1200).faces, False),
                  ("pole_tall", lab.build_pole(2000).faces, False),
                  ("pole_impact", lab.build_pole(3800).faces, False),
                  ("pole_launch", lab.build_pole(3200).faces, False),
                  ("tilter", lab.build_tilter().faces, False)]
        meshes.extend((name, mesh.faces, False) for name, mesh in lab.build_movers().items())
        for name, faces, water in meshes:
            with self.subTest(mesh=name):
                self.assertEqual(decode(f"physics_lab_{name}_dl",skip_labels=True), expected(faces, water))

    def test_textured_labels_preserve_authored_coverage_and_palette(self):
        mesh=lab.build_terrain()
        original=lab.ground_patches(mesh)+lab.static_terrain_faces(mesh)
        textures=dict(re.findall(r"ALIGNED8 static const u8 (physics_lab_label_\w+_texture)\[\] = \{(.*?)\n\};",MODEL,re.S))
        referenced_textures = set(re.findall(r"gsDPLoadTextureBlock\((physics_lab_label_\w+_texture),", MODEL))
        self.assertEqual(set(textures), referenced_textures)
        self.assertEqual(len(mesh.label_textures),20)
        for index,(bounds,height,pixels,background,cell,checker) in enumerate(mesh.label_textures):
            x0,x1,z0,z1=bounds
            old=[]
            for face in original:
                rect=lab.flat_rectangle(face)
                if rect and rect[:2]==(1,1) and rect[2][1]==height:
                    lo,hi=rect[2:]
                    l,r,t,u=max(lo[0],x0),min(hi[0],x1),max(lo[2],z0),min(hi[2],z1)
                    if l<r and t<u:
                        old.append(((l,r,t,u),face.checker_color or face.color))
            self.assertEqual(sum((r-l)*(u-t) for (l,r,t,u),_ in old),(x1-x0)*(z1-z0))
            cells=[((x0+c*cell,x0+(c+1)*cell,z0+r*cell,z0+(r+1)*cell),
                    lab.COLORS["label"] if pixel=="1" else background)
                   for r,row in enumerate(pixels) for c,pixel in enumerate(row)]
            for rect,color in old:
                self.assertTrue(lab.rectangle_covered(rect,[r for r,c in cells if c==color]))
            for rect,color in cells:
                self.assertTrue(lab.rectangle_covered(rect,[r for r,c in old if c==color]))
            for intensity in ((235,255) if checker else (None,)):
                name=f"physics_lab_label_{index}"+(f"_{intensity}" if checker else "")+"_texture"
                if name not in referenced_textures:
                    continue
                data=list(map(int,re.findall(r"\d+",textures[name])))
                width=len(data)//(8*4)
                self.assertLessEqual(len(data),4096)
                self.assertEqual(width%8,0)
                for row in range(8):
                    for column in range(width):
                        rgb=lab.COLORS["label"] if row<5 and column<len(pixels[0]) and pixels[row][column]=="1" else background
                        if intensity is not None:
                            rgb=tuple((c*intensity+128)//256 for c in rgb)
                        offset=(row*width+column)*4
                        self.assertEqual(tuple(data[offset:offset+4]),(*rgb,255))
        for child in re.findall(r"gsSPDisplayList\((\w+)\)",ARRAYS["physics_lab_labels_dl"]):
            body=ARRAYS[child]
            self.assertLess(body.index("gsSPCullDisplayList(0, 3)"),body.index("gsDPLoadTextureBlock"))
            self.assertEqual(decode(child,culled=True),[])
            triangles=decode(child)
            self.assertEqual(len(triangles),2)
            self.assertTrue(all(lab.normal(tuple(v[:3] for v in tri))[1]>0 for tri in triangles))
        for culled in (False,True):
            state=[]
            decode("physics_lab_labels_dl",culled=culled,seen=state)
            self.assertEqual(state[-3:],[("gsSPTexture","0, 0, 0, G_TX_RENDERTILE, G_OFF"),
                                        ("gsDPSetTextureFilter","G_TF_BILERP"),
                                        ("gsDPSetCombineMode","G_CC_SHADE, G_CC_SHADE")])

    def test_north_commands_omit_only_backfaces_and_share_vertices(self):
        groups=[name for name in ARRAYS if re.fullmatch(r"physics_lab_terrain_section_\d+(?:_distant)?_group_\d+",name)]
        limit=float(re.search(r"physics_lab_north_view_z = (-?\d+)\.0f",MODEL)[1])
        removed=0
        for name in groups:
            original=decode(name)
            intended=[]
            for tri in original:
                points=tuple(v[:3] for v in tri)
                n=lab.normal(points)
                if n[0]==0 and n[1]==0 and n[2]>0 and points[0][2]>=limit and len({p[2] for p in points})==1:
                    removed+=1
                else:
                    intended.append(tri)
            self.assertEqual(decode(name+"_north"),intended)
            self.assertEqual(decode(name+"_north",culled=True),[])
            self.assertEqual(re.findall(r"gsSPVertex\((\w+)_bounds, 8, 0\)",ARRAYS[name]),
                             re.findall(r"gsSPVertex\((\w+)_bounds, 8, 0\)",ARRAYS[name+"_north"]))
            for array in re.findall(r"gsSPVertex\((\w+)(?: \+ \d+)?",ARRAYS[name+"_north"]):
                self.assertIn(array,[name+"_bounds",name+"_vertices"])
        # Removing hidden wall caps reduces this count. The exact omissions
        # and shared vertex streams above remain the preservation criterion.
        self.assertGreater(removed,0)
        self.assertFalse(any(re.fullmatch(r"physics_lab_terrain_.*_north_vertices",n) for n in ARRAYS))
        rows=MODEL.split("physics_lab_terrain_north_batches[] = {",1)[1].split("};",1)[0]
        north=re.findall(r"\{ (\w+), (\w+), (\d+) \}",rows)
        rows=MODEL.split("physics_lab_terrain_batches[] = {",1)[1].split("};",1)[0]
        full=re.findall(r"\{ (\w+), (\w+), (\d+) \}",rows)
        self.assertEqual(north,[(a+"_north",b+"_north",m) for a,b,m in full])
        limit=float(re.search(r"physics_lab_north_view_z = (-?\d+)\.0f",MODEL)[1])
        self.assertEqual(limit,min(tri[0][2] for name in groups for tri in decode(name)
                                   if (n:=lab.normal(tuple(v[:3] for v in tri)))[0]==n[1]==0
                                   and n[2]>0 and tri[0][2]>=lab.NORTH_BACKFACE_PLANE_Z))

    def test_aerial_commands_omit_only_proven_backfaces(self):
        groups=[name for name in ARRAYS if re.fullmatch(r"physics_lab_terrain_section_\d+(?:_distant)?_group_\d+",name)]
        limit=float(re.search(r"physics_lab_north_view_z = (-?\d+)\.0f",MODEL)[1])
        width=float(re.search(r"physics_lab_aerial_view_half_width = (\d+)\.0f",MODEL)[1])
        ceiling=float(re.search(r"physics_lab_aerial_view_y = (\d+)\.0f",MODEL)[1])
        removed=0
        for name in groups:
            expected=[]
            for tri in decode(name):
                points = tuple((v[:3] for v in tri))
                n = lab.normal(points)
                north=n[0]==0 and n[1]==0 and n[2]>0 and points[0][2]>=limit
                side=n[1]==0 and n[2]==0 and ((n[0]>0 and points[0][0]>=width) or (n[0]<0 and points[0][0]<=-width))
                bottom=n[0]==0 and n[2]==0 and n[1]<0
                if north or side or bottom:
                    removed+=1
                else:
                    expected.append(tri)
            self.assertEqual(decode(name+"_aerial"),expected)
            self.assertEqual(decode(name+"_aerial",culled=True),[])
            for array in re.findall(r"gsSPVertex\((\w+)(?: \+ \d+)?",ARRAYS[name+"_aerial"]):
                self.assertIn(array,[name+"_bounds",name+"_vertices"])
        self.assertGreater(removed,1000)
        self.assertEqual(ceiling,max(v[1] for name in groups for tri in decode(name) for v in tri))
        rows=MODEL.split("physics_lab_terrain_aerial_batches[] = {",1)[1].split("};",1)[0]
        batches=re.findall(r"\{ (\w+), (\w+), (\d+) \}",rows)
        self.assertEqual(len(batches),6)
        for mask in range(8):
            actual=[tri for near,far,bits in batches for tri in decode(far if mask&int(bits) else near,skip_labels=True)]
            fullrows=MODEL.split("physics_lab_terrain_batches[] = {",1)[1].split("};",1)[0]
            full=re.findall(r"\{ (\w+), (\w+), (\d+) \}",fullrows)
            old=[tri for near,far,bits in full for tri in decode(far if mask&int(bits) else near,skip_labels=True)]
            expected=[]
            for tri in old:
                points = tuple((v[:3] for v in tri))
                n = lab.normal(points)
                if ((n[0]==0 and n[1]==0 and n[2]>0 and points[0][2]>=limit) or (n[0]==0 and n[2]==0 and n[1]<0)
                    or (n[1]==0 and n[2]==0 and ((n[0]>0 and points[0][0]>=width) or (n[0]<0 and points[0][0]<=-width)))):
                    continue
                expected.append(tri)
            self.assertEqual(actual,expected)

    def test_terrain_order_and_state(self):
        for prefix in ("ground", "terrain"):
            count = int(re.search(rf"physics_lab_{prefix}_section_count = (\d+)", MODEL)[1])
            calls = re.findall(rf"gsSPDisplayList\((physics_lab_{prefix}_section_\d+)\)", ARRAYS[f"physics_lab_{prefix}_dl"])
            self.assertEqual(calls, [f"physics_lab_{prefix}_section_{i}" for i in range(count)])
        state = []
        decode("physics_lab_terrain_dl", seen=state)
        label_draws=len(re.findall(r"gsSPDisplayList\(",ARRAYS["physics_lab_labels_dl"]))
        self.assertEqual(sum(op == "gsDPPipeSync" for op, _ in state), 6+label_draws)
        self.assertEqual(state[-2], ("gsSPSetGeometryMode", "G_LIGHTING | G_CULL_BACK"))

    def test_clipping_restores_the_baseline_after_culling(self):
        for prefix in ("ground", "terrain"):
            for culled in (False, True):
                state = []
                decode(f"physics_lab_{prefix}_dl", culled=culled, seen=state)
                clipping = [args for op, args in state if op == "gsSPClipRatio"]
                self.assertEqual(clipping[:2],
                                 ["FRUSTRATIO_1", "PHYSICS_LAB_BASE_CLIP_RATIO"])
                if prefix == "terrain":
                    self.assertEqual(clipping[2], "FRUSTRATIO_2")
                    self.assertEqual(clipping[-1], "PHYSICS_LAB_BASE_CLIP_RATIO")
                    overrides = clipping[3:-1]
                    self.assertEqual(overrides, ["FRUSTRATIO_1", "FRUSTRATIO_2"]
                                     * (len(overrides) // 2))
                else:
                    self.assertEqual(len(clipping), 2)

    def test_wide_floor_stream_preserves_geometry_and_complete_bounds(self):
        ordinary = decode("physics_lab_ground_dl")
        wide = decode("physics_lab_ground_wide_dl")
        self.assertEqual(wide, ordinary)
        bounds = re.search(r"physics_lab_ground_clip_bounds = \{ (.*?) \};", MODEL)[1]
        values = list(map(int, re.findall(r"-?\d+", bounds)))
        points = [vertex[:3] for triangle in ordinary for vertex in triangle]
        self.assertEqual(values[:3], [min(p[a] for p in points) for a in range(3)])
        self.assertEqual(values[3:], [max(p[a] for p in points) for a in range(3)])
        for culled in (False, True):
            state = []
            decode("physics_lab_ground_wide_dl", culled=culled, seen=state)
            self.assertEqual([args for op, args in state if op == "gsSPClipRatio"],
                             ["FRUSTRATIO_2", "PHYSICS_LAB_BASE_CLIP_RATIO"])

    def test_ground_vertices_use_normal_clipping_before_transform(self):
        uploads = []

        def walk(name, clipping):
            for op, args in re.findall(r"(gs\w+|PHYSICS_LAB_SET_CLIP_RATIO)\(([^)]*)\)",
                                       ARRAYS[name]):
                if op in ("gsSPClipRatio", "PHYSICS_LAB_SET_CLIP_RATIO"):
                    clipping = args
                elif op == "gsSPVertex":
                    self.assertEqual(clipping, "FRUSTRATIO_1", (name, args))
                    uploads.append(args)
                elif op == "gsSPDisplayList":
                    clipping = walk(args, clipping)
            return clipping

        # Start with the wider state used by the previous material. Every
        # ground upload must override it before transforming a checker patch.
        final = walk("physics_lab_ground_dl", "FRUSTRATIO_2")
        self.assertTrue(uploads)
        self.assertEqual(final, "PHYSICS_LAB_BASE_CLIP_RATIO")

    def test_long_wall_groups_clip_before_vertices_and_restore_after_culling(self):
        oversized = set()
        for name, body in ARRAYS.items():
            if not re.fullmatch(r"physics_lab_terrain_section_\d+(?:_distant)?(?:_north|_aerial)?", name):
                continue
            clipping = "FRUSTRATIO_2"
            for op, args in re.findall(r"(gs\w+)\(([^)]*)\)", body):
                if op == "gsSPClipRatio":
                    clipping = args
                elif op == "gsSPDisplayList" and args != "physics_lab_labels_dl":
                    # Bounds are padded by eight world units on each side.
                    # Inspect the actual uploaded bounds, independently of the
                    # generator's decision about which groups need protection.
                    bounds_name = re.search(r"gsSPVertex\((\w+), 8, 0\)", ARRAYS[args])[1]
                    points = [tuple(map(int, re.findall(r"-?\d+", row)))[:3]
                              for row in ARRAYS[bounds_name].splitlines() if "{{{" in row]
                    long = any(max(p[a] for p in points) - min(p[a] for p in points) > 8192 + 16
                               for a in (0, 2))
                    self.assertEqual(clipping, "FRUSTRATIO_1" if long else "FRUSTRATIO_2",
                                     (name, args))
                    if long:
                        oversized.add(args)
                    # An early-culling child leaves the same parent state as
                    # a drawn child; the next parent command must restore it.
                    self.assertFalse(any(op == "gsSPClipRatio"
                                         for op, _ in re.findall(r"(gs\w+)\(([^)]*)\)", ARRAYS[args])))
            self.assertEqual(clipping, "FRUSTRATIO_2", name)
        self.assertGreaterEqual(len(oversized), 8)

    def test_sculpture_bounds_and_culled_state_restoration(self):
        # One wrapper restores material even when every child culls itself.
        visible_state, hidden_state = [], []
        decode("physics_lab_sculptures_dl", seen=visible_state)
        self.assertEqual(decode("physics_lab_sculptures_dl", culled=True, seen=hidden_state), [])
        self.assertEqual(hidden_state, visible_state)
        self.assertEqual(len(visible_state), 9)
        for i in range(12):
            name = f"physics_lab_sculpture_{i}"
            points = [tuple(map(int, re.findall(r"-?\d+", line)))[:3]
                      for line in ARRAYS[name+"_bounds"].splitlines() if "{{{" in line]
            self.assertEqual(len(points), 8)
            for tri in decode(name):
                for v in tri:
                    self.assertTrue(all(min(p[a] for p in points)<v[a]<max(p[a] for p in points) for a in range(3)))

    def test_vertex_work_reduced_with_identical_triangles(self):
        def loads(pattern):
            return sum(int(n) for name, body in ARRAYS.items() if re.fullmatch(pattern, name)
                       for n in re.findall(r"gsSPVertex\(\w+_vertices \+ \d+, (\d+), 0\)", body))
        # Compare the expanded lab with its own unoptimized mesh. The previous
        # map's fixed budget predates the added bays, markings and drop decks.
        raw_vertices=sum(len(f.points) for f in lab.build_terrain().faces if f.layer=="opaque")
        self.assertLess(loads(r"physics_lab_(?:ground|terrain)_section_\d+"), raw_vertices * .6)
        sculpture = loads(r"physics_lab_sculpture_0")
        self.assertGreater(sculpture, 0)
        self.assertLess(sculpture, 280)
        self.assertEqual(len(decode("physics_lab_sculpture_0")), 92)

    def test_merges_preserve_coverage_color_winding_and_order(self):
        original = lab.exposed_terrain_faces(lab.build_terrain())
        simplified = lab.merge_terrain_faces(original)
        cursor, joined = 0, 0
        for face in simplified:
            if face == original[cursor]:
                cursor += 1
                continue
            # Independently prove each replacement is an exact tiling by the
            # next source rectangles: same plane/style, no gaps or overlaps.
            lo = [min(p[a] for p in face.points) for a in range(3)]
            hi = [max(p[a] for p in face.points) for a in range(3)]
            axes = [a for a in range(3) if lo[a] != hi[a]]
            self.assertEqual(len(axes), 2)
            a,b = axes
            target_area = (hi[a]-lo[a])*(hi[b]-lo[b])
            covered, pieces = 0, []
            while covered < target_area:
                old = original[cursor]
                self.assertEqual((old.color,old.layer,old.surface,old.force),
                                 (face.color,face.layer,face.surface,face.force))
                self.assertEqual(len(old.points), 4)
                self.assertGreater(sum(x*y for x,y in zip(lab.normal(old.points),lab.normal(face.points))), 0)
                ol = [min(p[c] for p in old.points) for c in range(3)]
                oh = [max(p[c] for p in old.points) for c in range(3)]
                self.assertTrue(all(lo[c] <= ol[c] <= oh[c] <= hi[c] for c in range(3)))
                self.assertEqual(set(old.points), {tuple(ol[c] if c not in axes else (x if c==a else y)
                                                        for c in range(3))
                                                   for x in (ol[a],oh[a]) for y in (ol[b],oh[b])})
                for pl,ph in pieces:
                    self.assertTrue(oh[a]<=pl[a] or ph[a]<=ol[a] or oh[b]<=pl[b] or ph[b]<=ol[b])
                pieces.append((ol,oh))
                covered += (oh[a]-ol[a])*(oh[b]-ol[b])
                cursor += 1
            self.assertEqual(covered, target_area)
            self.assertGreater(len(pieces), 1)
            joined += len(pieces)-1
        self.assertEqual(cursor, len(original))
        self.assertGreater(joined, len(original) * .15)
        print(f"Terrain merge: {joined} redundant rectangles removed; exact ordered coverage verified.")

    def test_floor_markings_preserve_collision_and_disjoint_render_coverage(self):
        mesh=lab.Mesh()
        mesh.box(0,200,0,200,0,200,"ice","SURFACE_ICE")
        collision=lab.collision_c(mesh,"sample")
        lab.paint_floor(mesh,(20,60,0,200),200,lab.COLORS["wall"])
        lab.paint_floor(mesh,(40,80,50,150),200,lab.COLORS["wall"])
        self.assertEqual(lab.collision_c(mesh,"sample"),collision)
        tops=[f for f in mesh.faces if f.layer=="opaque" and all(p[1]==200 for p in f.points)]
        rectangles=[lab.flat_rectangle(f)[2:] for f in tops]
        self.assertEqual(sum((hi[0]-lo[0])*(hi[2]-lo[2]) for lo,hi in rectangles),200*200)
        for i,(lo,hi) in enumerate(rectangles):
            self.assertTrue(all(0<=lo[a]<=hi[a]<=200 for a in (0,2)))
            for other_lo,other_hi in rectangles[i+1:]:
                self.assertTrue(hi[0]<=other_lo[0] or other_hi[0]<=lo[0] or
                                hi[2]<=other_lo[2] or other_hi[2]<=lo[2])
        self.assertTrue(any(f.color==lab.COLORS["wall"] for f in tops))
        self.assertTrue(any(f.color==lab.COLORS["ice"] for f in tops))

        # District labels cross checker boundaries and can overlap later paint.
        # They must retain the exact collision and disjoint textured coverage.
        mesh = lab.Mesh()
        mesh.floor(0,2000,0,2000,tile=1000,aligned=True)
        for face in mesh.faces:
            face.checker_color = lab.COLORS["ground"]
        collision = lab.collision_c(mesh,"sample")
        lab.paint_label(mesh,"WATER",100,925)
        lab.paint_label(mesh,"80",650,950,cell=25)
        self.assertEqual(lab.collision_c(mesh,"sample"),collision)
        rectangles = [lab.flat_rectangle(f)[2:] for f in lab.ground_patches(mesh)]
        self.assertEqual(sum((hi[0]-lo[0])*(hi[2]-lo[2]) for lo,hi in rectangles),2000*2000)
        for i,(lo,hi) in enumerate(rectangles):
            for other_lo,other_hi in rectangles[i+1:]:
                self.assertTrue(hi[0]<=other_lo[0] or other_hi[0]<=lo[0] or
                                hi[2]<=other_lo[2] or other_hi[2]<=lo[2])

    def test_practice_marks_preserve_collision_and_staging_widths(self):
        mesh=lab.Mesh()
        mesh.box(-6200,-5300,-3900,-3700,0,1292,"normal")
        mesh.floor(-6200,-5300,-1540,-1400)
        mesh.floor(-5300,-5100,-1540,-1400)
        mesh.box(-1600,1900,8600,9000,0,250,"normal")
        mesh.box(-1420,-980,11000,11400,0,250,"normal")
        collision=lab.collision_c(mesh,"practice")
        lab.paint_practice_marks(mesh)
        self.assertEqual(lab.collision_c(mesh,"practice"),collision)

        def color_at(x,y,z):
            hits=[]
            for face in mesh.faces:
                rectangle=lab.flat_rectangle(face) if face.layer=="opaque" else None
                if rectangle is not None and rectangle[0:2]==(1,1):
                    _,_,lo,hi=rectangle
                    if lo[1]==y and lo[0]<x<hi[0] and lo[2]<z<hi[2]:
                        hits.append(face.color)
            self.assertEqual(len(hits),1,(x,y,z))
            return hits[0]

        for point in [(-5740,1292,-3845),(-5740,0,-1475),(-5740,0,-1435),
                      (-5200,0,-1475),(-5200,0,-1435),
                      (-1200,250,8935),(-1200,250,11130),(-1200,250,11170)]:
            self.assertEqual(color_at(*point),lab.COLORS["label"])
        for point in [(-5740,1292,-3880),(-1250,250,8935),(-1200,250,11150)]:
            self.assertEqual(color_at(*point),lab.COLORS["normal"])
        for point in [(-5200,0,-1510),(-5200,0,-1455)]:
            self.assertEqual(color_at(*point),lab.COLORS["ground"])

    def test_merge_controls(self):
        def rectangle(x0,x1,z0,z1,color=(10,20,30),surface=lab.DEFAULT):
            return lab.Face(((x0,0,z0),(x0,0,z1),(x1,0,z1),(x1,0,z0)),color,surface)
        a,b=rectangle(0,10,0,10),rectangle(10,20,0,10)
        self.assertEqual(len(lab.merge_terrain_faces([a,b])), 1)
        for other in (rectangle(11,20,0,10), rectangle(9,20,0,10), rectangle(10,20,0,5),
                      rectangle(10,20,0,10,(11,20,30)), rectangle(10,20,0,10,surface="SURFACE_ICE"),
                      lab.Face(b.points[::-1],b.color,b.surface)):
            self.assertEqual(lab.merge_terrain_faces([a,other]), [a,other])
        self.assertEqual(len(lab.merge_terrain_faces([a,rectangle(10,7000,0,10)])), 1)
        intervening=rectangle(0,20,0,10,(99,99,99))
        self.assertEqual(lab.merge_terrain_faces([a,intervening,b]), [a,intervening,b])

    def test_ground_merges_preserve_coverage_and_collision(self):
        mesh = lab.build_terrain()
        collision = lab.collision_c(mesh, "coverage_test")
        original = [face for face in mesh.faces if face.checker_color is not None]
        patches = lab.ground_patches(mesh)
        self.assertEqual(lab.collision_c(mesh, "coverage_test"), collision)

        # Compare exact coloured coverage in every horizontal band, including
        # holes and narrow letters/entry lines, excluding render-only buried ground.
        # New rectangle edges may disappear.
        def rectangles(faces):
            output = []
            for face in faces:
                axis, winding, lo, hi = lab.flat_rectangle(face)
                self.assertEqual((axis, winding, lo[1], hi[1]), (1, 1, 0, 0))
                output.append((lo[0], hi[0], lo[2], hi[2],
                               (face.checker_color, face.surface, face.force)))
            return output

        before, after = rectangles(original), rectangles(patches)
        edges = sorted({edge for rectangle in before + after for edge in rectangle[2:4]}
                       | {edge for rectangle in mesh.covered_ground for edge in rectangle[2:4]})

        def band(rectangles, z0, z1, omit_covered=False):
            spans = sorted((x0, x1, style) for x0, x1, low, high, style in rectangles
                           if low <= z0 and z1 <= high)
            if omit_covered:
                for left, right, low, high in mesh.covered_ground:
                    if low <= z0 and z1 <= high:
                        clipped = []
                        for x0, x1, style in spans:
                            if x1 <= left or x0 >= right:
                                clipped.append((x0, x1, style))
                            else:
                                if x0 < left:
                                    clipped.append((x0, left, style))
                                if x1 > right:
                                    clipped.append((right, x1, style))
                        spans = clipped
            joined = []
            for x0, x1, style in spans:
                if joined:
                    self.assertLessEqual(joined[-1][1], x0)
                if joined and joined[-1][1] == x0 and joined[-1][2] == style:
                    joined[-1] = (joined[-1][0], x1, style)
                else:
                    joined.append((x0, x1, style))
            return joined

        for z0, z1 in zip(edges, edges[1:]):
            self.assertEqual(band(before, z0, z1, omit_covered=True), band(after, z0, z1))

        # Long, disjoint spans share world-anchored UVs and need no old culling split.
        strip = lab.Mesh()
        strip.floor(0, 12000, 0, 1000, tile=1000, aligned=True)
        for face in strip.faces:
            face.checker_color = lab.COLORS["ground"]
        self.assertEqual(len(lab.ground_patches(strip)), 1)

    def test_prebuilt_runs_avoid_spatial_list_splits(self):
        mesh = lab.build_terrain()
        self.assertEqual(len(lab.terrain_sections(lab.ground_patches(mesh))), 1)
        sections = lab.terrain_sections(lab.static_terrain_faces(mesh))
        self.assertEqual(len(sections), 6)
        self.assertEqual([face for section in sections for face in section],
                         lab.static_terrain_faces(mesh))

    def test_bounded_structure_runs_preserve_primitives_and_bounds(self):
        mesh = lab.build_terrain()
        sections = lab.terrain_sections(lab.without_textured_labels(mesh,lab.static_terrain_faces(mesh)))
        for i, source in enumerate(sections):
            name = f"physics_lab_terrain_section_{i}"
            self.assertEqual(decode(name,skip_labels=True), expected(source))
            choices = [name]
            group = source[0].lod_group
            if group is not None:
                lo, hi = lab.face_bounds(source)
                far = [face for face in mesh.lods[group]
                       if all(all(lo[a] <= p[a] <= hi[a] for a in range(3))
                              for p in face.points)]
                choices.append(name + "_distant")
                self.assertEqual(decode(choices[-1]), expected(far))
            for choice in choices:
                children = re.findall(r"gsSPDisplayList\((\w+)\)", ARRAYS[choice])
                self.assertTrue(children)
                for child in children:
                    if child=="physics_lab_labels_dl":
                        continue # Label coverage/material is checked independently below.
                    body = ARRAYS[child]
                    self.assertIn("gsSPCullDisplayList(0, 7)", body)
                    bound_name = re.search(r"gsSPVertex\((\w+), 8, 0\)", body)[1]
                    bounds = [tuple(map(int, re.findall(r"-?\d+", row)))[:3]
                              for row in ARRAYS[bound_name].splitlines() if "{{{" in row]
                    self.assertEqual(len(bounds), 8)
                    low = [min(v[a] for v in bounds) for a in range(3)]
                    high = [max(v[a] for v in bounds) for a in range(3)]
                    for triangle in decode(child):
                        for vertex in triangle:
                            for axis in range(3):
                                self.assertLessEqual(low[axis] + 8, vertex[axis])
                                self.assertGreaterEqual(high[axis] - 8, vertex[axis])
                    self.assertEqual(decode(child, culled=True), [])
        state = []
        self.assertEqual(decode("physics_lab_terrain_dl", culled=True, seen=state),
                         decode("physics_lab_ground_dl"))
        self.assertEqual(state[-2][0], "gsSPSetGeometryMode")

    def test_static_batches_preserve_every_lod_combination(self):
        rows = MODEL.split("physics_lab_terrain_batches[] = {", 1)[1].split("};", 1)[0]
        batches = re.findall(r"\{ (\w+), (\w+), (\d+) \}", rows)
        self.assertEqual(len(batches), 6)
        sections = lab.terrain_sections(lab.static_terrain_faces(lab.build_terrain()))
        groups = list(lab.build_terrain().lods)
        for mask in range(8):
            actual = [tri for near,far,bits in batches for tri in decode(far if mask & int(bits) else near)]
            intended = []
            for i,faces in enumerate(sections):
                group = faces[0].lod_group
                suffix = "_distant" if group is not None and mask & (1 << groups.index(group)) else ""
                intended.extend(decode(f"physics_lab_terrain_section_{i}{suffix}"))
            self.assertEqual(actual, intended)
        self.assertNotIn('physics_lab_occluders', MODEL)
        self.assertNotIn('PhysicsLabTerrainNode', MODEL)

    def test_distant_meshes_and_material_restoration(self):
        # Decoded shipped lists, not just generator metadata: every LOD must
        # save real triangle/vertex commands, retain bounds and restore state.
        for i in range(12):
            near=f"physics_lab_sculpture_{i}"
            far=f"physics_lab_sculpture_{i}_distant"
            a,b=decode(near),decode(far)
            self.assertEqual((len(a),len(b)),(92,50))
            points=[v[:3] for tri in a for v in tri]
            lo=[min(p[a] for p in points) for a in range(3)]
            hi=[max(p[a] for p in points) for a in range(3)]
            for tri in b:
                self.assertGreater(sum(v*v for v in lab.normal([p[:3] for p in tri])),0)
                for vertex in tri:
                    self.assertTrue(all(lo[a]<=vertex[a]<=hi[a] for a in range(3)))
                    self.assertEqual(vertex[-1],255)
            visible,culled=[],[]
            decode(far,seen=visible)
            self.assertEqual(decode(far,culled=True,seen=culled),[])
            self.assertEqual(visible,culled)
            uploads=lambda name: sum(int(n) for n in re.findall(r"gsSPVertex\([^,]+, (\d+), 0\)",ARRAYS[name]))
            self.assertLess(uploads(far),uploads(near)*.50)
            # Sharing distant colors must never collapse or move a silhouette
            # corner, change primitive order, or exceed the shade tolerance.
            material, cx, cz, scale, _ = lab.sculpture_instances()[i]
            direction = 1 if scale == 1 else -1
            wanted = []
            for _, tri in lab.sculpture_render_triangles(True):
                points = tuple(tuple(round((v - lab.SCULPTURE_ORIGIN[a]) / lab.SCULPTURE_SCALE)
                                     for a, v in enumerate((cx + direction * x * scale,
                                                            y * scale, cz + direction * z * scale)))
                               for x, y, z in tri)
                wanted.append(points)
            self.assertEqual([tuple(v[:3] for v in tri) for tri in b], wanted)
            for rendered, (index, tri) in zip(b, lab.sculpture_render_triangles(True)):
                color = tuple(round(c * (.84, .95, 1.0)[index % 3])
                              for c in lab.SCULPTURES[material][3])
                original = lab.shaded_color([v[:3] for v in rendered], color)
                for vertex in rendered:
                    self.assertTrue(all(abs(vertex[-4 + a] - original[a]) <= 8 for a in range(3)))
        mesh=lab.build_terrain()
        sections=lab.terrain_sections(lab.static_terrain_faces(mesh))
        near_count=far_count=0
        for i,faces in enumerate(sections):
            if faces[0].lod_group is None:
                continue
            near=decode(f"physics_lab_terrain_section_{i}")
            far=decode(f"physics_lab_terrain_section_{i}_distant")
            self.assertEqual(len(near),len(far)*2)
            near_count += len(near)
            far_count += len(far)
        self.assertEqual((near_count,far_count),(384,192))
        for prefix in ("ground","terrain"):
            for name in ARRAYS:
                if not re.fullmatch(rf"physics_lab_{prefix}_group_\d+_distant",name):
                    continue
                calls=re.findall(r"gsSPDisplayList\((\w+)\)",ARRAYS[name])
                near_calls=re.findall(r"gsSPDisplayList\((\w+)\)",ARRAYS[name.removesuffix("_distant")])
                self.assertEqual([c.removesuffix("_distant") for c in calls],near_calls)
                self.assertTrue(any(c.endswith("_distant") for c in calls))
    def test_production_submission_and_cutoffs(self):
        with tempfile.TemporaryDirectory(prefix="lab-render-") as temporary:
            directory = Path(temporary)
            prepare_render_fixture(directory)
            subprocess.run(["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror",
                            "-I", str(directory), str(ROOT / "tests/physics_lab_render_test.c"),
                            "-lm", "-o", str(directory / "test")], check=True)
            subprocess.run([str(directory / "test")], check=True)


def prepare_render_fixture(directory):
    source = Source(ROOT / "src/game/physics_lab_render.c")
    header = Source(ROOT / "src/game/physics_lab.h")
    types = "".join(header.declaration("struct", name) for name in
                    ("PhysicsLabRenderSection", "PhysicsLabTerrainBatch", "PhysicsLabSculpture"))
    types += Source(ROOT / "src/engine/behavior_script.h").macros("ENEMY_DRAW_DISTANCE_EXIT_SCALE")
    types += "\n" + "\n".join(re.findall(r"^#define LAB_SCULPTURE_.*$", read_source(ROOT / "src/game/physics_lab.h"), re.M)) + "\n"
    bounds = read_source(ROOT / "levels/physics_lab/object_bounds.inc.c")
    ids = re.findall(r"\{ ((?:LAB_)?MODEL_\w+),", bounds)
    types += "enum { " + ",".join(ids) + " };\n"
    types += source.declaration("struct", "PhysicsLabObjectBound") + bounds
    types += source.declaration("struct", "PhysicsLabObjectCache") + "\nstatic struct PhysicsLabObjectCache sLabObjectCache[128];\n"
    types += source.declaration("struct", "PhysicsLabView") + "\nstatic struct PhysicsLabView sLabView;\nstatic u32 sLabDistantMeshes;\n"
    types += source.declaration("enum", "PhysicsLabTerrainView")
    types += source.macros("LAB_GROUND_WIDE_CLIP_MIN_DEPTH")
    types += "s32 gWaterEffectViewRequested;\n"
    (directory / "physics_lab_render_definitions.inc").write_text(types)
    tables = "\n".join(re.search(r"const struct \w+ " + name + r"\[\] = \{.*?\n\};", MODEL, re.S)[0]
                       for name in ("physics_lab_terrain_batches", "physics_lab_terrain_north_batches", "physics_lab_terrain_aerial_batches", "physics_lab_terrain_lod_bounds", "physics_lab_sculptures"))
    names = sorted(set(re.findall(r"physics_lab_(?:terrain_batch|sculpture)_\d+(?:_distant)?(?:_north|_aerial)?", tables)))
    tables = "\n".join("static const Gfx " + name + "[] = {{0,NULL}};" for name in names) + "\n" + tables
    tables += "\n" + "\n".join(re.search(r"const u32 " + name + r" = \d+;", MODEL)[0]
                              for name in ("physics_lab_terrain_batch_count", "physics_lab_terrain_lod_count", "physics_lab_sculpture_count"))
    tables+="\n"+re.search(r"const f32 physics_lab_north_view_z = -?\d+\.0f;",MODEL)[0]
    tables+="\n"+re.search(r"const f32 physics_lab_aerial_view_y = -?\d+\.0f;",MODEL)[0]
    tables+="\n"+re.search(r"const f32 physics_lab_aerial_view_half_width = \d+\.0f;",MODEL)[0]
    tables += "\n" + re.search(r"const struct PhysicsLabRenderSection physics_lab_ground_clip_bounds = \{.*?\};", MODEL)[0]
    (directory / "physics_lab_render_tables.inc").write_text(tables)
    functions = "".join(source.function(name) for name in (
        "physics_lab_prepare_view", "object_bounds_in_view", "physics_lab_enemy_in_view",
        "object_water_effect_in_view", "physics_lab_activity_in_view", "physics_lab_use_distant_mesh",
        "lab_reset_object_cache", "lab_object_bound", "physics_lab_object_in_view", "physics_lab_terrain_view", "geo_physics_lab_terrain"))
    functions += Source(ROOT / "src/game/physics_lab_visibility.c").function("geo_physics_lab_sculptures")
    (directory / "physics_lab_render_functions.inc").write_text(functions)



if __name__ == "__main__":
    unittest.main()
