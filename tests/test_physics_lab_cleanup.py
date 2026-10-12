#!/usr/bin/env python3
"""Independent footprint checks and sampled poses for lab rendering exports."""
import math
from collections import defaultdict
from dataclasses import replace
from pathlib import Path
import random
import re
import sys
import unittest

from test_physics_lab_rendering import lab
sys.path.insert(0,str(Path(__file__).resolve().parents[1] / 'tools'))
import generate_physics_lab_bounds as bounds


class TerrainCleanup(unittest.TestCase):
    def test_further_merges_preserve_plane_coverage_and_intersections(self):
        def floor(x0,x1,y=0,z0=0,z1=10):
            return lab.Face(((x0,y,z0),(x0,y,z1),(x1,y,z1),(x1,y,z0)),(100,100,100),lab.DEFAULT)
        a,b= floor(0,10),floor(10,20)
        remote=floor(0,20,100)
        self.assertEqual(lab.merge_clear_terrain_faces([a,remote,b]),[remote,floor(0,20)])
        crossing=lab.Face(((5,-10,0),(5,10,0),(5,10,10),(5,-10,10)),a.color,a.surface)
        for blocker in (floor(5,15),crossing):
            self.assertEqual(lab.merge_clear_terrain_faces([a,blocker,b]),[a,blocker,b])
        original=lab.merge_terrain_faces(lab.exposed_terrain_faces(lab.build_terrain()))
        merged=lab.merge_clear_terrain_faces(original)
        self.assertLess(len(merged),len(original))
        groups=[defaultdict(list),defaultdict(list)]
        for collection,faces in zip(groups,(original,merged)):
            for face in faces:
                rectangle=lab.flat_rectangle(face)
                if rectangle:
                    axis, sign, lo, hi = rectangle
                    axes = [a for a in range(3) if a != axis]
                    key=(axis,sign,lo[axis],face.color,face.surface,face.force,face.layer)
                    collection[key].append(tuple((lo[a],hi[a]) for a in axes))
        self.assertEqual(groups[0].keys(),groups[1].keys())
        for key,old in groups[0].items():
            new=groups[1][key]
            xs,ys=[sorted({v for rect in old+new for v in rect[a]}) for a in range(2)]
            for x0,x1 in zip(xs,xs[1:]):
                for y0,y1 in zip(ys,ys[1:]):
                    counts=[sum(r[0][0]<=x0<x1<=r[0][1] and r[1][0]<=y0<y1<=r[1][1] for r in rects) for rects in (old,new)]
                    self.assertEqual(*counts)
        self.assertEqual([f for f in original if lab.flat_rectangle(f) is None],
                         [f for f in merged if lab.flat_rectangle(f) is None])

    def test_removed_portions_are_covered_by_solids_or_sealed_opposing_caps(self):
        mesh = lab.build_terrain()
        original = [f for f in mesh.faces if f.layer == 'opaque' and f.checker_color is None]
        collision = lab.collision_c(mesh, "cleanup")
        kept = lab.exposed_terrain_faces(mesh)
        identities = {id(f) for f in kept}
        solids = lab.closed_terrain_boxes(mesh)
        ground = [(min(p[0] for p in f.points), max(p[0] for p in f.points),
                   min(p[2] for p in f.points), max(p[2] for p in f.points))
                  for f in mesh.faces if f.checker_color is not None]
        caps = [f for f, seal in mesh.end_caps
                if f.lod_group is None and f.layer == 'opaque'
                and f.surface != 'SURFACE_VANISH_CAP_WALLS'
                and (seal is None or lab.rectangle_covered(seal, ground))]
        original_ids = {id(f) for f in original}
        fragments = defaultdict(list)
        for face in kept:
            rect = lab.flat_rectangle(face)
            if rect is not None:
                axis, sign, lo, _ = rect
                fragments[axis, sign, lo[axis]].append(face)

        removed = 0
        clipped = 0
        for face in original:
            if id(face) in identities:
                continue
            removed += 1
            opposing = [cap for cap in caps if cap is not face
                        and frozenset(cap.points) == frozenset(face.points)
                        and sum(a * b for a, b in zip(lab.normal(face.points),
                                                     lab.normal(cap.points))) < 0]
            if any(cap is face for cap in caps) and opposing:
                continue
            rect = lab.flat_rectangle(face)
            self.assertIsNotNone(rect, face)
            axis, sign, lo, hi = rect
            a, b = [i for i in range(3) if i != axis]
            visible = []
            for fragment in fragments[axis, sign, lo[axis]]:
                _, _, fl, fh = lab.flat_rectangle(fragment)
                if (replace(fragment, points=face.points) == face
                        and all(lo[i] <= fl[i] < fh[i] <= hi[i] for i in (a, b))):
                    visible.append((fl[a], fh[a], fl[b], fh[b]))
            clipped += bool(visible)
            covers = [(bl[a], bh[a], bl[b], bh[b]) for bl, bh in solids
                      if (bl[axis] <= lo[axis] < bh[axis] if sign > 0
                          else bl[axis] < lo[axis] <= bh[axis])]
            if face.lod_group is None:
                for cap in caps:
                    cr = lab.flat_rectangle(cap)
                    if cr is not None and cr[:2] == (axis, -sign) and cr[2][axis] == lo[axis]:
                        covers.append((cr[2][a], cr[3][a], cr[2][b], cr[3][b]))
            # Partition at every cover and fragment edge. The midpoint of each
            # cell independently proves that exactly the uncovered area survives.
            xs = sorted({lo[a], hi[a]} | {v for r in covers + visible
                                         for v in r[:2] if lo[a] < v < hi[a]})
            ys = sorted({lo[b], hi[b]} | {v for r in covers + visible
                                         for v in r[2:] if lo[b] < v < hi[b]})
            for x0, x1 in zip(xs, xs[1:]):
                for y0, y1 in zip(ys, ys[1:]):
                    x, y = (x0 + x1) / 2, (y0 + y1) / 2
                    def contains(rectangle):
                        return rectangle[0] < x < rectangle[1] and rectangle[2] < y < rectangle[3]
                    self.assertEqual(any(map(contains, visible)),
                                     not any(map(contains, covers)), face)
        self.assertGreater(removed, 100)
        self.assertGreater(clipped, 0)
        # Every newly generated fragment retains an original face's attributes
        # and lies within that face; this also rejects unrelated extra geometry.
        for fragment in kept:
            if id(fragment) in original_ids:
                continue
            fr = lab.flat_rectangle(fragment)
            self.assertIsNotNone(fr)
            axis, sign, lo, hi = fr
            self.assertTrue(any(replace(fragment, points=f.points) == f
                                and (r := lab.flat_rectangle(f)) is not None
                                and r[:2] == (axis, sign)
                                and r[2][axis] == lo[axis]
                                and all(r[2][i] <= lo[i] <= hi[i] <= r[3][i] for i in range(3))
                                for f in original), fragment)
        self.assertEqual(lab.collision_c(mesh, "cleanup"), collision)
        self.assertEqual(mesh.faces, lab.build_terrain().faces)

    def test_gaps_partial_faces_vanish_and_open_boxes_survive(self):
        def fixture(x0,x1,surface=lab.DEFAULT,bottom=True):
            m = lab.Mesh()
            m.box(x0, x1, 0, 100, 0, 100, surface=surface, bottom=bottom)
            return m
        a=fixture(0,100)
        target=next(f for f in a.faces if all(p[0]==100 for p in f.points))
        for gap,surface,closed in ((1,lab.DEFAULT,True),(0,'SURFACE_VANISH_CAP_WALLS',True),(0,lab.DEFAULT,False)):
            m = fixture(100 + gap, 200, surface, closed)
            m.faces.insert(0, target)
            self.assertIn(target,lab.exposed_terrain_faces(m))
        m = fixture(100, 200)
        m.faces.insert(0, target)
        self.assertNotIn(target,lab.exposed_terrain_faces(m))
        m = fixture(100, 200)
        m.boxes = [((100, 0, 0), (200, 100, 99), True)]
        m.faces.insert(0, target)
        fragments = [f for f in lab.exposed_terrain_faces(m)
                     if all(p[0] == 100 for p in f.points) and lab.normal(f.points)[0] > 0]
        expected = replace(target, points=tuple((x, y, max(z, 99)) for x, y, z in target.points))
        self.assertEqual(fragments, [expected])
        self.assertFalse(lab.rectangle_covered((0,10,0,10),[(0,4,0,10),(6,10,0,10)]))
        self.assertTrue(lab.rectangle_covered((0,10,0,10),[(0,6,0,10),(4,10,0,10)]))

    def test_transformed_solids_and_ground_holes(self):
        m = lab.Mesh()
        lab.place(m, lambda x: x.box(10, 30, 50, 90, 0, 100), dx=100, dz=200, quarter_turn=True)
        self.assertEqual(m.boxes,[((150,0,170),(190,100,190),False)])
        self.assertEqual(lab.closed_terrain_boxes(m),[])
        for x0,x1 in ((150,169),(171,190)):
            f=lab.Face(((x0,0,170),(x0,0,190),(x1,0,190),(x1,0,170)),(1,2,3),lab.DEFAULT,checker_color=(1,2,3))
            m.faces.append(f)
        self.assertEqual(lab.closed_terrain_boxes(m),[])
        m.faces.append(lab.Face(((169,0,170),(169,0,190),(171,0,190),(171,0,170)),(1,2,3),lab.DEFAULT,checker_color=(1,2,3)))
        self.assertEqual(len(lab.closed_terrain_boxes(m)),1)


