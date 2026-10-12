#!/usr/bin/env python3
"""Optional particle-policy host tests. No ROM build; engine services are doubles."""
import argparse
from pathlib import Path
import os
import re
import shlex
import subprocess
import tempfile
from regression_support import Source, HarnessError

ROOT = Path(__file__).resolve().parents[1]


def prepare(directory, overrides):
    def src(path):
        return Source(ROOT / path, overrides.get(path))
    types = src('include/types.h')
    definitions = [src('src/game/object_list_processor.h').macros('OBJECT_POOL_CAPACITY'),
                   types.macros('MAX_OBJECT_FIELDS')]
    for name in ('GraphNode', 'AnimInfo', 'GraphNodeObject', 'ObjectNode', 'Object'):
        definitions.append(types.declaration('struct', name))
    for path, name in [('src/game/object_list_processor.h', 'ObjectList'),
                       ('src/game/object_list_processor.h', 'gTimeStopStateFlags'),
                       ('include/object_constants.h', 'ObjActiveFlags'),
                       ('include/object_constants.h', 'RespawnInfoType'),
                       ('include/object_constants.h', 'ActiveParticleFlags'),
                       ('src/engine/graph_node.h', 'GraphRenderFlags'),
                       ('src/engine/behavior_script.h', 'BhvProc'),
                       ('src/game/interaction.h', 'ObjInteractTypes'),
                       ('src/game/particle_budget.h', 'ParticlePriority')]:
        definitions.append(src(path).declaration('enum', name))
    definitions += [src('src/game/particle_budget.h').macros('PARTICLE_.*RESERVE'),
                    src('include/object_constants.h').macros('WHITE_PUFF_EXPLODE_BP_.*'),
                    src('include/model_ids.h').macros('MODEL_.*'),
                    src('src/engine/surface_collision.h').macros('FLOOR_LOWER_LIMIT(_MISC)?'),
                    src('src/engine/behavior_script.c').macros('BHV_CMD_GET_.*')]
    for name in ('WaterDropletParams','SpawnParticlesInfo'):
        definitions.append(src('src/game/object_helpers.h').declaration('struct', name))
    definitions.append(src('src/game/object_helpers.h').declaration('enum','WaterDropletFlags'))
    liquid = src('src/game/liquid_particles.h')
    definitions += [liquid.declaration('enum', name) for name in
                    ('LiquidParticleMaterial', 'WaterDropletAction')]
    definitions.append(liquid.macros('LIQUID_PARTICLE_.*|SLIDE_KICK_LIQUID_DROPLET_COUNT'))
    behavior_data = src('data/behavior_data.c').text
    policy = src('src/game/particle_budget.c').text
    names = sorted(set(re.findall(r'\bbhv\w+', policy)) | {
        'bhvMario', 'bhvGoomba', 'bhvExplosion', 'bhvKoopaShellFlame', 'bhvWaterAirBubble',
        'bhvBubbleParticleSpawner', 'bhvIdleWaterWave', 'bhvSparkleSpawn',
        'bhvStrongWindParticle', 'bhvWaterSplash', 'bhvShallowWaterSplash', 'bhvShallowWaterWave',
        'bhvVanishWaterRingSpawner', 'bhvPlungeBubble', 'bhvWaterEntryBubble'})
    for name in names:
        match = re.search(r'const BehaviorScript '+name+r'\[\]\s*=\s*\{\s*BEGIN\((\w+)\)',behavior_data)
        if not match:
            raise HarnessError('Missing reviewed behavior entry: '+name)
        definitions.append(f'static const BehaviorScript {name}[] = {{ ((uintptr_t){match[1]}) << 16 }};')
    (directory/'particle_definitions.inc').write_text('\n'.join(definitions))
    # Compile the complete production policy implementation, removing includes only.
    (directory/'particle_policy.inc').write_text(re.sub(r'^#include.*\n','',policy,flags=re.M))
    functions=[]
    for path,names in [
        ('src/game/spawn_object.c', ['try_allocate_object','deallocate_object','init_free_object_list',
            'clear_object_lists','unload_object','initialize_object','allocate_object_with_policy',
            'create_object_with_policy','create_object','create_object_for_parent','try_create_particle']),
        ('src/game/object_helpers.c', ['geo_update_small_water_splash_opacity', 'obj_translate_xz_random','obj_translate_xyz_random','spawn_object_at_origin','spawn_object','try_spawn_particle',
            'try_to_spawn_object','spawn_water_droplet_with_level','spawn_water_droplet',
            'spawn_water_droplet_at_water_level','cur_obj_spawn_particles_with_priority','cur_obj_spawn_particles']),
        ('src/game/object_list_processor.c', ['spawn_particle','update_objects_starting_at','update_objects_during_time_stop']),
        ('src/engine/behavior_script.c', ['bhv_cmd_spawn_child','bhv_cmd_spawn_child_with_param','bhv_cmd_parent_bit_clear']),
        ('src/game/behaviors/break_particles.inc.c', ['spawn_triangle_break_particles_with_priority','spawn_triangle_break_particles']),
        ('src/game/behaviors/water_splashes_and_waves.inc.c', ['spawn_shallow_water_droplets',
            'bhv_shallow_water_wave_spawn_droplets','bhv_shallow_water_splash_spawn_droplets',
            'spawn_slide_kick_liquid_particles', 'slide_kick_liquid_droplet_land',
            'bhv_water_splash_spawn_droplets','bhv_shallow_water_splash_init','bhv_water_droplet_loop',
            'bhv_wave_trail_shrink']),
        ('src/game/behaviors/ground_particles.inc.c', ['cur_obj_clear_particle_parent_flags','bhv_water_down_mist_init']),
        ('src/game/behaviors/water_objs.inc.c', ['bhv_spawn_mario_breath_bubble', 'bhv_water_waves_init']),
        ('src/game/behaviors/explosion.inc.c', ['bhv_explosion_loop']),
        ('src/game/behaviors/white_puff_explode.inc.c', ['bhv_white_puff_exploding_loop']),
    ]:
        functions += [src(path).function(name) for name in names]
    water = src('src/game/behaviors/water_splashes_and_waves.inc.c')
    start,_=water.function_span('spawn_shallow_water_droplets')
    (directory/'particle_params.inc').write_text(water.mapped(0,start))
    (directory/'particle_functions.inc').write_text('\n'.join(functions))


