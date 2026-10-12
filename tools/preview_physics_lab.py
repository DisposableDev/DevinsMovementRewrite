#!/usr/bin/env python3
"""Optional Pillow/NumPy previews from source; not needed for ROM builds."""
import math
import re
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont
import numpy as np
import generate_physics_lab as lab

OUT=lab.ROOT/"docs"
FONT="/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"

ENEMY_SECTIONS = [
    ("Basic combat", (3900,-12000), "Tiny → regular → huge; matched", "Koopas on the southern row.", "#df9c59",
     ("bhvGoomba", "bhvFloomba", "bhvKoopa", "bhvPiranhaPlant", "bhvScuttlebug")),
    ("Grabbing and recovery", (9500,-8000), "Chuckya and Heave-Ho; west", "approach, long east throw strip.", "#eec96b",
     ("bhvChuckya", "bhvHeaveHo")),
    ("Heavy contacts", (12400,-11800), "Whomp → Thwomp → Grindel", "beside the retained Tox sweep.", "#d66b62",
     ("bhvSmallWhomp", "bhvThwomp", "bhvGrindel", "bhvToxBox")),
    ("Burrowing targets", (13050,-9500), "Three usable Mole holes, on", "flat ground east of the landing.", "#b8a276",
     ("bhvMontyMoleHole", "bhvMontyMole")),
    ("Air targets", (-12200,-10400), "Lakitu/Spiny spawning floor;", "Fly Guy and Klepto farther south.", "#92c7ef",
     ("bhvEnemyLakitu", "bhvSpiny", "bhvFlyGuy", "bhvKlepto")),
    ("Wind forces", (-10800,-2600), "Tweester and northward Fwoosh;", "separate marked approaches.", "#c2a7e8",
     ("bhvTweester", "bhvCloud")),
    ("Matched knockback", (-10800,3000), "Bully / Chill Bully: identical dry", "courts and southern entrances.", "#f0b090",
     ("bhvSmallBully", "bhvSmallChillBully")),
    ("Haunted court", (-13800,10900), "Piano, chair, Boo and books;", "Swoop under its existing roof.", "#bfd7b8",
     ("bhvSwoop", "bhvBoo", "bhvHauntedChair", "bhvMadPiano", "bhvBookendSpawn")),
    ("Shock contacts", (-13600,13400), "Homing/circling Amps; rings show", "1500-unit chase thresholds.", "#eee48a",
     ("bhvHomingAmp", "bhvCirclingAmp")),
    ("Projectiles and fire", (-5450,12800), "Bill/bomb lanes plus five bays;", "side corridor and tall backstops.", "#eb927b",
     ("bhvBulletBill", "bhvWaterBombSpawner", "bhvBobomb", "bhvSnufit", "bhvFirePiranhaPlant",
      "bhvFlamethrower", "bhvBouncingFireball")),
    ("Water and cold contacts", (4500,13000), "Retained water Amps, chest pairs,", "Spindrift and Mr. Blizzard.", "#a1e4e1",
     ("bhvPhysicsLabChests", "bhvSpindrift", "bhvMrBlizzard")),
    ("Aquatic habitat", (9600,12600), "Shark, eel, Bub, Bubba, clam", "and Skeeter keep their basin.", "#74bde7",
     ("bhvSushiShark", "bhvUnagi", "bhvBubSpawner", "bhvBubba", "bhvClamShell", "bhvSkeeter")),
    ("Retained rolling lanes", (-13800,6200), "Spindel, both Tox sweeps and", "the southeast boulder enclosure.", "#d66b62",
     ("bhvSpindel", "bhvBigBoulder")),
    ("Individual response tests", (-11100,-6100), "Mr. I ring, Chain Chomp, Pokey,", "Moneybag disguise and cap Ukiki.", "#d0b8a5",
     ("bhvMrI", "bhvChainChomp", "bhvPokey", "bhvMoneybagHidden", "bhvUkiki")),
]


def font(size):
    return ImageFont.truetype(FONT,size)