class ObjectBounds(unittest.TestCase):
    def test_exports_and_unsupported_callbacks(self):
        rows, skipped = bounds.records()
        self.assertEqual(bounds.OUT.read_text(), bounds.output(rows, skipped))
        self.assertGreater(len(rows), 60)
        self.assertIn(('MODEL_CHUCKYA', 'geo_update_held_mario_pos'), skipped)
        self.assertIn(('LAB_MODEL_SNUFIT', 'geo_snufit_move_mask'), skipped)
        parser = bounds.Bounds()
        parser.arrays['bad'] = ('Gfx', 'gsSPMatrix(matrix, 0),\ngsSPEndDisplayList(),')
        with self.assertRaises(bounds.Unsupported):
            parser.display('bad')
        parser.arrays['nested'] = ('Gfx', 'gsSPMatrix(pointer(matrix), 0),')
        with self.assertRaises(bounds.Unsupported):
            parser.display('nested')
        with self.assertRaises(bounds.Unsupported):
            parser.measure(bounds.Node('GEO_HELD_OBJECT'))

    def test_palette_bounds_cover_every_branch_and_reject_transform_callbacks(self):
        parser = bounds.Bounds()
        for name, radius in (('small', 3), ('large', 12)):
            parser.arrays[name + '_vertices'] = ('Vtx', '{{{ %d, 0, 0 }}}' % radius)
            parser.arrays[name] = ('Gfx', f'gsSPVertex({name}_vertices, 1, 0), gsSPEndDisplayList(),')
        palette = bounds.Node('GEO_SWITCH_CASE', ['2', 'geo_switch_mario_palette'], [
            bounds.Node('GEO_DISPLAY_LIST', ['LAYER_OPAQUE', 'small']),
            bounds.Node('GEO_DISPLAY_LIST', ['LAYER_OPAQUE', 'large']),
        ])
        self.assertEqual(parser.measure(palette), (20, 0, 1.0))

        palette.children[1] = bounds.Node('GEO_ASM', ['0', 'geo_update_projectile_pos_from_parent'])
        with self.assertRaises(bounds.Unsupported):
            parser.measure(palette)
        palette.children.pop()
        palette.args[-1] = 'geo_unknown_selector'
        with self.assertRaises(bounds.Unsupported):
            parser.measure(palette)

    def test_cap_palettes_preserve_ukiki_limit_and_blizzard_fallbacks(self):
        parser = bounds.Bounds()
        standard = parser.display('mario_cap_seg3_dl_03022F48')
        alternate = parser.display('he_thicc_mario_cap_seg3_dl_03022F48')
        self.assertGreater(standard, 0)
        self.assertEqual(standard, alternate)
        self.assertEqual(parser.measure(parser.layout('mario_cap_palette_geo')),
                         (math.ceil(standard + 8), 0, 1.0))

        rows, skipped = bounds.records()
        self.assertTrue(any(model == 'LAB_MODEL_UKIKI' for model, *_ in rows))
        self.assertIn('{ LAB_MODEL_UKIKI, 3500 }', bounds.output(rows, skipped))
        for model in ('LAB_MODEL_MR_BLIZZARD', 'LAB_MODEL_MR_BLIZZARD_HIDDEN'):
            self.assertIn((model, 'geo_update_projectile_pos_from_parent'), skipped)

    def test_object_cost_estimates_use_alternative_branches(self):
        parser=bounds.Bounds()
        parser.arrays['triangle']=('Gfx','gsSP1Triangle(0,1,2,0),')
        parser.arrays['quad']=('Gfx','gsSP2Triangles(0,1,2,0,0,2,3,0),')
        parser.arrays['combined']=('Gfx','gsSPDisplayList(triangle), gsSPDisplayList(quad),')
        a=bounds.Node('GEO_DISPLAY_LIST',['LAYER_OPAQUE','triangle'])
        b=bounds.Node('GEO_DISPLAY_LIST',['LAYER_OPAQUE','combined'])
        self.assertEqual(parser.triangles(bounds.Node(children=[a,b])),4)
        self.assertEqual(parser.triangles(bounds.Node('GEO_SWITCH_CASE',children=[a,b])),3)
        self.assertEqual(parser.triangles(parser.layout('bubbly_tree_geo')),2)
        self.assertGreaterEqual(parser.triangles(parser.layout('koopa_with_shell_geo')),128)

    def test_all_exported_models_contain_transformed_vertices(self):
        parser = bounds.Bounds()
        rng = random.Random(9027)
        point_cache = {}
        def points(name):
            if name=='NULL':return []
            if name not in point_cache:
                out=[]
                for op,args in re.findall(r'\b(gsSPVertex|gsSPDisplayList|gsSPBranchList)\(([^()]*)\)',parser.arrays[name][1]):
                    target=args.split(',')[0].split()[0]
                    if op=='gsSPVertex':
                        out += [tuple(map(int,p)) for p in re.findall(r'\{\s*\{\s*\{?\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)',parser.arrays[target][1])]
                    else:out += points(target)
                point_cache[name]=out
            return point_cache[name]
        def mul(a,b):return [[sum(a[i][k]*b[k][j] for k in range(3)) for j in range(3)] for i in range(3)]
        def vec(a,p):return [sum(a[i][k]*p[k] for k in range(3)) for i in range(3)]
        identity=[[1,0,0],[0,1,0],[0,0,1]]
        def rotation():
            a,b,c=[rng.uniform(-math.pi,math.pi) for _ in range(3)]
            x=[[1,0,0],[0,math.cos(a),-math.sin(a)],[0,math.sin(a),math.cos(a)]]
            y=[[math.cos(b),0,math.sin(b)],[0,1,0],[-math.sin(b),0,math.cos(b)]]
            z=[[math.cos(c),-math.sin(c),0],[math.sin(c),math.cos(c),0],[0,0,1]]
            return mul(mul(x,y),z)
        total=0
        for model,geo,radius,_,_ in bounds.records()[0]:
            tree=parser.layout(geo)
            for pose in range(8):
                def visit(node,matrix,position):
                    nonlocal total
                    op, args = (node.op, node.args)
                    display = None
                    if op=='GEO_SCALE':
                        scale = int(args[1], 0) / 65536
                        matrix = [[v * scale for v in row] for row in matrix]
                    elif op in ('GEO_ANIMATED_PART','GEO_TRANSLATE_NODE','GEO_TRANSLATE_NODE_WITH_DL','GEO_TRANSLATE_ROTATE','GEO_TRANSLATE_ROTATE_WITH_DL'):
                        offset=vec(matrix,list(map(lambda x:int(x,0),args[1:4])))
                        position=[a+b for a,b in zip(position,offset)]
                        matrix=mul(matrix,rotation())
                        if op=='GEO_ANIMATED_PART' or op.endswith('_WITH_DL'):display=args[-1]
                    elif op.startswith('GEO_BILLBOARD'):
                        if args:
                            offset=vec(matrix,list(map(lambda x:int(x,0),args[1:4])))
                            position=[a+b for a,b in zip(position,offset)]
                        matrix=rotation()
                    elif op=='GEO_ROTATION_NODE':matrix=mul(matrix,rotation())
                    elif op=='GEO_DISPLAY_LIST':display=args[-1]
                    if display:
                        for point in points(display):
                            world=[a+b for a,b in zip(vec(matrix,point),position)]
                            self.assertLessEqual(sum(v*v for v in world),radius*radius,(model,pose,point,world,radius))
                            total+=1
                    for child in node.children:visit(child,matrix,position)
                visit(tree,identity,[0,0,0])
        self.assertGreater(total,50000)
        print(f'Object bounds: {total} transformed vertices contained across sampled articulated poses.')


if __name__=='__main__':unittest.main()