MUTATIONS = [
    ('liquid spawn height', 'src/game/behaviors/water_splashes_and_waves.inc.c', 'drop->oPosY = contactHeight;', 'drop->oPosY = m->pos[1];'),
    ('lava depends on water', 'src/game/behaviors/water_splashes_and_waves.inc.c', 'if (o->oBehParams2ndByte == LIQUID_PARTICLE_LAVA)', 'if (FALSE)'),
    ('liquid burst ignores pressure', 'src/game/behaviors/water_splashes_and_waves.inc.c', 's32 count = particle_budget_burst_count(PARTICLE_PRIORITY_DROPLET, SLIDE_KICK_LIQUID_DROPLET_COUNT);', 's32 count = SLIDE_KICK_LIQUID_DROPLET_COUNT;'),
    ('cold splash renders white', 'src/game/object_helpers.c', 'red = LIQUID_PARTICLE_COLD_R;', 'red = 255;'),
    ('attack record cleanup', 'src/game/spawn_object.c', 'mario_forget_attack_object(obj);', '(void) obj;'),
    ('live count', 'src/game/particle_budget.c', 'sParticleFreeSlots--;', 'sParticleFreeSlots += 0;'),
    ('priority order', 'src/game/particle_budget.c', 'sParticlePriorities[poolIndex] < sParticlePriorities[bestPoolIndex]', 'sParticlePriorities[poolIndex] > sParticlePriorities[bestPoolIndex]'),
    ('equal-priority protection', 'src/game/particle_budget.c', 'candidatePriority >= incomingPriority', 'candidatePriority > incomingPriority'),
    ('current object safety', 'src/game/particle_budget.c', 'obj == gCurrentObject || ', ''),
    ('reserve', 'src/game/spawn_object.c', 'particle_budget_free_slots() <= PARTICLE_SLOT_RESERVE', 'particle_budget_free_slots() < 0'),
    ('240-slot burst threshold', 'src/game/particle_budget.h', '#define PARTICLE_SOFT_RESERVE 72', '#define PARTICLE_SOFT_RESERVE 90'),
    ('flag ownership', 'src/game/particle_budget.c', '(sMarioParticleFlagOwners[i] == NULL || sMarioParticleFlagOwners[i] == obj)', 'TRUE'),
    ('free ownership', 'src/game/particle_budget.c', 'if (sMarioParticleFlagOwners[i] == obj)', 'if (sMarioParticleFlagOwners[i] != NULL)'),
    ('celebration limit', 'src/game/particle_budget.c', 'case PARTICLE_PRIORITY_CELEBRATION:\n            limit = 12;', 'case PARTICLE_PRIORITY_CELEBRATION:\n            limit = 60;'),
    ('bubble limit', 'src/game/behaviors/explosion.inc.c', 'count = particle_budget_burst_count(PARTICLE_PRIORITY_EXPLOSION_BUBBLE, 40);', 'count = 40;'),
    ('reuse birth stamp', 'src/game/particle_budget.c', 'sParticleBirthOrder[index] = sParticleNextBirthOrder++;', '(void) sParticleNextBirthOrder++;'),
]


def run(overrides):
    with tempfile.TemporaryDirectory(prefix='particle-tests-') as temp:
        temp=Path(temp)
        prepare(temp,overrides)
        executable=temp/'particle-tests'
        command=shlex.split(os.environ.get('CC','cc'))+['-std=c11','-O1','-g','-Wall','-Wextra',
            '-Werror=implicit-function-declaration','-Wno-unused-function','-Wno-unused-variable',
            '-I'+str(temp),'-I'+str(ROOT/'include'),str(ROOT/'tests/particle_budget_test.c'),'-lm','-o',str(executable)]
        result=subprocess.run(command,text=True,capture_output=True,timeout=30)
        if result.returncode:
            raise HarnessError(result.stderr)
        result=subprocess.run([str(executable)],text=True,capture_output=True,timeout=10)
        if result.returncode not in (0,1):
            raise HarnessError('Test executable crashed: '+result.stderr)
        return result


def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--self-check',action='store_true')
    args=parser.parse_args()
    try:
        result=run({})
        print(result.stdout,end='')
        if result.returncode:
            print(result.stderr,end='')
            return 1
        if args.self_check:
            for name,path,old,new in MUTATIONS:
                text=(ROOT/path).read_text()
                if text.count(old)!=1:
                    raise HarnessError('Mutation boundary changed: '+name)
                result=run({path:text.replace(old,new)})
                if result.returncode!=1:
                    raise HarnessError('Mutation survived: '+name)
                print('DETECTED '+name)
            print(f'{len(MUTATIONS)} deliberate mutations detected.')
        print('Host only; no renderer, ROM, emulator, or target performance measurement.')
        return 0
    except (HarnessError, OSError, subprocess.TimeoutExpired) as error:
        print('HARNESS ERROR: '+str(error))
        return 2


if __name__=='__main__':
    raise SystemExit(main())
