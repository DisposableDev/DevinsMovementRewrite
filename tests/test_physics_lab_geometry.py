#!/usr/bin/env python3
"""Inspect the shipped collision data, not a substitute copy of the generator.

These checks verify the map's surfaces, dimensions and routes, not Mario's
collision response, rendering, camera, moving-platform behavior or game feel.
"""
import math
from collections import Counter
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]


def read_collision():
    text = (ROOT / "levels/physics_lab/collision.inc.c").read_text()
    text = text.split("const Collision physics_lab_collision[] = {", 1)[1].split("};", 1)[0]
    vertices = [tuple(map(int, p)) for p in re.findall(r"COL_VERTEX\((-?\d+), (-?\d+), (-?\d+)\)", text)]
    triangles = []
    expected = 0
    surface = None
    for command, body in re.findall(r"(COL_TRI_INIT|COL_TRI_SPECIAL|COL_TRI)\(([^)]+)\)", text):
        args = [v.strip() for v in body.split(",")]
        if command == "COL_TRI_INIT":
            assert expected == 0, "Previous surface triangle count does not match"
            surface, expected = args[0], int(args[1])
        else:
            assert expected > 0
            points = tuple(vertices[int(i)] for i in args[:3])
            a, b, c = points
            u = [b[i]-a[i] for i in range(3)]
            v = [c[i]-a[i] for i in range(3)]
            n = (u[1]*v[2]-u[2]*v[1], u[2]*v[0]-u[0]*v[2], u[0]*v[1]-u[1]*v[0])
            triangles.append((points, n, surface, int(args[3], 0) if len(args)==4 else 0))
            expected -= 1
    assert expected == 0
    assert len(vertices) == int(re.search(r"COL_VERTEX_INIT\((\d+)\)", text)[1])
    return vertices, triangles


VERTICES, TRIANGLES = read_collision()
def heights_at(x, z, ceilings=False):
    hits = []
    for points, n, surface, force in TRIANGLES:
        if (n[1] >= 0 if ceilings else n[1] <= 0):
            continue
        a,b,c = points
        den = (b[2]-c[2])*(a[0]-c[0])+(c[0]-b[0])*(a[2]-c[2])
        f = ((b[2]-c[2])*(x-c[0])+(c[0]-b[0])*(z-c[2]))/den
        g = ((c[2]-a[2])*(x-c[0])+(a[0]-c[0])*(z-c[2]))/den
        if min(f,g,1-f-g) >= -1e-7:
            hits.append((f*a[1]+g*b[1]+(1-f-g)*c[1],surface,force))
    return hits


def floor(x,z):
    return max(heights_at(x,z), default=(-math.inf,None,None))


def fixture_placement(behavior, params=r"[^,]+"):
    script=(ROOT/"levels/physics_lab/script.c").read_text()
    match=re.search(r"OBJECT\([^,]+, (-?\d+), (-?\d+), (-?\d+), 0, (\d+), 0, "
                    +params+r", "+behavior+r"\)",script)
    assert match is not None, behavior
    return tuple(map(int,match.groups()))


def enemy_placements(behavior):
    source = (ROOT / "levels/physics_lab/enemies.inc.c").read_text()
    pattern = (r"OBJECT\(([^,]+), (-?\d+), (-?\d+), (-?\d+), 0, (\d+), 0, "
               r"([^,]+), " + behavior + r"\)")
    return [{"model": model, "position": tuple(map(int, (x, y, z))),
             "yaw": int(yaw), "params": params.strip()}
            for model, x, y, z, yaw, params in re.findall(pattern, source)]


def collision_vertices(name, path="levels/physics_lab/collision.inc.c"):
    source=(ROOT/path).read_text().split("const Collision "+name+"[] = {",1)[1].split("};",1)[0]
    return [tuple(map(int,v)) for v in re.findall(r"COL_VERTEX\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)",source)]


def static_intrusions(lo, hi):
    # Conservative triangle bounds: a clear result guarantees no static face
    # enters this volume. Touching the ground or a dock boundary is allowed.
    return [tri for tri,_,_,_ in TRIANGLES if all(
        max(p[i] for p in tri)>lo[i]+.01 and min(p[i] for p in tri)<hi[i]-.01
        for i in range(3))]