def overview(mesh, enemy_focus=False):
    im=Image.new("RGB",(2060,1670),"#121c29")
    d=ImageDraw.Draw(im)
    title = "ENEMY TEST GROUPS" if enemy_focus else "CONNECTED PLAYGROUND"
    d.text((65,25),"PHYSICS LAB  /  " + title,font=font(40),fill="#edf3f8")
    subtitle = "North is up • Native behaviors • Entry → approach → target → recovery" if enemy_focus else "North is up • Shared walking routes • Tests grouped by purpose"
    d.text((68,80),subtitle,font=font(22),fill="#9fbbcb")
    project=lambda x,z:(round(60+(x+15500)/31000*1440),round(140+(z+15500)/31000*1440))
    d.rectangle((60,140,1500,1580),fill="#253342")
    opaque=[]
    for f,tri in mesh.triangles():
        if f.layer not in ("hidden","collision_only") and lab.normal(tri)[1]>0:
            opaque.append((f,tri))
    for index,cx,cz,scale in lab.SMALL_SCULPTURES:
        ox,oz=lab.SCULPTURES[index][:2]
        for f,tri in mesh.triangles():
            if f.layer==f"sculpture_{index}" and lab.normal(tri)[1]>0:
                opaque.append((f,tuple((-(x-ox)*scale+cx,y*scale,-(z-oz)*scale+cz) for x,y,z in tri)))
    for f,tri in sorted(opaque,key=lambda pair:sum(v[1] for v in pair[1])):
        color=f.color if f.layer=="water" else lab.shaded_color(tri,f.color)
        d.polygon([project(v[0],v[2]) for v in tri],fill=color)
    script=(lab.ROOT/"levels/physics_lab/script.c").read_text()
    for x,y,z,kind in re.findall(r"OBJECT\([^,]+, (-?\d+), (-?\d+), (-?\d+), 0, 0, 0, LAB_MOVER_(\w+) << 16, bhvPhysicsLabMover",script):
        x,y,z=map(int,(x,y,z))
        if kind=="DISK":
            d.ellipse((*project(x-650,z-650),*project(x+650,z+650)),fill=lab.COLORS["platform"],outline="#6b512e",width=2)
            continue
        w,h=(2200,500) if kind=="BRIDGE" else (800,800)
        if kind.startswith("WALL_"):
            w,h=100,1000
            x+=-600 if kind=="WALL_LEFT" else 600
        d.rectangle((*project(x-w/2,z-h/2),*project(x+w/2,z+h/2)),fill=lab.COLORS["platform"],outline="#4b3c2b",width=2)
        if kind=="SHUTTLE":
            d.line([project(x-900,z),project(x+900,z)],fill="#663813",width=3)
    for x,y,z in re.findall(r"OBJECT\([^,]+, (-?\d+), (-?\d+), (-?\d+), 0, 0, 0, 0, bhvPhysicsLabTiltingPlatform",script):
        x,z=int(x),int(z)
        d.rectangle((*project(x-450,z-450),*project(x+450,z+450)),fill=lab.COLORS["platform"],outline="#4b3c2b",width=2)
    for x,y,z in re.findall(r"OBJECT\(MODEL_CHECKERBOARD_PLATFORM, (-?\d+), (-?\d+), (-?\d+),",script):
        for offset in (-235,235):
            cx,cz=int(x)+offset,int(z)
            d.rectangle((*project(cx-184,cz-307),*project(cx+184,cz+307)),fill="#d4c6a7",outline="#4b3c2b",width=2)
    # Object locations are read from the shipped level scripts, so the overview
    # reveals enemy clustering and supply placement as well as static terrain.
    enemies=(lab.ROOT/"levels/physics_lab/enemies.inc.c").read_text()
    colors = {behavior: color for _,_,_,_,color,behaviors in ENEMY_SECTIONS for behavior in behaviors}
    for x,y,z,behavior in re.findall(r"OBJECT\([^,]+, (-?\d+), (-?\d+), (-?\d+), [^\n]+, (bhv\w+)\)",enemies):
        x,z=int(x),int(z)
        cx,cy=project(x,z)
        color = colors.get(behavior,"#ba4942") if enemy_focus else "#ba4942"
        if enemy_focus and behavior == "bhvCirclingAmp" and x > 0:
            color = "#a1e4e1"
        if enemy_focus and behavior == "bhvHomingAmp":
            d.ellipse((*project(x-1500,z-1500),*project(x+1500,z+1500)),outline=color,width=2)
        radius = 5 if enemy_focus else 3
        d.ellipse((cx-radius,cy-radius,cx+radius,cy+radius),fill=color,outline="#502b30")
    if enemy_focus:
        for x,z,yaw in lab.ENEMY_ENTRIES:
            angle = math.radians(yaw)
            end = project(x+400*math.sin(angle),z-400*math.cos(angle))
            d.line([project(x,z),end],fill="#f3f6eb",width=2)
            d.ellipse((end[0]-2,end[1]-2,end[0]+2,end[1]+2),fill="#f3f6eb")
    for x,y,z in re.findall(r"LAB_CAP_(?:ROW|COLUMN)\((-?\d+), (-?\d+), (-?\d+)\)",script):
        cx,cy=project(int(x),int(z))
        d.rectangle((cx-5,cy-5,cx+5,cy+5),fill="#f3d779",outline="#534920")
    sections=[
        ("Banked slide circuit",(-6800,-9850),"Banked turns, crest, jump gap","and uphill return."),
        ("Moving solids",(9500,-3400),"Shuttle, disk, rocking bridge,","low-roof lift and closing walls."),
        ("Main water tests",(8400,4000),"Depth terraces, opposing currents,","tunnel, sloping roof and shaft."),
        ("Matched slopes",(-4000,-3000),"Four equal compound ramps","with different surface properties."),
        ("Precision court",(0,10800),"Beams, low roofs, tight passages,","jump gaps and shrinking pads."),
        ("Mixed-surface route",(-12100,0),"Normal, ice, uphill grip, moving","sand and a normal finish."),
        ("Wing Cap course",(6000,-9600),"High launch, flight arches, tunnel","and descending landing terraces."),
        ("Uneven terrain",(-8700,-1800),"Ridges, humps, diagonal slopes","and closely spaced small steps."),
        ("Comparison garden",(-11100,12000),"Four surface groups, each with","large, half and quarter statues."),
        ("Momentum lanes",(1500,-14000),"Three long coasting lanes; rolling","hills and braking on the west."),
        ("Shallow and cold water",(4500,11000),"Separate pools with walking exits;","poison gas beside the west hazards."),
        ("Aquatic enemy basin",(9600,12800),"Open water for shark, eel, Bub,","Bubba, clam and Skeeter."),
        ("BLJ activation",(14500,1000),"Steps and long runout; contained","boulder lane beyond the south exit."),
        ("Enemy courts",(3500,-11800),"Ordered combat/heavy rows;","matched contact and firing bays."),
        ("A / Seam comparison",(-7550,12800),"Control, triangulation, 8-unit lip","and a normal-to-ice boundary."),
        ("B / Seam replica",(-7925,-10175),"Identical translated specimens;","enter through the circuit gap."),
        ("C / Matched impact beds",(9600,8750),"Dry, shallow and deep water;","two pole-access release decks."),
        ("D / Projectile bay",(-4500,13700),"Separate Bill and bomb lanes;","neighboring shot/fire bays."),
        ("E / Animation marker",(-1325,3575),"Fixed viewing mark beside the","tree, pole and landing tests."),
    ]
    if enemy_focus:
        sections = [(title,position,line1,line2) for title,position,line1,line2,_,_ in ENEMY_SECTIONS]
    for i,(title,(x,z),line1,line2) in enumerate(sections):
        n=f"{i+1:02}"
        cx,cy=project(x,z)
        d.ellipse((cx-21,cy-21,cx+21,cy+21),fill="#17202f",outline="#f2f6ff",width=2)
        d.text((cx,cy-1),n,font=font(19),fill="white",anchor="mm")
        y=140+i*72
        d.rounded_rectangle((1540,y,1995,y+68),radius=10,fill="#1f2d3c")
        number_color = ENEMY_SECTIONS[i][4] if enemy_focus else "#e7b576"
        d.text((1560,y+3),n,font=font(21),fill=number_color)
        d.text((1610,y+3),title,font=font(21),fill="#f0f4fa")
        d.text((1560,y+28),line1,font=font(18),fill="#b7ccda")
        d.text((1560,y+48),line2,font=font(18),fill="#b7ccda")
    cx,cy=project(0,400)
    d.ellipse((cx-9,cy-9,cx+9,cy+9),fill="white",outline="#18273a",width=2)
    d.text((cx+13,cy-10),"START",font=font(17),fill="#192c3b")
    d.text((1545,1510),"Pause → Exit Course",font=font(25),fill="#f5d3a0")
    d.text((1545,1554),"Returns to the castle entrance.",font=font(20),fill="#b7ccda")
    caption = "Source placements and geometry. Airborne pursuit, throws, child spawning and camera need playtesting." if enemy_focus else "Source overview. Red dots: enemies and hazards. Gold squares: complete cap trios."
    d.text((65,1600),caption,font=font(21),fill="#9fbbcb")
    im.save(OUT/("physics-lab-enemy-layout.png" if enemy_focus else "physics-lab-overview.png"))


