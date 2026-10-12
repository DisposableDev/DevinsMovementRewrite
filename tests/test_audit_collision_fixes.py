"""Fork-audit regressions: native queries/actions against current authored geometry.

The host fixture supplies rendering, animation, gravity and camera services; it does
not run the ROM, native terrain loader or controller routes in an emulator.
"""
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

from regression_support import Source
from run_regressions import prepare

ROOT = Path(__file__).resolve().parents[1]


def geometry(path, name):
    text = (ROOT / path).read_text()
    vertices = re.findall(r'COL_VERTEX\(([^)]+)\)', text)
    faces = []
    surface = None
    for match in re.finditer(r'COL_TRI_INIT\(([^,]+),[^)]+\)|COL_TRI\(([^)]+)\)', text):
        if match[1]:
            surface = match[1]
        else:
            faces.append('{' + match[2] + ',' + surface + '}')
    return ('static const Vec3s ' + name + 'Vertices[]={'
            + ','.join('{' + vertex + '}' for vertex in vertices) + '};\n'
            + 'static const struct {int a,b,c,type;} ' + name + 'Faces[]={' + ','.join(faces) + '};\n'
            + 'static void load_' + name + '(void) { reset(); surfaceCount=nodeCount=0;'
            + 'memset(gStaticSurfacePartition,0,sizeof(gStaticSurfacePartition));'
            + 'for(unsigned i=0;i<ARRAY_COUNT(' + name + 'Faces);i++) {'
            + 'triangle((s16*)' + name + 'Vertices[' + name + 'Faces[i].a],'
            + '(s16*)' + name + 'Vertices[' + name + 'Faces[i].b],'
            + '(s16*)' + name + 'Vertices[' + name + 'Faces[i].c],0,' + name + 'Faces[i].type);}}\n')


