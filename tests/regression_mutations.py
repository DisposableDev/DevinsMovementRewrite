"""Registry and selection of suite-owned deliberate source defects."""
from regression_suites.mutation_support import MutationSource, resolve_overrides
from regression_suites.common import shared_mutations, mutations_audit_repairs
from regression_suites.zero_health import mutations_zero_health
from regression_suites.debug_warp import mutations_debug_warp
from regression_suites.end_screen import mutations_end_screen
from regression_suites.doors import mutations_doors
from regression_suites.rotation import mutations_rotation
from regression_suites.platform import mutations_platform
from regression_suites.he_thicc import mutations_he_thicc
from regression_suites.rumble import mutations_rumble
from regression_suites.character_save import mutations_character_save
from regression_suites.character import mutations_character
from regression_suites.punch import mutations_punch
from regression_suites.animation import mutations_animation
from regression_suites.audio import mutations_audio
from regression_suites.swimming import mutations_swimming
from regression_suites.swim_animation import mutations_swim_animation
from regression_suites.landing import mutations_landing
from regression_suites.bounce import mutations_bounce
from regression_suites.fixes import mutations_fixes
from regression_suites.geometry import mutations_geometry
from regression_suites.render import mutations_render
from regression_suites.wallpose import mutations_wallpose
from regression_suites.twirl import mutations_twirl
from regression_suites.twirl_speed import mutations_twirl_speed
from regression_suites.sideflip import mutations_sideflip
from regression_suites.air_backward import mutations_air_backward
from regression_suites.jump_kick_wall import mutations_jump_kick_wall
from regression_suites.metal import mutations_metal
from regression_suites.bowser import mutations_bowser
from regression_suites.shell import mutations_shell
from regression_suites.clam import mutations_clam
from regression_suites.hud import mutations_hud
from regression_suites.switches import mutations_switches


MUTATORS = {
    "switches": mutations_switches,
    "zero_health": mutations_zero_health,
    "debug_warp": mutations_debug_warp,
    "end_screen": mutations_end_screen,
    "doors": mutations_doors,
    "rotation": mutations_rotation,
    "platform": mutations_platform,
    "he_thicc": mutations_he_thicc,
    "rumble": mutations_rumble,
    "character_save": mutations_character_save,
    "character": mutations_character,
    "punch": mutations_punch,
    "animation": mutations_animation,
    "audio": mutations_audio,
    "swimming": mutations_swimming,
    "swim_animation": mutations_swim_animation,
    "landing": mutations_landing,
    "bounce": mutations_bounce,
    "fixes": mutations_fixes,
    "geometry": mutations_geometry,
    "render": mutations_render,
    "wallpose": mutations_wallpose,
    "twirl": mutations_twirl,
    "twirl_speed": mutations_twirl_speed,
    "sideflip": mutations_sideflip,
    "air_backward": mutations_air_backward,
    "jump_kick_wall": mutations_jump_kick_wall,
    "metal": mutations_metal,
    "bowser": mutations_bowser,
    "shell": mutations_shell,
    "clam": mutations_clam,
    "hud": mutations_hud,
}

def mutation_specs(root, suites):
    for suite in suites:
        if suite in MUTATORS:
            yield from MUTATORS[suite](root)
    yield from shared_mutations(root, suites)
    yield from mutations_audit_repairs(root, suites)

def mutations(root, suites, cases=None, names=None, materialize=True):
    for suite, name, case, overrides in mutation_specs(root, suites):
        if cases is not None and case not in cases:
            continue
        if names is not None and name not in names:
            continue
        if materialize:
            overrides = resolve_overrides(overrides)
        yield suite, name, case, overrides