def sculptures(mesh):
    im=Image.new("RGB",(1800,1530),"#121c29")
    d=ImageDraw.Draw(im)
    d.text((55,30),"THE SCULPTURE GARDEN",font=font(42),fill="#edf3f8")
    d.text((55,90),"Identical bad clearances. Six different surface properties.",font=font(24),fill="#adc4d6")
    labels=["Normal","Slippery","Very slippery","Grippy","Ice","Vanish Cap walls"]
    # Orthographic source view from the front-right, showing the central cleft.
    view=(.52,.38,.765)
    length=math.sqrt(sum(v*v for v in view))
    view=tuple(v/length for v in view)
    right=(view[2],0,-view[0])
    length=math.hypot(right[0],right[2])
    right=tuple(v/length for v in right)
    up=(view[1]*right[2],view[2]*right[0]-view[0]*right[2],-view[1]*right[0])
    dot=lambda a,b:sum(x*y for x,y in zip(a,b))
    for i,(cx,cz,_,rgb) in enumerate(lab.SCULPTURES):
        col=i%2
        row=i//2
        ox=50+col*875
        oy=155+row*420
        d.rounded_rectangle((ox,oy,ox+825,oy+398),radius=12,fill="#223143")
        d.text((ox+26,oy+18),labels[i],font=font(26),fill=rgb)
        faces=[]
        for f,tri in mesh.triangles():
            if f.layer!=f"sculpture_{i}": continue
            local=[(x-cx,y,z-cz) for x,y,z in tri]
            n=lab.normal(local)
            if dot(n,view)<=0: continue
            faces.append((f,local))
        pixels=np.empty((340,825,3),dtype=np.uint8)
        pixels[:]=(34,49,67)
        depth=np.full((340,825),-np.inf)
        for f,tri in faces:
            pts=[(410+dot(p,right)*.14,265-dot(p,up)*.14,dot(p,view)) for p in tri]
            color=lab.shaded_color(tri,f.color)
            a,b,c=pts
            x0=max(0,math.floor(min(p[0] for p in pts)))
            x1=min(825,math.ceil(max(p[0] for p in pts)))
            y0=max(0,math.floor(min(p[1] for p in pts)))
            y1=min(340,math.ceil(max(p[1] for p in pts)))
            den=(b[1]-c[1])*(a[0]-c[0])+(c[0]-b[0])*(a[1]-c[1])
            if abs(den)<1e-8 or x0>=x1 or y0>=y1: continue
            yy,xx=np.mgrid[y0:y1,x0:x1]
            xx=xx+.5
            yy=yy+.5
            u=((b[1]-c[1])*(xx-c[0])+(c[0]-b[0])*(yy-c[1]))/den
            v=((c[1]-a[1])*(xx-c[0])+(a[0]-c[0])*(yy-c[1]))/den
            w=1-u-v
            z=u*a[2]+v*b[2]+w*c[2]
            visible=(u>=0)&(v>=0)&(w>=0)&(z>depth[y0:y1,x0:x1])
            depth[y0:y1,x0:x1][visible]=z[visible]
            pixels[y0:y1,x0:x1][visible]=color
        im.paste(Image.fromarray(pixels),(ox,oy+50))
        # Scale marker is the normal 160-unit standing collision height.
        x,y=ox+745,oy+340
        h=160*.14
        d.line((x,y,x,y-h),fill="#dfe8ef",width=3)
        d.line((x-7,y,x+7,y),fill="#dfe8ef",width=2)
        d.line((x-7,y-h,x+7,y-h),fill="#dfe8ef",width=2)
        d.text((ox+625,oy+356),"Mario height",font=font(17),fill="#b5c9da")
    d.text((55,1445),"Narrowing clefts • Inward-leaning fins • Low undercuts • Triangular seams",font=font(24),fill="#d5e1eb")
    d.text((55,1485),"Source geometry preview. Actual sticking and escape behavior needs playtesting.",font=font(20),fill="#91abba")
    im.save(OUT/"physics-lab-sculptures.png")


if __name__=="__main__":
    OUT.mkdir(exist_ok=True)
    mesh=lab.build_terrain()
    overview(mesh)
    overview(mesh, enemy_focus=True)
    sculptures(mesh)
    print("Wrote Physics Lab overview, enemy layout and sculpture previews.")