class AuditCollisionFixTests(unittest.TestCase):
    def test_native_contact_timing_normals_support_and_finale_bounds(self):
        with tempfile.TemporaryDirectory(prefix='audit-collision-fixes-') as temporary:
            folder = Path(temporary)
            prepare(ROOT, 'swimming', folder, {})
            step = Source(ROOT / 'src/game/mario_step.c')
            definitions = (folder / 'swimming_definitions.inc').read_text()
            definitions += Source(ROOT / 'include/config/config_movement.h').macros('MARIO_GROUND_AIR_COLLISION_.*')
            definitions += Source(ROOT / 'include/sm64.h').macros('COS25')
            (folder / 'swimming_definitions.inc').write_text(definitions)
            sources = (folder / 'swimming_sources.inc').read_text()
            sources += '\n'.join(step.function(name) for name in (
                'get_mario_ground_air_collision_steps', 'perform_air_step_internal',
                'perform_slide_kick_air_step', 'perform_air_step_with_fraction'))
            sources += Source(ROOT / 'src/game/mario_actions_cutscene.c').function('act_debug_free_move')
            sources += Source(ROOT / 'src/game/mario_actions_moving.c').function('check_ledge_climb_down_internal')
            (folder / 'swimming_sources.inc').write_text(sources)
            fixture = (ROOT / 'tests/swimming_collision_test.c').read_text()
            fixture = fixture.replace('int main(int argc, char **argv)', 'int inherited_main(int argc, char **argv)')
            fixture = fixture.replace('struct MarioState {',
                'struct Camera {int mode,defMode;}; struct Area {struct Camera *camera;};\n'
                'struct MarioState { struct Area *area; f32 intendedMag; s16 intendedYaw,health; u8 hurtCounter; '
                'int squishTimer,fastMetalFallSquish,terrainSoundAddend,windCatchActive; '
                'u8 airTwirlUsed,airTwirlWingControlTimer,airTwirlWingResetUsed,wallKickChainCount;')
            fixture = fixture.replace('            Vec3f pos;', '            Vec3f pos, scale;')
            fixture = fixture.replace('u16 buttonDown;', 'u16 buttonDown,buttonPressed;')
            fixture = fixture.replace('|| m->action == ACT_GROUND_POUND);',
                                      '|| m->action == ACT_GROUND_POUND || m->action == ACT_SLIDE_KICK || m->action == ACT_RIDING_SHELL_JUMP);')
            fixture = fixture.replace('assert(floor == NULL || floor->type != SURFACE_VERTICAL_WIND);',
                'if(m->vel[1]>=0 || next[1]>=500) return; assert(floor == NULL || floor->type != SURFACE_VERTICAL_WIND);')
            support = r'''
#define CAMERA_MODE_8_DIRECTIONS 14
#define B_BUTTON 0x4000
#define Z_TRIG 0x2000
#define U_JPAD 0x0800
#define D_JPAD 0x0400
#define MARIO_ANIM_A_POSE 195
#define MARIO_ANIM_CLIMB_DOWN_LEDGE 48
static struct Controller *gPlayer1Controller=&controller;
static int freeMoveAllowed=TRUE;
static int game_options_free_move_active(void) {return freeMoveAllowed;}
static void vec3_zero(Vec3f v) {vec3f_set(v,0,0,0);}
static void set_camera_mode(struct Camera *camera,int mode,int frames) {camera->mode=mode;}
static int set_mario_action(struct MarioState *m,u32 action,u32 arg) {m->action=action;return TRUE;}
static void set_mario_animation(struct MarioState *m,int animation) {}
static f32 mario_get_movement_time_scale(struct MarioState *m) {return 1;}
static void limit_metal_water_fall_speed(struct MarioState *m,f32 timeScale) {}
static void mario_he_thicc_contact_shake(struct MarioState *m) {}
static int mario_get_terrain_sound_addend(struct MarioState *m) {return 0;}
static void apply_gravity(struct MarioState *m) {m->vel[1]-=2.4f;}
static void apply_vertical_wind(struct MarioState *m,f32 old) {}
static void castle_secret_glass_check_air(struct MarioState *m) {}
'''
            fixture = fixture.replace('#include "swimming_sources.inc"', support + '\n#include "swimming_sources.inc"')
            tests = r'''
#include <assert.h>
int main(void) {
    reset(); mario.flags=MARIO_WING_CAP; mario.forwardVel=48;
    mario.action=ACT_HE_THICC_BELLY_SLIDE;
    assert(!mario_water_walk_conditions_met(&mario,0,20000));
    mario.action=ACT_HE_THICC_WAVEDASH;
    assert(!mario_water_walk_conditions_met(&mario,0,20000));
    mario.action=ACT_WALKING;
    assert(mario_water_walk_conditions_met(&mario,0,20000));

    // The first struck ramp remains the rebound plane even after the floor changes.
    reset(); struct Surface *struck=triangle((Vec3s){0,0,-100},(Vec3s){0,0,100},
                                             (Vec3s){20,20,0},0,SURFACE_DEFAULT);
    vec3f_set(mario.pos,-5,5,0); vec3f_set(mario.vel,64,-16,0);
    mario.forwardVel=64; mario.faceAngle[0]=-0x0A00; mario.faceAngle[1]=0x4000;
    sSwimming.rollFrame=1;
    assert(perform_water_step(&mario)==WATER_STEP_HIT_FLOOR);
    assert(mario.floor!=struck);
    assert(fabsf(sSwimming.rollFloorNormal[0]-struck->normal.x)<.0001f);
    assert(fabsf(sSwimming.rollFloorNormal[1]-struck->normal.y)<.0001f);

    // Actual THI support: its synthetic water vertices must not lift the sweep to Y=0.
    load_thi(); enemyWater=mario.waterLevel=-3071;
    mario.action=ACT_RIDING_SHELL_JUMP; mario.flags=0;
    mario.floor=&gWaterSurfacePseudoFloor; mario.floorHeight=-3071;
    gWaterSurfacePseudoFloor.originOffset=3071;
    vec3f_set(mario.pos,-2812.4402f,-3071,4574.3999f);
    vec3f_set(mario.vel,0,50,32); mario.forwardVel=32;
    Vec3f next={mario.pos[0],-3058.5f,mario.pos[2]+8};
    assert(perform_air_quarter_step(&mario,next,0,TRUE)!=AIR_STEP_LANDED);
    assert(fabsf(mario.pos[1]+3058.5f)<.02f);

    // The authored ridge is reached 1/32 of a frame into a64-unit horizontal step.
    load_thi(); mario.action=ACT_SLIDE_KICK; enemyWater=mario.waterLevel=-3071;
    vec3f_set(mario.pos,-4599,718,-1995.75f); mario.faceAngle[1]=-0x4000;
    mario.floorHeight=find_floor(mario.pos[0],mario.pos[1],mario.pos[2],&mario.floor);
    mario_set_forward_vel(&mario,64); mario.vel[1]=-32;
    f32 remaining,water;
    assert(perform_slide_kick_air_step(&mario,&remaining,FALSE,&water)==AIR_STEP_LANDED);
    assert(fabsf(remaining-.96875f)<.0001f);
    mario.vel[1]*=-.5f;
    assert(perform_air_step_with_fraction(&mario,0,remaining)==AIR_STEP_NONE);
    assert(fabsf(mario.pos[0]+4663)<.02f);
    assert(mario.pos[1]>733);

    // Eligible deep water stops descent at contact and retains the rest of the frame.
    load_thi(); enemyWater=mario.waterLevel=-3071; mario.action=ACT_SLIDE_KICK;
    vec3f_set(mario.pos,-8000,-3067,-7800); vec3f_set(mario.vel,0,-32,64);
    mario.forwardVel=64; mario.faceAngle[1]=0;
    mario.floorHeight=find_floor(mario.pos[0],mario.pos[1],mario.pos[2],&mario.floor);
    assert(perform_slide_kick_air_step(&mario,&remaining,TRUE,&water)==AIR_STEP_NONE);
    assert(water==-3071 && fabsf(remaining-.875f)<.0001f);
    mario.vel[1]*=-.5f;
    assert(perform_air_step_with_fraction(&mario,0,remaining)==AIR_STEP_NONE);
    assert(mario.pos[1]>-3071 && fabsf(mario.pos[2]+7736)<.02f);

    // The audited THI ledge lies20 units beyond the last supported slide footprint.
    load_thi(); mario.action=ACT_CROUCH_SLIDE; mario.flags=0;
    vec3f_set(mario.pos,-4587,717,-1995.75f); vec3f_set(mario.vel,-100,0,0);
    mario.forwardVel=100; mario.faceAngle[1]=-0x4000;
    mario.floorHeight=find_floor(mario.pos[0],mario.pos[1],mario.pos[2],&mario.floor);
    struct MarioState before=mario;
    assert(!check_ledge_climb_down_internal(&mario,TRUE,FALSE));
    mario=before;
    assert(check_ledge_climb_down_internal(&mario,TRUE,TRUE));
    assert(mario.action==ACT_LEDGE_CLIMB_DOWN && mario.floor!=NULL);

    // Native debug flight sees the authored ceiling below its20,000 query sentinel.
    load_octagon(); struct Camera camera={14,14}; struct Area area={&camera}; mario.area=&area;
    struct Surface *ceil; assert(find_ceil(-1023,-197,717,&ceil)==19900 && ceil!=NULL);
    vec3f_set(mario.pos,-1023,-200,717); mario.action=ACT_DEBUG_FREE_MOVE;
    controller.buttonDown=B_BUTTON|U_JPAD;
    for(int frame=0;frame<640;frame++) {
        mario.floorHeight=find_floor(mario.pos[0],mario.pos[1],mario.pos[2],&mario.floor);
        act_debug_free_move(&mario); assert(mario.pos[1]<=19740);
    }
    assert(mario.pos[1]==19740);
    const int inside[8]={36,35,41,46,51,56,61,66};
    const int outside[8]={30,31,39,44,49,54,59,64};
    for(int high=0;high<2;high++) for(int corner=0;corner<8;corner++) {
        load_octagon(); mario.area=&area;
        const s16 *v=octagonVertices[inside[corner]];
        f32 dx=v[0]+1023,dz=v[2]-717,length=sqrtf(dx*dx+dz*dz); dx/=length;dz/=length;
        vec3f_set(mario.pos,v[0]-100*dx,high?19740:100,v[2]-100*dz);
        mario.action=ACT_DEBUG_FREE_MOVE;mario.intendedMag=32;mario.intendedYaw=atan2s(dz,dx);
        controller.buttonDown=B_BUTTON|(high?U_JPAD:0);
        for(int frame=0;frame<32;frame++) {
            mario.floorHeight=find_floor(mario.pos[0],mario.pos[1],mario.pos[2],&mario.floor);
            object.header.gfx.node.flags = GRAPH_RENDER_INVISIBLE | GRAPH_RENDER_BILLBOARD;
            act_debug_free_move(&mario); assert(mario.pos[1]<19900);
            assert(object.header.gfx.node.flags == (GRAPH_RENDER_ACTIVE | GRAPH_RENDER_BILLBOARD));
            for(int edge=0;edge<8;edge++) {
                const s16 *a=octagonVertices[outside[edge]],*b=octagonVertices[outside[(edge+1)%8]];
                f32 nx=b[2]-a[2],nz=a[0]-b[0];
                f32 sign=nx*(-1023-a[0])+nz*(717-a[2])>=0?1:-1;
                f32 distance=sign*(nx*(mario.pos[0]-a[0])+nz*(mario.pos[2]-a[2]))/sqrtf(nx*nx+nz*nz);
                assert(distance>=-.05f);
            }
        }
    }
    // Revocation exits through the ordinary water/floor/air recovery paths without
    // moving, healing, or injecting the manual exit's upward boost.
    freeMoveAllowed=FALSE;
    controller.buttonDown=U_JPAD|B_BUTTON;
    controller.buttonPressed=L_TRIG;
    mario.intendedMag=32; mario.intendedYaw=0;
    for(int destination=0;destination<3;destination++) {
        mario.action=ACT_DEBUG_FREE_MOVE; mario.health=0x500; mario.hurtCounter=3;
        mario.pos[0]=100; mario.pos[1]=200; mario.pos[2]=300;
        mario.floorHeight=destination==1?200:0;
        mario.waterLevel=destination==0?400:-1000;
        mario.forwardVel=50; vec3f_set(mario.vel,10,20,30);
        camera.mode=14;camera.defMode=1;
        assert(act_debug_free_move(&mario));
        assert(mario.action==(destination==0?ACT_WATER_IDLE:destination==1?ACT_IDLE:ACT_FREEFALL));
        assert(mario.pos[0]==100 && mario.pos[1]==200 && mario.pos[2]==300);
        assert(mario.health==0x500 && mario.hurtCounter==3);
        assert(mario.forwardVel==0 && mario.vel[0]==0 && mario.vel[1]==0 && mario.vel[2]==0);
        assert(camera.mode==1);
    }
    return 0;
}
'''
            code = fixture + geometry('levels/thi/areas/1/collision.inc.c', 'thi')
            code += geometry('levels/castle_inside/areas/4/collision.inc.c', 'octagon') + tests
            (folder / 'probe.c').write_text(code)
            result = subprocess.run(['cc','-std=c99','-O2','-Wall','-Wextra','-Werror',
                '-Wno-unused-parameter','-Wno-unused-variable','-Wno-unused-function',
                '-I',str(folder),'-I',str(ROOT/'tests'),str(folder/'probe.c'),'-o',str(folder/'probe'),'-lm'],
                capture_output=True,text=True,timeout=30)
            self.assertEqual(result.returncode,0,result.stderr)
            result = subprocess.run([str(folder/'probe')],capture_output=True,text=True,timeout=15)
            self.assertEqual(result.returncode,0,result.stderr)


if __name__=='__main__':
    unittest.main()