class PhysicsLabGeometry(unittest.TestCase):
    def test_movers_have_clear_swept_space(self):
        shuttle=fixture_placement("bhvPhysicsLabMover","LAB_MOVER_SHUTTLE << 16")
        disk=fixture_placement("bhvPhysicsLabMover","LAB_MOVER_DISK << 16")
        bridge=fixture_placement("bhvPhysicsLabMover","LAB_MOVER_BRIDGE << 16")
        for (x,y,z,yaw),name,travel in [(shuttle,"shuttle",900),(disk,"disk",0),(bridge,"bridge",0)]:
            vertices=collision_vertices("physics_lab_"+name+"_collision")
            if name=="disk":
                radius=max(math.hypot(a,c) for a,b,c in vertices)
                lo = (x - radius, y - 100, z - radius)
                hi = (x + radius, y, z + radius)
            elif name=="bridge":
                poses=[(a*math.cos(t)-b*math.sin(t),a*math.sin(t)+b*math.cos(t),c)
                       for a,b,c in vertices for t in (-0x700*math.tau/65536,0,0x700*math.tau/65536)]
                # The farthest horizontal point may occur between the extremes.
                radius=max(math.hypot(a,b) for a,b,c in vertices)
                lo=(x-radius,y+min(b for a,b,c in poses),z-250)
                hi=(x+radius,y+max(b for a,b,c in poses),z+250)
            else:
                lo=(x+min(a for a,b,c in vertices)-travel,y-100,z-400)
                hi=(x+max(a for a,b,c in vertices)+travel,y,z+400)
            self.assertEqual(static_intrusions(lo,hi),[],name)
        # Keep a walkable boarding ramp close to, but outside, the disk sweep.
        x,y,z,_=disk
        self.assertEqual(floor(x,z+700),(y,"SURFACE_NOT_SLIPPERY",0))
        self.assertIn((0,"SURFACE_DEFAULT",0),heights_at(x,z+1600))
        self.assertGreaterEqual(min(y for y,_,_ in heights_at(x,z+1600,True)),220)

    def test_tilter_clearance_and_boarding_ramps(self):
        x,y,z,_=fixture_placement("bhvPhysicsLabTiltingPlatform")
        radius=max(math.sqrt(sum(v*v for v in p)) for p in collision_vertices("physics_lab_tilter_collision"))
        # This bound covers every orientation, including corner-loaded tilts.
        self.assertGreater(y-radius,40)
        self.assertEqual(static_intrusions((x-radius,y-radius,z-radius),(x+radius,y+radius,z+radius)),[])
        for x,z0,z1,top in [(-1550,-5700,-4950,550),(1700,-5500,-4800,500)]:
            for z in range(z0,z1,100):
                self.assertAlmostEqual(floor(x,z)[0],top*(z1-z)/(z1-z0))
                self.assertEqual(heights_at(x,z,True),[])

    def test_return_routes_do_not_cross_other_tests(self):
        for x in (-12550,-12300,-12100,-11650):
            for z in (3000,3500,3850,4200):
                expected=800*(4300-z)/1800
                above_ground={round(y,5) for y,_,_ in heights_at(x,z) if y>0}
                self.assertEqual(above_ground,{round(expected,5)},(x,z))
        for x in range(-13400,-12799,100):
            self.assertAlmostEqual(floor(x,3700)[0],200*(-12800-x)/700)
        # The northern access stays west of the court, with room for Mario at
        # its edge; its main three comparison surfaces keep their full width.
        access=[p for tri,_,s,_ in TRIANGLES for p in tri
                if s=="SURFACE_NOT_SLIPPERY" and all(-12900<=v[2]<=-12000 for v in tri)
                and all(0<=v[0]<=1500 for v in tri)]
        self.assertTrue(access)
        self.assertLessEqual(max(x for x,y,z in access),1200)
        self.assertGreaterEqual(max(x for x,y,z in access)-min(x for x,y,z in access),600)
        for x in range(4600,6400,200):
            self.assertEqual(floor(x,-10675)[0],0)  # between flight tunnel and court

    def test_flight_tunnel_has_no_arch_pillar_inside(self):
        for x in range(4550,7500,100):
            for z in (-10400,-9800,-9200):
                bottom=1500-(x-4500)*.2
                self.assertAlmostEqual(floor(x,z)[0],bottom+1050)
                self.assertEqual({round(y,4) for y,_,_ in heights_at(x,z,True)}, {round(bottom+900,4)})

    def test_circling_target_and_coin_disguise_have_room(self):
        source=(ROOT/"levels/physics_lab/enemies.inc.c").read_text()
        actors=[(name,*map(int,(x,y,z))) for x,y,z,name in re.findall(
            r"OBJECT\([^,]+, (-?\d+), (-?\d+), (-?\d+), [^\n]+, (bhv\w+)\)",source)]
        for target,clearance in [("bhvMrI",900),("bhvMoneybagHidden",1000)]:
            _,x,y,z=next(a for a in actors if a[0]==target)
            for name,xx,yy,zz in actors:
                if name!=target:
                    self.assertGreater(math.hypot(x-xx,z-zz),clearance,(target,name))
            # Actual collision supports a complete approach/circling ring.
            for i in range(32):
                angle=i*math.tau/32
                xx = x + 500 * math.sin(angle)
                zz = z + 500 * math.cos(angle)
                self.assertEqual(floor(xx,zz),(0,"SURFACE_DEFAULT",0))
                self.assertEqual(heights_at(xx,zz,True),[])

    def test_small_sculptures_have_matching_scaled_collision(self):
        # Visual copies now use a single static batch; its exact transformed
        # vertices are checked in test_physics_lab_visibility. Keep this test
        # independent, comparing shipped collision with the full-size template.
        variants=["SURFACE_DEFAULT","SURFACE_SLIPPERY","SURFACE_VERY_SLIPPERY",
                  "SURFACE_NOT_SLIPPERY"]
        template=[points for points,n,s,_ in TRIANGLES if any(p[1]>0 for p in points)
                  and all(abs(x+11100)<=930 and abs(z-7900)<=950 for x,y,z in points)]
        self.assertEqual(len(template),126)
        for scale in (.5,.25):
            for i,surface in enumerate(variants):
                large_x,large_z=[(-11100,7900),(-7600,7900),(-4100,7900),(-11100,12000)][i]
                cx,cz=(large_x-600,large_z+2100) if scale==.5 else (large_x+650,large_z+2100)
                expected=sorted(tuple((round(-(x+11100)*scale+cx),round(y*scale),round(-(z-7900)*scale+cz))
                                      for x,y,z in tri) for tri in template)
                actual=sorted(points for points,n,s,_ in TRIANGLES if s==surface and any(p[1]>0 for p in points)
                              and all(abs(x-cx)<=math.ceil(930*scale) and abs(z-cz)<=math.ceil(950*scale) for x,y,z in points))
                self.assertEqual(actual,expected,(scale,surface))

    def test_both_tox_boxes_complete_routes_are_clear(self):
        script=(ROOT/"levels/physics_lab/enemies.inc.c").read_text()
        placements=re.findall(r"OBJECT\(LAB_MODEL_SSL_TOX_BOX, (-?\d+), (\d+), (-?\d+), 0, (\d+), 0, (\d+) << 16, bhvToxBox\)",script)
        self.assertEqual(len(placements),2)
        source=(ROOT/"src/game/behaviors/tox_box.inc.c").read_text()
        for placement in placements:
            x,y,z,yaw,pattern=map(int,placement)
            self.assertEqual(y,0)
            sequence=list(map(int,re.search(r"sToxBoxDirectionPattern"+str(pattern+1)+r"\[\] = \{(.*?)\}",source,re.S)[1].split(',')))
            points=[(x,z)]
            angle=math.radians(yaw)
            for action in sequence:
                left,forward={4:(0,64),5:(0,-64),6:(-64,0),7:(64,0)}.get(action,(0,0))
                dx=round(left*math.cos(angle)+forward*math.sin(angle))
                dz=round(forward*math.cos(angle)-left*math.sin(angle))
                for frame in range(8 if dx or dz else 0):
                    x += dx
                    z += dz
                    points.append((x, z))
            self.assertEqual(points[0],points[-1])
            radius=math.ceil(math.sqrt(3)*256)
            x0 = min((x for x, z in points)) - radius
            x1 = max((x for x, z in points)) + radius
            z0 = min((z for x, z in points)) - radius
            z1 = max((z for x, z in points)) + radius
            for tri,n,s,f in TRIANGLES:
                if max(p[1] for p in tri)<=0:continue
                overlaps=not (max(p[0] for p in tri)<x0 or min(p[0] for p in tri)>x1
                              or max(p[2] for p in tri)<z0 or min(p[2] for p in tri)>z1)
                self.assertFalse(overlaps,(placement,tri,s))
            for x in range(x0,x1+1,200):
                for z in range(z0,z1+1,200):
                    self.assertAlmostEqual(floor(x,z)[0],0)

    def test_environment_pool_depths_and_gas_refuges(self):
        for z,depth in [(7650,20),(8150,60),(8650,100)]:
            self.assertEqual(180-floor(4500,z+600)[0],depth)
        self.assertEqual(600-floor(4400,13300)[0],480)
        self.assertEqual(floor(3200,13300)[1],"SURFACE_ICE")
        self.assertEqual(floor(-8700,4100)[0],20)
        self.assertEqual(floor(-9400,4700)[0],600)
        self.assertEqual(floor(-7900,5000)[0],600)
        text=(ROOT/"levels/physics_lab/collision.inc.c").read_text()
        for box in ("2, 3300, 7800, 5700, 9700, 180","3, 3300, 11400, 5700, 14300, 600",
                    "50, -9900, 3400, -7400, 5300, 550"):
            self.assertTrue(f"COL_WATER_BOX({box})" in text)

    def test_inertia_runouts_and_hills(self):
        for z,surface in [(-14700,"SURFACE_DEFAULT"),(-13950,"SURFACE_ICE"),(-13200,"SURFACE_NOT_SLIPPERY")]:
            for x in range(-8000,5100,200):
                self.assertAlmostEqual(floor(x,z)[0],200)
                self.assertEqual(floor(x,z)[1:],(surface,0))
            self.assertGreater(floor(-11000,z)[0],floor(-8500,z)[0])
            self.assertGreater(floor(10000,z)[0],floor(5500,z)[0])
        for z,y in [(-10500,1100),(-8800,200),(-7100,900),(-5400,200),(-3700,700),(-2000,200)]:
            self.assertEqual(floor(-14300,z)[0],y)
        for z,surface in [(-1000,"SURFACE_ICE"),(1000,"SURFACE_DEFAULT"),(2100,"SURFACE_NOT_SLIPPERY")]:
            self.assertEqual(floor(-14300,z)[1],surface)

    def test_blj_activation_steps_and_access(self):
        for i in range(20):
            self.assertEqual(floor(14400,936-128*i)[0],64*(i+1))
        self.assertEqual(floor(14400,-2000)[0],1280)
        for z in range(-14800,-1600,200):
            self.assertAlmostEqual(floor(14400,z)[0],1280)
        for z in range(1200,10000,200):
            self.assertAlmostEqual(floor(14400,z)[0],20)
        self.assertEqual(floor(12850,2500)[0],0)  # old internal curb removed
        self.assertEqual(floor(13600,2500)[0],10)
        self.assertEqual(floor(13800,2500)[0],20)

    def test_aquatic_native_shark_orbit_has_water_and_bottom_clearance(self):
        for i in range(64):
            angle=i*math.tau/64
            x = 9600 + 1700 * math.sin(angle)
            z = 12300 + 1700 * math.cos(angle)
            y=-40-900+200*math.sin(angle)
            self.assertTrue(7300<x<12100 and 9900<z<14600)
            below=[h for h,s,f in heights_at(x,z) if h<=y+78]
            self.assertAlmostEqual(max(below),-2200)
            self.assertGreater(y-max(below),400)
            self.assertEqual(heights_at(x,z,True),[])

    def test_aquatic_basin_is_enclosed_above_the_death_plane(self):
        # The full submerged banks must push swimmers toward the tank interior,
        # and be visible from inside. Exclude the shorter faces of the exit ramp.
        for axis,plane,other,lo,hi,sign in [(0,7300,2,9900,14600,1),
                                         (0,12100,2,9900,14600,-1),
                                         (2,9900,0,7300,12100,1),
                                         (2,14600,0,7300,12100,-1)]:
            banks=[n for points,n,_,_ in TRIANGLES
                   if all(p[axis]==plane for p in points)
                   and min(p[1] for p in points)==-2200 and max(p[1] for p in points)==0
                   and min(p[other] for p in points)==lo and max(p[other] for p in points)==hi]
            self.assertEqual(len(banks),2,(axis,plane))
            self.assertTrue(all(n[axis]*sign>0 for n in banks),(axis,plane,banks))
        # The bottom covers the full footprint, including the wall/floor seams.
        bottom=[n for points,n,_,_ in TRIANGLES
                if all(p[1]==-2200 and 7300<=p[0]<=12100 and 9900<=p[2]<=14600
                       for p in points) and n[1]>0]
        self.assertEqual(sum(n[1]/2 for n in bottom),(12100-7300)*(14600-9900))
        for x in (7300,7301,9600,12099,12100):
            for z in (9900,9901,12300,14599,14600):
                self.assertTrue(any(abs(y+2200)<1e-6 for y,_,_ in heights_at(x,z)),(x,z))
        # The shore remains level, and the walking exit still reaches dry ground.
        for x,z in [(7240,12000),(12160,12000),(9600,9840),(9000,14660)]:
            self.assertEqual(floor(x,z)[0],0)
        for x in range(11550,12100,100):
            self.assertEqual(floor(x,14660)[0],0)
            self.assertAlmostEqual(floor(x,14599)[0],-2200/3700)

    def test_main_promenade_is_flat_and_open(self):
        paths=[(-7300,-6700,-6900,6700),(-7300,13200,-6900,-6400),
               (-7300,13200,6300,6700),(12600,13200,-6900,14800),
               (-2700,-2200,6900,14700),(6300,6800,7900,14700),
               (-12200,13200,14800,15100)]
        for x0,x1,z0,z1 in paths:
            for x in range(x0,x1+1,250):
                for z in range(z0,z1+1,250):
                    # The northern terrace crosses above this ground route.
                    ground = [hit for hit in heights_at(x,z) if hit[0] <= 100]
                    self.assertEqual(max(ground),(0,"SURFACE_DEFAULT",0),(x,z))
                    self.assertFalse(any(y < 300 for y,_,_ in heights_at(x,z,True)),(x,z))

    def test_northern_terrace_joins_and_clear_ground_crossings(self):
        grip = "SURFACE_NOT_SLIPPERY"
        for x in range(-2700,600,200):
            self.assertTrue(any(abs(y-800) < .001 and s == grip
                                for y,s,_ in heights_at(x,-7150)))
        for x in range(600,1501,100):
            self.assertTrue(any(abs(y-(800-200*(x-600)/900)) < .001 and s == grip
                                for y,s,_ in heights_at(x,-7150)))
        for x,z0,z1,y0,y1 in [(-2500,-6900,-6200,800,1292),
                              (-1650,-6900,-6000,800,550),
                              (1700,-6900,-5900,600,500)]:
            for z in range(z0,z1+1,50):
                expected = y0+(y1-y0)*(z-z0)/(z1-z0)
                self.assertTrue(any(abs(y-expected) < .001 and s == grip
                                    for y,s,_ in heights_at(x,z)),(x,z,expected))
            # A complete shared edge joins the existing landing or dock.
            for dx in (-100,0,100):
                self.assertTrue(any(abs(y-y1) < .001 for y,_,_ in heights_at(x+dx,z1)))
        self.assertTrue(any(abs(y-600) < .001 and s == grip
                            for y,s,_ in heights_at(1700,-7150)))

        # Columns and slab undersides leave the full walking envelope clear.
        for x in range(-2800,1901,100):
            self.assertEqual(static_intrusions((x-40,1,-6940),(x+40,300,-6360)),[])
        for z in range(-7500,-6999,100):
            self.assertEqual(static_intrusions((650,1,z-40),(1150,300,z+40)),[])
        for x,z in [(-2500,-6600),(-1650,-6600),(1700,-6600),(900,-7150)]:
            self.assertIn((0,"SURFACE_DEFAULT",0),heights_at(x,z))
            self.assertGreaterEqual(min(y for y,_,_ in heights_at(x,z,True)),400)

    def test_finishing_galleries_have_complete_ground_returns(self):
        grip = "SURFACE_NOT_SLIPPERY"
        for x in range(11050,12001,50):
            self.assertAlmostEqual(floor(x,-2400)[0],350)
            self.assertEqual(floor(x,-2400)[1:],(grip,0))
        for x in range(12000,12501,25):
            self.assertAlmostEqual(floor(x,-2400)[0],350*(12500-x)/500)
        self.assertEqual(floor(12560,-2400),(0,"SURFACE_DEFAULT",0))
        for z in range(-4100,-3399,50):
            self.assertAlmostEqual(floor(12225,z)[0],300+50*(z+4100)/700)
        for x in range(11600,12401,50):
            self.assertAlmostEqual(floor(x,-3275)[0],350)
        for z in range(-4100,-3199,50):
            old_height = 300*(-3200-z)/900
            self.assertTrue(any(abs(y-old_height) < .001 for y,_,_ in heights_at(11800,z)))
            ceilings = heights_at(11800,z,True)
            if ceilings:
                self.assertGreaterEqual(min(y for y,_,_ in ceilings)-old_height,200)

        for x in range(-2000,1621,80):
            self.assertAlmostEqual(floor(x,11525)[0],250)
            self.assertEqual(floor(x,11525)[1:],(grip,0))
        for z in range(8600,11651,100):
            self.assertAlmostEqual(floor(-1800,z)[0],250)
            self.assertEqual(floor(-1800,z)[1:],(grip,0))
        for z in range(7800,8601,50):
            self.assertAlmostEqual(floor(-1800,z)[0],250*(z-7800)/800)
        self.assertEqual(floor(-1800,7750),(0,"SURFACE_DEFAULT",0))
        for x,width in [(-1200,80),(-400,120),(500,180),(1400,260)]:
            self.assertEqual(floor(x,11399)[0],250)
            self.assertEqual(floor(x,11401)[0],250)
            self.assertEqual(floor(x-width/2+1,10000)[0],250)
            self.assertEqual(floor(x+width/2-1,10000)[0],250)
            self.assertEqual(floor(x-width/2-1,10000)[0],0)
            self.assertEqual(floor(x+width/2+1,10000)[0],0)
        # The gallery stops short of the low-roof specimens.
        self.assertEqual(static_intrusions((-1420,1,11651),(1620,1000,11699)),[])

    def test_slope_landing_has_direct_commons_return(self):
        grip = "SURFACE_NOT_SLIPPERY"
        for z in range(-5700,-3699,100):
            self.assertAlmostEqual(floor(-1800,z)[0],1292)
            self.assertEqual(floor(-1800,z)[1:],(grip,0))
        for z in range(-3700,-1299,100):
            self.assertAlmostEqual(floor(-1800,z)[0],1292*(-1300-z)/2400)
        self.assertEqual(floor(-1800,-1250),(0,"SURFACE_DEFAULT",0))
        for z in range(-5650,-4950,100):
            old_height = 550*(-4950-z)/750
            self.assertTrue(any(abs(y-old_height) < .001 for y,_,_ in heights_at(-1700,z)))
            self.assertGreaterEqual(min(y for y,_,_ in heights_at(-1700,z,True))-old_height,400)
        # The widened terrace is a shared stopping place above the north path.
        self.assertEqual(floor(-1000,-7600),(800,grip,0))
        self.assertEqual(static_intrusions((650,1,-7800),(1150,300,-6900)),[])

    def test_precision_and_shallow_water_have_shared_entry(self):
        for x in range(1900,2001,20):
            self.assertAlmostEqual(floor(x,8800)[0],250-(x-1900)/10)
        for x in range(2000,3151,50):
            self.assertAlmostEqual(floor(x,8800)[0],240)
            self.assertEqual(floor(x,8800)[1:],("SURFACE_NOT_SLIPPERY",0))
        self.assertEqual(floor(3200,8800)[0],240)
        for z in range(7800,8601,50):
            self.assertAlmostEqual(floor(2500,z)[0],240*(z-7800)/800)
        self.assertEqual(floor(2500,7750),(0,"SURFACE_DEFAULT",0))
        # The old ceiling keeps its full 450-unit entry clearance.
        self.assertEqual(floor(2300,9400)[0],650)
        self.assertEqual(heights_at(2300,9400,True),[(450,"SURFACE_DEFAULT",0)])
        self.assertEqual(static_intrusions((2000,1,9010),(2800,400,9180)),[])

    def test_commons_terrace_has_broad_clear_stopping_space(self):
        # The inset directory remains below standing eye height on the pad.
        self.assertEqual(static_intrusions((3550,245,-1750),(5950,500,-1640)),[])
        # A complete standing envelope fits along the directory and across
        # the terrace, with continuous walking slopes on all four approaches.
        for x in range(3550,5951,100):
            for z in range(-1550,-999,100):
                self.assertAlmostEqual(floor(x,z)[0],120)
                self.assertEqual(floor(x,z)[1],"SURFACE_NOT_SLIPPERY")
                self.assertEqual(static_intrusions((x-40,121,z-40),(x+40,420,z+40)),[])
        for z in range(-900,-649,25):
            self.assertAlmostEqual(floor(4750,z)[0],120*(-650-z)/250)
        for z in range(-2000,-1749,25):
            self.assertAlmostEqual(floor(4750,z)[0],120*(z+2000)/250)
        for x in range(3150,3351,25):
            self.assertAlmostEqual(floor(x,-1325)[0],120*(x-3150)/200)
        for x in range(6150,6351,25):
            self.assertAlmostEqual(floor(x,-1325)[0],120*(6350-x)/200)
        # The old isolated pad is gone; nearby measured fixtures and supply
        # starts still stand on their original floor with clear approaches.
        for x,z in [(1700,1600),(1350,1950),(2050,1950),(1700,2300),
                    (1500,2700),(1900,1100),(4750,-600)]:
            self.assertAlmostEqual(floor(x,z)[0],0)
        self.assertEqual(floor(900,1950)[0],450)
        self.assertEqual(floor(2900,-1800)[0],3000)
        self.assertEqual(static_intrusions((3000,1,-2199),(6300,300,-2001)),[])

    def test_outer_entrance_markers_keep_walking_approaches_open(self):
        for x,z in [(-11470,2630),(-9990,2630),(-11470,5330),(-9990,5330),
                    (2150,7650),(2850,7650),(3070,10200),(5770,10200),
                    (11260,14680),(12340,14680)]:
            self.assertEqual(floor(x,z),(120,"SURFACE_DEFAULT",0))
        for lo,hi in [((-11390,1,2301),(-10070,300,2700)),
                      ((-11390,1,5001),(-10070,300,5400)),
                      ((2280,1,7540),(2720,300,7800)),
                      ((3300,1,10250),(5700,300,10600)),
                      ((11440,1,14600),(12160,300,14800)),
                      ((-12750,1,5400),(-10500,400,6000))]:
            self.assertEqual(static_intrusions(lo,hi),[],(lo,hi))
            for x in range(lo[0]+60,hi[0],200):
                for z in range(lo[2]+60,hi[2],100):
                    self.assertEqual(floor(x,z),(0,"SURFACE_DEFAULT",0))

    def test_outer_direction_routes_have_clear_ground(self):
        # Garden approach bends before the slippery sculpture's plinth;
        # hills follow the western link, and aquatic uses the north shore.
        for points in [[(-6850,6350),(-8725,6775),(-9300,7500)],
                       [(-7350,5875),(-12700,5875)],
                       [(7950,9700),(9600,9700),(9600,9840)]]:
            for a,b in zip(points,points[1:]):
                lo=(min(a[0],b[0])-60,1,min(a[1],b[1])-60)
                hi=(max(a[0],b[0])+60,300,max(a[1],b[1])+60)
                self.assertEqual(static_intrusions(lo,hi),[],(a,b))
                for i in range(21):
                    x=a[0]+(b[0]-a[0])*i/20
                    z=a[1]+(b[1]-a[1])*i/20
                    self.assertEqual(floor(x,z),(0,"SURFACE_DEFAULT",0))

    def test_district_entrances_leave_full_walking_approaches(self):
        for z in range(-2200,-1299,50):
            self.assertIn((0,"SURFACE_DEFAULT",0),heights_at(7275,z))
            self.assertEqual(static_intrusions((7200,1,z-40),(7450,300,z+40)),[])
            self.assertEqual(heights_at(7275,z,True),[])
        # The asymmetric landing forms no longer span the entrance overhead.
        self.assertAlmostEqual(floor(7050,-1775)[0],150)
        self.assertAlmostEqual(floor(8450,-1650)[0],560)
        ceilings = heights_at(8450,-1650,True)
        self.assertTrue(ceilings)
        for y,surface,force in ceilings:
            self.assertAlmostEqual(y,480)
            self.assertEqual((surface,force),("SURFACE_DEFAULT",0))
        for z in range(-2700,-999,100):
            original = 350 if z <= -2100 else 350*(-1000-z)/1100
            self.assertAlmostEqual(floor(7800,z)[0],original)
            self.assertEqual(heights_at(7800,z,True),[])
        for x in range(-6200,-2299,100):
            self.assertEqual(floor(x,1220)[0],0)
            self.assertEqual(static_intrusions((x-40,1,1080),(x+40,300,1320)),[])
        for z in range(-3200,-1649,100):
            self.assertAlmostEqual(floor(-6450,z)[0],500*(-1650-z)/1550)
        # The motion shoulder remains beside the compound ramp specimens.
        self.assertEqual(floor(-6250,-2000)[0],0)

    def test_water_quays_preserve_ground_supply_crossing(self):
        grip = "SURFACE_NOT_SLIPPERY"
        # Angled quay shoulders stay inward of the continuous ground route.
        self.assertEqual(static_intrusions((6220,1,7800),(6840,400,14700)),[])
        for z0,z1,y0,y1 in [(7800,8500,0,240),(8500,9900,240,240),
                            (9900,10250,240,0),(10600,11400,0,660),
                            (11400,13900,660,660),(13900,14700,660,0)]:
            for z in range(z0,z1+1,50):
                expected = y0+(y1-y0)*(z-z0)/(z1-z0)
                self.assertAlmostEqual(floor(6025,z)[0],expected)
                if z0 < z < z1:
                    self.assertEqual(floor(6025,z)[1],grip)
                self.assertEqual(heights_at(6025,z,True),[])
        for x in range(5900,6901,100):
            for z in range(10300,10600,50):
                self.assertEqual(floor(x,z),(0,"SURFACE_DEFAULT",0))
                self.assertEqual(static_intrusions((x-40,1,z-40),(x+40,300,z+40)),[])
        self.assertAlmostEqual(floor(5849,9000)[0],240)
        self.assertAlmostEqual(floor(5851,9000)[0],240)
        self.assertAlmostEqual(floor(5849,12500)[0],660)
        self.assertAlmostEqual(floor(5851,12500)[0],660)

    def test_removed_statue_sites_contain_only_the_new_low_tests(self):
        # Repurposing the empty sites must not restore the removed sculptures.
        seam=[(points,n,surface) for points,n,surface,_ in TRIANGLES
              if all(-8500<=x<=-6550 and 11650<=z<=13800 for x,y,z in points)]
        self.assertTrue(seam)
        self.assertLessEqual(max(p[1] for points,_,_ in seam for p in points),8)
        self.assertFalse(any(surface=="SURFACE_VANISH_CAP_WALLS" for _,_,surface in seam))
        projectile=[p for points,_,_,_ in TRIANGLES
                    if all(-5000<=x<=-3050 and 11200<=z<=14600 for x,y,z in points)
                    for p in points]
        self.assertTrue(projectile)
        self.assertLessEqual(max(p[1] for p in projectile),350)

    def test_sculpture_access_and_outer_margin(self):
        for x,z in [(-11100,7900),(-7600,7900),(-4100,7900),(-11100,12000)]:
            for xx,zz in [(x-1100,z),(x+1100,z),(x,z-1150),(x,z+1150),
                          (x-600,z+2700),(x+650,z+2500)]:
                self.assertEqual(floor(xx,zz),(0,"SURFACE_DEFAULT",0),(xx,zz))
                self.assertTrue(-14900<xx<14900 and -14900<zz<14900)

    def test_seam_replicas_have_identical_geometry_and_measured_lanes(self):
        def specimen(x0,z0):
            return [(tuple((x-x0,y,z-z0) for x,y,z in points),n,surface,force)
                    for points,n,surface,force in TRIANGLES
                    if all(x0<=x<=x0+1950 and z0<=z<=z0+2150 for x,y,z in points)]
        a,b=specimen(-8500,11650),specimen(-8900,-11100)
        self.assertEqual(Counter((p,s,f) for p,_,s,f in a),Counter((p,s,f) for p,_,s,f in b))
        self.assertEqual(sum(n[1]/2 for _,n,_,_ in a if n[1]>0),1950*2150)
        for x0,z0 in [(-8500,11650),(-8900,-11100)]:
            for lane in range(4):
                x=x0+225+lane*500
                for z in (150,350,749,925,1105,1750,2000):
                    expected_y=8 if lane==2 and 900<z<1000 else 0
                    expected_surface="SURFACE_ICE" if lane==3 and 1050<z<1800 else "SURFACE_DEFAULT"
                    self.assertEqual(floor(x,z0+z),(expected_y,expected_surface,0),(lane,z))
                    self.assertEqual(heights_at(x,z0+z,True),[])
            counts=[sum(n[1]>0 and all(75+lane*500<=x<=375+lane*500 and 300<=z<=1800
                                      for x,y,z in p) for p,n,_,_ in a) for lane in (0,1)]
            self.assertEqual(counts,[2,20])

    def test_impact_beds_have_clear_drop_columns_and_walking_exits(self):
        for x,depth in [(8050,0),(9450,60),(10850,600)]:
            for z in (8350,8750,9250):
                self.assertEqual(floor(x,z),(-depth,"SURFACE_DEFAULT",0))
                self.assertEqual(heights_at(x,z,True),[])
                self.assertEqual(static_intrusions((x-100,50,z-100),(x+100,3900,z+100)),[])
            for height in (1700,3600):
                self.assertIn((height,"SURFACE_DEFAULT",0),heights_at(x,8050))
                self.assertIn((height-60,"SURFACE_DEFAULT",0),heights_at(x,8050,True))
        text=(ROOT/"levels/physics_lab/collision.inc.c").read_text()
        boxes=[tuple(map(int,box.split(','))) for box in re.findall(r"COL_WATER_BOX\(([^)]+)\)",text)]
        for bounds in [(5,8900,8250,10000,9500,0),(6,10300,8250,11400,9500,0)]:
            self.assertIn(bounds,boxes)
        self.assertFalse(any(index<50 and a<8050<b and c<8750<d for index,a,c,b,d,_ in boxes))
        for x,start,length,depth in [(9900,9250,250,60),(11250,8300,1200,600)]:
            for distance in range(0,length+1,50):
                self.assertAlmostEqual(floor(x,start+distance)[0],-depth+depth*distance/length)
            self.assertEqual(floor(x,9560),(0,"SURFACE_DEFAULT",0))

    def test_new_access_paths_and_pole_clearance(self):
        for x0,x1,z0,z1 in [(-7900,-7300,11000,11650),(-6200,-5800,-12400,-10900),
                            (-6950,-5800,-11100,-10700),(6900,7500,7800,8250),
                            (-4400,-3800,11000,11200),(-3050,-2800,13100,13700),
                            (-1650,-1000,3300,3850)]:
            for x in range(x0+60,x1,150):
                for z in range(z0+60,z1,150):
                    # Elevated release decks leave the ground route open below.
                    below=[hit for hit in heights_at(x,z) if hit[0]<=100]
                    self.assertEqual(max(below),(0,"SURFACE_DEFAULT",0),(x,z))
                    self.assertFalse(any(y<160 for y,_,_ in heights_at(x,z,True)),(x,z))
        self.assertEqual(fixture_placement("bhvPoleGrabbing","380 << 16"),(7150,0,8600,0))
        self.assertEqual(static_intrusions((7050,1,8450),(7250,3900,8750)),[])
        for height in (1700,3600):
            self.assertIn((height,"SURFACE_DEFAULT",0),heights_at(7150,8300))
        # A 250-unit open gap allows jumping off the pole toward either arm;
        # no surrounding slab interferes with the native grab/climb action.
        self.assertEqual(heights_at(7150,8400,True),[])

    def test_projectile_lanes_preserve_native_parameters_and_room(self):
        source=(ROOT/"levels/physics_lab/enemies.inc.c").read_text()
        self.assertIn("-4500, 200, 11650, 0, 0, 0, 0, bhvBulletBill",source)
        self.assertIn("-3500, 0, 13200, 0, 0, 0, 4 << 16, bhvWaterBombSpawner",source)
        for z in range(12100,13101,100):
            self.assertEqual(floor(-4500,z),(0,"SURFACE_DEFAULT",0))
            self.assertEqual(static_intrusions((-4580,1,z-80),(-4420,350,z+80)),[])
        # Stationary water-bomb trials retain the native 400-unit trigger
        # radius and 100-unit aim lead, with clear overhead spawning space.
        for z in range(12900,13601,100):
            self.assertEqual(floor(-3500,z),(0,"SURFACE_DEFAULT",0))
            self.assertEqual(static_intrusions((-3580,1,z-80),(-3420,2500,z+80)),[])
        for x,z in [(-4950,12300),(-3100,12300),(-3970,13000),(-4500,14550)]:
            self.assertEqual(floor(x,z)[0],350)

    def test_ground_comparisons_are_ordered_and_equivalent(self):
        goombas = sorted(enemy_placements("bhvGoomba"), key=lambda p: p["position"][0])
        self.assertEqual([p["params"] for p in goombas], ["2 << 16", "0", "1 << 16"])
        targets = goombas + enemy_placements("bhvFloomba")
        self.assertEqual(len(targets), 4)
        for target in targets:
            x, y, z = target["position"]
            self.assertEqual((y, z), (0, -12000))
            for dx in range(-500, 501, 100):
                for dz in range(-500, 701, 100):
                    self.assertEqual(floor(x + dx, z + dz), (0, "SURFACE_DEFAULT", 0))
            for dx in (-600, 600):
                self.assertEqual(floor(x + dx, z)[0], 350)
        koopas = enemy_placements("bhvKoopa")
        self.assertEqual(len(koopas), 2)
        self.assertEqual({p["params"] for p in koopas},
                         {"KOOPA_BP_NORMAL << 16", "KOOPA_BP_UNSHELLED << 16"})
        self.assertEqual(koopas[0]["position"][1:], koopas[1]["position"][1:])
        self.assertEqual(koopas[0]["yaw"], koopas[1]["yaw"])
        for behavior in ("bhvBobomb", "bhvFirePiranhaPlant", "bhvSnufit"):
            for target in enemy_placements(behavior):
                self.assertGreater(target["position"][2], 11000)

    def test_grabbing_targets_have_open_approaches_and_throw_recovery(self):
        chuckya = enemy_placements("bhvChuckya")[0]
        heave = enemy_placements("bhvHeaveHo")[0]
        self.assertEqual(chuckya["position"][1:], heave["position"][1:])
        self.assertGreater(heave["position"][0] - chuckya["position"][0], 2000)
        self.assertEqual(heave["yaw"], 270)
        # The marked west approach sends the native backward throw east into
        # over 3100 units of level recovery space, with no flight roof above it.
        self.assertEqual(static_intrusions((7500, 1, -8850), (13700, 2200, -7250)), [])
        for x in range(7500, 13701, 200):
            for z in range(-8750, -7349, 200):
                self.assertEqual(floor(x, z), (0, "SURFACE_DEFAULT", 0))
        for behavior in ("bhvPhysicsLabMover", "bhvPhysicsLabTiltingPlatform"):
            script = (ROOT / "levels/physics_lab/script.c").read_text()
            points = re.findall(r"OBJECT\([^,]+, (-?\d+), -?\d+, (-?\d+), "
                                r"[^\n]+, " + behavior + r"\)", script)
            for target in (chuckya, heave):
                x, _, z = target["position"]
                for xx, zz in points:
                    self.assertGreater(math.hypot(x - int(xx), z - int(zz)), 3000)

    def test_bully_variants_have_identical_dry_courts(self):
        targets = [enemy_placements(b)[0] for b in ("bhvSmallBully", "bhvSmallChillBully")]
        walls = []
        for target in targets:
            x, _, z = target["position"]
            selected = [(tuple((xx - x, y, zz - z) for xx, y, zz in points), surface, force)
                        for points, _, surface, force in TRIANGLES
                        if any(y > 0 for _, y, _ in points)
                        and all(x - 750 <= xx <= x + 850 and z - 800 <= zz <= z + 800
                                for xx, _, zz in points)]
            self.assertTrue(selected)
            walls.append(Counter(selected))
            for dx in range(-600, 601, 200):
                for dz in range(-600, 601, 200):
                    self.assertEqual(floor(x + dx, z + dz), (0, "SURFACE_DEFAULT", 0))
                    self.assertEqual(heights_at(x + dx, z + dz, True), [])
            # The same 600-unit southern opening admits a normal Mario with
            # room to move, and has level ground beyond the recovery line.
            for dx in (-200, 0, 200):
                for dz in (700, 800, 900, 1000):
                    self.assertEqual(floor(x + dx, z + dz), (0, "SURFACE_DEFAULT", 0))
        self.assertEqual(walls[0], walls[1])

    def test_heavy_row_and_burrowing_floor_clear_existing_courses(self):
        heavy = [enemy_placements(b)[0] for b in ("bhvSmallWhomp", "bhvThwomp", "bhvGrindel")]
        self.assertEqual([p["position"][2] for p in heavy], [-11800] * 3)
        self.assertEqual([p["position"][0] for p in heavy], sorted(p["position"][0] for p in heavy))
        for target, radius in zip(heavy, (350, 350, 250)):
            x, _, z = target["position"]
            self.assertEqual(static_intrusions((x - radius, 1, z - 700),
                                              (x + radius, 1800, z + 700)), [])
            for dz in range(-500, 1001, 100):
                self.assertEqual(floor(x, z + dz), (0, "SURFACE_DEFAULT", 0))
        holes = enemy_placements("bhvMontyMoleHole")
        self.assertEqual(len(holes), 3)
        self.assertIn(enemy_placements("bhvMontyMole")[0]["position"],
                      [p["position"] for p in holes])
        for target in holes:
            x, _, z = target["position"]
            for dx, dz in ((0, 0), (-100, 0), (100, 0), (0, -100), (0, 100)):
                self.assertEqual(floor(x + dx, z + dz), (0, "SURFACE_DEFAULT", 0))
                self.assertEqual(heights_at(x + dx, z + dz, True), [])

    def test_lakitu_and_spiny_have_their_own_open_spawning_floor(self):
        lakitu = enemy_placements("bhvEnemyLakitu")[0]
        spiny = enemy_placements("bhvSpiny")[0]
        self.assertEqual(lakitu["position"][0], spiny["position"][0])
        self.assertLess(lakitu["position"][2], -11000)
        self.assertLess(spiny["position"][2], -11000)
        for target in (lakitu, spiny):
            x, _, z = target["position"]
            for dx in range(-250, 251, 125):
                for dz in range(-250, 251, 125):
                    self.assertEqual(floor(x + dx, z + dz), (0, "SURFACE_DEFAULT", 0))
                    self.assertEqual(static_intrusions((x + dx - 50, 1, z + dz - 50),
                                                      (x + dx + 50, 1800, z + dz + 50)), [])

    def test_homing_amp_chase_ranges_clear_impact_and_main_routes(self):
        protected = [(7000, 11600, 7800, 9700), (-7000, 7000, -1000, 1000),
                     (-7500, 13300, -7000, -6300), (-7350, -6650, -7000, 6800),
                     (12500, 13300, -7000, 14900), (-7500, 13300, 6200, 6800),
                     (-12500, -2100, 10500, 11000), (-12500, 13300, 14800, 15100)]
        amps = enemy_placements("bhvHomingAmp")
        self.assertEqual(len(amps), 2)
        for amp in amps:
            x, _, z = amp["position"]
            for x0, x1, z0, z1 in protected:
                dx = max(x0 - x, 0, x - x1)
                dz = max(z0 - z, 0, z - z1)
                self.assertGreater(math.hypot(dx, dz), 1500)
        # The native 800-unit activation spheres do not overlap each other.
        a, b = [p["position"] for p in amps]
        self.assertGreater(math.hypot(a[0] - b[0], a[2] - b[2]), 1600)
        water_amps = [p for p in enemy_placements("bhvCirclingAmp")
                      if p["params"] == "AMP_BP_ROT_RADIUS_200 << 16"]
        self.assertEqual([p["position"] for p in water_amps],
                         [(6200, 140, 5600), (5000, 240, 8800), (5200, 320, 13600)])

    def test_added_firing_bays_have_backstops_and_a_continuous_side_approach(self):
        for behavior in ("bhvBobomb", "bhvSnufit", "bhvFirePiranhaPlant",
                         "bhvFlamethrower", "bhvBouncingFireball"):
            target = enemy_placements(behavior)[0]
            x, _, z = target["position"]
            self.assertEqual(floor(x, z), (0, "SURFACE_DEFAULT", 0))
            self.assertEqual(floor(-6300, z), (0, "SURFACE_DEFAULT", 0))
            self.assertEqual(static_intrusions((-6350, 1, z - 50),
                                              (-6250, 160, z + 50)), [])
            self.assertEqual(floor(-5100, z)[0], 1500)
        for x in (-6190, -6140, -6090):
            for z in range(11050, 14701, 50):
                self.assertEqual(floor(x, z), (0, "SURFACE_DEFAULT", 0))
                self.assertEqual(static_intrusions((x - 50, 1, z - 50),
                                                  (x + 50, 160, z + 50)), [])
        # The eastern backstop remains; the long western barrier is removed.
        snufit = enemy_placements("bhvSnufit")[0]
        x, y, z = snufit["position"]
        for wall_x in (-6250, -5150):
            faces = [p for p, n, _, _ in TRIANGLES
                     if n[1] == 0 and all(xx == wall_x for xx, _, _ in p)
                     and min(zz for _, _, zz in p) <= z <= max(zz for _, _, zz in p)]
            if wall_x == -6250:
                self.assertFalse(faces)
                continue
            self.assertTrue(faces)
            self.assertEqual(min(y for p in faces for _, y, _ in p), 0)
            self.assertEqual(max(y for p in faces for _, y, _ in p), 1500)

    def test_full_sculpture_rear_steps(self):
        for x,z,surface in [(-11100,7900,"SURFACE_DEFAULT"),
                            (-7600,7900,"SURFACE_SLIPPERY"),
                            (-4100,7900,"SURFACE_VERY_SLIPPERY"),
                            (-11100,12000,"SURFACE_NOT_SLIPPERY")]:
            self.assertEqual(floor(x,z+951),(0,"SURFACE_DEFAULT",0))
            self.assertEqual(floor(x,z+950),(40,surface,0))
            self.assertTrue(any(n[1]==0 and all(p[2]==z+950 for p in tri)
                                and min(p[1] for p in tri)==0 and max(p[1] for p in tri)==40
                                for tri,n,s,_ in TRIANGLES if s==surface))

    def test_cold_bounds_match_the_water_box(self):
        source=(ROOT/"src/game/physics_lab.c").read_text().split("s32 physics_lab_is_cold_pool",1)[1]
        bounds=[float(v) for v in re.findall(r"[xz] [<>] ([0-9.]+)f",source)]
        self.assertEqual(bounds,[3300,5700,11400,14300])

    def test_cap_trios_have_level_ground_and_headroom(self):
        script=(ROOT/"levels/physics_lab/script.c").read_text()
        supplies=re.findall(r"LAB_CAP_(ROW|COLUMN)\((-?\d+), (-?\d+), (-?\d+)\)",script)
        self.assertEqual(len(supplies),12)
        for direction,x,y,z in supplies:
            x,y,z=map(int,(x,y,z))
            for offset in (-400,0,400):
                xx,zz=(x+offset,z) if direction=="ROW" else (x,z+offset)
                self.assertAlmostEqual(floor(xx,zz)[0],y-220,msg=str((xx,y,zz)))
                # An elevated deck's underside is below the supply's floor;
                # only ceilings above that floor can obstruct its boxes.
                self.assertFalse(any(y-220<=h<y+200 for h,_,_ in heights_at(xx,zz,True)))
        # The additional impact supply has ground under its expanded shadows.
        for x in (10100,10500,10900):
            for dx,dz in [(-250,-250),(-250,250),(250,-250),(250,250)]:
                self.assertEqual(floor(x+dx,7450+dz),(0,"SURFACE_DEFAULT",0))

    def test_valid_triangles_and_bounds(self):
        self.assertTrue(all(any(n) for _,n,_,_ in TRIANGLES))
        self.assertTrue(all(abs(p[0])<=15500 and abs(p[2])<=15500 and -3600<=p[1]<=3600 for p in VERTICES))

    def test_no_accidental_holes_in_playground(self):
        for x in range(-15000,15001,600):
            for z in range(-15000,15001,600):
                self.assertGreater(floor(x,z)[0],-3600,(x,z))

    def test_spawn_and_runway(self):
        self.assertEqual(floor(0,400),(0,"SURFACE_DEFAULT",0))
        for x in range(-5900,6200,300):
            self.assertEqual(floor(x,750)[0],0)

    def test_four_matching_slope_lanes(self):
        for x,surface in [(-5750,"SURFACE_DEFAULT"),(-4650,"SURFACE_SLIPPERY"),
                          (-3550,"SURFACE_VERY_SLIPPERY"),(-2450,"SURFACE_NOT_SLIPPERY")]:
            for z,y in [(-1950,94),(-2650,390),(-3350,942),(-5000,1292)]:
                actual=floor(x,z)
                self.assertAlmostEqual(actual[0],y)
                self.assertEqual(actual[1],surface)

    def test_tower_walk_route(self):
        for i in range(6):
            z=-5400+i*600
            for x in range(3050,5600,100):
                fraction=(x-3000)/2600
                expected=500*(i+(fraction if i%2==0 else 1-fraction))
                self.assertAlmostEqual(floor(x,z)[0],expected)
                self.assertEqual(floor(x,z)[1],"SURFACE_NOT_SLIPPERY")
            if i<5:
                x=5850 if i%2==0 else 2750
                for z2 in range(-5550+i*600,-4600+i*600,100):
                    self.assertAlmostEqual(floor(x,z2)[0],500*(i+1))
        self.assertEqual(floor(2000,-2100)[0],3000)

    def test_tower_ground_passages_and_closed_decks(self):
        # Mario can cross beneath the launch deck and raised switchbacks.
        # Outer columns retain a clear corridor inside each turn landing.
        for lo,hi in [((1800,1,-3300),(2200,300,-1000)),
                      ((3250,1,-5050),(5350,300,-2100)),
                      ((2670,1,-5150),(2830,300,-2650)),
                      ((5770,1,-5750),(5930,300,-2100))]:
            self.assertEqual(static_intrusions(lo,hi),[],(lo,hi))
        for x,z,ceiling in [(2000,-2100,2840),(2750,-4500,900),(5850,-5000,400)]:
            self.assertIn((0,"SURFACE_DEFAULT",0),heights_at(x,z))
            self.assertIn((ceiling,"SURFACE_DEFAULT",0),heights_at(x,z,True))
        for i in range(1,6):
            z=-5400+i*600
            for x in range(3250,5351,350):
                top=floor(x,z)[0]
                self.assertTrue(any(abs(y-(top-100))<.001 for y,_,_ in heights_at(x,z,True)))

    def test_launch_pole_has_clear_climb_and_deck_exit(self):
        x,y,z,_=fixture_placement("bhvPoleGrabbing","320 << 16")
        self.assertEqual((x,y,z),(1020,0,-2100))
        self.assertEqual(floor(x,z),(0,"SURFACE_DEFAULT",0))
        # The climb clears the bowl, deck columns and underside. Its top
        # provides a short, unobstructed jump onto the original Y=3000 deck.
        self.assertEqual(static_intrusions((x-100,1,z-100),(x+100,3400,z+100)),[])
        self.assertEqual(static_intrusions((x,3001,z-100),(1500,3600,z+100)),[])
        self.assertEqual(floor(1400,z)[0],3000)

    def test_water_depths_and_current(self):
        for z,y in [(1400,-40),(1800,-80),(2200,-240),(3300,-800),(4400,-1500),(5900,-1800)]:
            self.assertAlmostEqual(floor(3000,z)[0],y)
        self.assertEqual(floor(5800,5400),(-1800,"SURFACE_FLOWING_WATER",0x100))
        self.assertEqual(floor(4000,5900)[1],"SURFACE_DEFAULT")
        text=(ROOT/"levels/physics_lab/collision.inc.c").read_text()
        self.assertIn("COL_WATER_BOX(0, 2400, 1000, 6400, 6000, -40)",text)

    def test_crawl_hanging_and_submerged_clearance(self):
        for x,z,y,surface in [(-1200,2000,140,"SURFACE_DEFAULT"),
                              (0,1800,330,"SURFACE_HANGABLE"),
                              (3300,5400,-1420,"SURFACE_DEFAULT")]:
            self.assertTrue(any(abs(h-y)<.001 and s==surface for h,s,_ in heights_at(x,z,True)))
        # The tunnel has a continuous bottom 380 units below its roof.
        self.assertIn((-1800,"SURFACE_DEFAULT",0),heights_at(3300,5400))

    def test_hazards_and_vanish_barriers(self):
        for x,surface in [(-5500,"SURFACE_BURNING"),(-3700,"SURFACE_SHALLOW_QUICKSAND"),
                          (-1700,"SURFACE_HORIZONTAL_WIND")]:
            self.assertEqual(floor(x,5000)[1],surface)
        self.assertEqual(floor(-1700,5000)[2],0x40)
        walls=[n for _,n,s,_ in TRIANGLES if s=="SURFACE_VANISH_CAP_WALLS"]
        self.assertTrue(any(n[0]>0 for n in walls))
        self.assertTrue(any(n[0]<0 for n in walls))

    def test_cannon_is_recessed_and_reachable(self):
        self.assertAlmostEqual(floor(-6400,0)[0],-262.5)
        self.assertAlmostEqual(floor(-6400,-299)[0],-.875)
        # Cannon idle hitbox spans -340 to -190: the approach reaches it.
        self.assertTrue(-340 < floor(-6400,0)[0] < -190)

    def test_banked_slide_circuit_and_jump(self):
        self.assertEqual(floor(-9000,-8200)[1],"SURFACE_VERY_SLIPPERY")
        self.assertGreater(floor(-9000,-8200)[0],floor(-4500,-8200)[0])
        # Inside/outside of both banked turns, and the clear 400-unit gap.
        for inner,outer in [(-2500,-1850),(-11000,-11600)]:
            self.assertGreater(floor(outer,-10000)[0],floor(inner,-10000)[0])
        self.assertEqual(floor(-6000,-11800)[0],0)
        self.assertEqual(floor(-5790,-11800)[1],"SURFACE_VERY_SLIPPERY")
        self.assertEqual(floor(-6210,-11800)[1],"SURFACE_VERY_SLIPPERY")
        # Walking access joins the elevated start at the same height.
        self.assertAlmostEqual(floor(-9300,-7750)[0],1400)
        self.assertAlmostEqual(floor(-9300,-5150)[0],0)

    def test_mixed_surface_sequence(self):
        expected=[(-4000,"SURFACE_DEFAULT",0),(-2000,"SURFACE_ICE",0),
                  (0,"SURFACE_NOT_SLIPPERY",0),(1800,"SURFACE_SHALLOW_MOVING_QUICKSAND",0x100),
                  (3500,"SURFACE_DEFAULT",0)]
        for z,surface,force in expected:
            self.assertEqual(floor(-12100,z)[1:],(surface,force))
        self.assertAlmostEqual(floor(-12100,0)[0],440)

    def test_water_terraces_currents_and_exit(self):
        for z,depth in [(1300,20),(1750,60),(2150,100),(2550,200),(2900,400)]:
            self.assertAlmostEqual(-40-floor(8200,z)[0],depth)
        self.assertEqual(floor(7000,5500),(-2000,"SURFACE_FLOWING_WATER",0x100))
        self.assertEqual(floor(9500,5500),(-2000,"SURFACE_FLOWING_WATER",0x180))
        for z in range(1100,6000,200):
            self.assertAlmostEqual(floor(11200,z)[0],-.4*(z-1000))
        text=(ROOT/"levels/physics_lab/collision.inc.c").read_text()
        self.assertTrue("COL_WATER_BOX_INIT(8)" in text)
        self.assertIn("COL_WATER_BOX(1, 6400, 1000, 11600, 6000, -40)",text)

    def test_pool_divider_has_solid_thickness_and_open_connection(self):
        for z in (2000,5000):
            self.assertEqual(floor(6520,z)[0],0)
            for x,sign in ((6400,-1),(6640,1)):
                self.assertTrue(any(all(p[0]==x for p in points) and n[0]*sign>0
                                    and min(p[2] for p in points)<=z<=max(p[2] for p in points)
                                    and min(p[1] for p in points)<=-300<max(p[1] for p in points)
                                    for points,n,_,_ in TRIANGLES))
        for x in (6401,6520,6639):
            self.assertLess(floor(x,3600)[0],-400)
        for points,n,_,_ in TRIANGLES:
            if n[0] and min(p[0] for p in points)>=6400 and max(p[0] for p in points)<=6640:
                if min(p[2] for p in points)<3600<max(p[2] for p in points):
                    self.assertLess(max(p[1] for p in points),-400)

    def test_underwater_roof_shaft_and_pool_connection(self):
        for z in (4500,4800,5200):
            expected=-1550+500*(z-4400)/900
            self.assertTrue(any(abs(y-expected)<.001 for y,_,_ in heights_at(8400,z,True)))
            self.assertIn((-2000,"SURFACE_DEFAULT",0),heights_at(8400,z))
        # Shaft has no ceiling above the swim path; its lower west entrance is
        # 700 tall, measured from the bottom to the lintel underside.
        self.assertEqual(heights_at(10400,5200,True),[])
        self.assertAlmostEqual(floor(10400,5200)[0],-2000)
        self.assertTrue(any(y==-1300 for y,_,_ in heights_at(10000,5200,True)))
        # No east/west-facing wall blocks the upper connecting opening.
        for points,n,_,_ in TRIANGLES:
            if all(p[0]==6400 for p in points) and n[0]!=0:
                if min(p[2] for p in points)<3600<max(p[2] for p in points):
                    self.assertLess(max(p[1] for p in points),-400)

    def test_precision_widths_and_clearances(self):
        for x,width in [(-1200,80),(-400,120),(500,180),(1400,260)]:
            self.assertEqual(floor(x+width/2-1,10000)[0],250)
            self.assertEqual(floor(x+width/2+1,10000)[0],0)
        for i,height in enumerate([100,125,150,175,200]):
            self.assertTrue(any(abs(y-height)<1e-6 for y,_,_ in heights_at(-1550+i*850,12500,True)))
        for i,gap in enumerate([100,140,180,240,320]):
            left=-1800+i*850
            self.assertEqual(floor(left+gap/2,14500)[0],0)
            self.assertAlmostEqual(floor(left-1,14500)[0],700)
            self.assertAlmostEqual(floor(left+gap+1,14500)[0],700)

    def test_flight_tunnel_and_uneven_terrain(self):
        for x in (4600,5500,6500,7400):
            bottom=1500-(x-4500)*.2
            self.assertTrue(any(abs(y-bottom)<.001 for y,_,_ in heights_at(x,-9800)))
            self.assertTrue(any(abs(y-bottom-900)<.001 for y,_,_ in heights_at(x,-9800,True)))
        row=[floor(-8700,-x-6000)[0] for x in range(-6100,-2099,200)]
        self.assertGreater(max(row)-min(row),150)
        self.assertEqual(floor(-9500,-1100)[0],0)
        self.assertEqual(floor(-8960,1300)[0],150)

    def test_static_lift_roof_and_bridge_docks(self):
        for x in (7900,8300,8700):
            for z in (-400,0,400):
                self.assertTrue(any(abs(y-950)<.001 and (surface,force)==("SURFACE_DEFAULT",0)
                                    for y,surface,force in heights_at(x,z,True)))
                self.assertAlmostEqual(floor(x,z)[0],1120)
        # The complete lift travel still clears every static support, and the
        # former west wall is open for a ground approach between the posts.
        self.assertEqual(static_intrusions((7900,40,-400),(8700,820,400)),[])
        self.assertEqual(static_intrusions((7400,1,-350),(7890,300,350)),[])
        for x in (7900,10700):
            self.assertEqual(floor(x,-2400)[0],350)

    def test_sculptures_repeat_geometry_with_real_surface_variants(self):
        variants=[(-11100,7900,"SURFACE_DEFAULT"),(-7600,7900,"SURFACE_SLIPPERY"),
                  (-4100,7900,"SURFACE_VERY_SLIPPERY"),(-11100,12000,"SURFACE_NOT_SLIPPERY")]
        first=None
        for cx,cz,surface in variants:
            sculpture=[(points,n,s) for points,n,s,_ in TRIANGLES
                       if any(p[1]>0 for p in points) and all(abs(p[0]-cx)<=930 and abs(p[2]-cz)<=950 for p in points)]
            self.assertEqual(len(sculpture),126)
            self.assertTrue(all(s==surface for _,_,s in sculpture))
            self.assertTrue(any(n[1]<0 for _,n,_ in sculpture))
            self.assertTrue(any(n[0] and n[1] and n[2] for _,n,_ in sculpture))
            signature=sorted(tuple((x-cx,y,z-cz) for x,y,z in points) for points,_,_ in sculpture)
            if first is None:
                first=signature
            else:
                self.assertEqual(signature,first)
            # Central undercut has less standing clearance than Mario's normal
            # height, yet remains reachable from the rising front approach.
            roof=min(y for y,_,_ in heights_at(cx,cz,True))
            ground=max(y for y,_,_ in heights_at(cx,cz) if y<roof)
            self.assertTrue(140<roof-ground<160,(cx,roof-ground))
            self.assertEqual(floor(cx-900,cz+800)[1],surface)


if __name__=="__main__":
    unittest.main()
