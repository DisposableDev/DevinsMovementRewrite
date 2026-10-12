"""Registry of suite-owned production extraction adapters."""
from regression_suites.zero_health import prepare_zero_health
from regression_suites.debug_warp import prepare_debug_warp
from regression_suites.end_screen import prepare_end_screen
from regression_suites.doors import prepare_doors
from regression_suites.rotation import prepare_rotation
from regression_suites.platform import prepare_platform
from regression_suites.he_thicc import prepare_he_thicc
from regression_suites.rumble import prepare_rumble
from regression_suites.character_save import prepare_character_save
from regression_suites.character import prepare_character
from regression_suites.punch import prepare_punch
from regression_suites.animation import prepare_animation
from regression_suites.audio import prepare_audio
from regression_suites.swimming import prepare_swimming
from regression_suites.swim_animation import prepare_swim_animation
from regression_suites.landing import prepare_landing
from regression_suites.bounce import prepare_bounce
from regression_suites.fixes import prepare_fixes
from regression_suites.geometry import prepare_geometry
from regression_suites.render import prepare_render
from regression_suites.wallpose import prepare_wallpose
from regression_suites.twirl import prepare_twirl
from regression_suites.twirl_speed import prepare_twirl_speed
from regression_suites.sideflip import prepare_sideflip
from regression_suites.air_backward import prepare_air_backward
from regression_suites.jump_kick_wall import prepare_jump_kick_wall
from regression_suites.metal import prepare_metal
from regression_suites.bowser import prepare_bowser
from regression_suites.shell import prepare_shell
from regression_suites.switches import prepare_switches
from regression_suites.one_up import prepare_one_up
from regression_suites.clam import prepare_clam
from regression_suites.hud import prepare_hud


PREPARERS = {
    "zero_health": prepare_zero_health,
    "debug_warp": prepare_debug_warp,
    "end_screen": prepare_end_screen,
    "doors": prepare_doors,
    "rotation": prepare_rotation,
    "platform": prepare_platform,
    "he_thicc": prepare_he_thicc,
    "rumble": prepare_rumble,
    "character_save": prepare_character_save,
    "character": prepare_character,
    "punch": prepare_punch,
    "animation": prepare_animation,
    "audio": prepare_audio,
    "swimming": prepare_swimming,
    "swim_animation": prepare_swim_animation,
    "landing": prepare_landing,
    "bounce": prepare_bounce,
    "fixes": prepare_fixes,
    "geometry": prepare_geometry,
    "render": prepare_render,
    "wallpose": prepare_wallpose,
    "twirl": prepare_twirl,
    "twirl_speed": prepare_twirl_speed,
    "sideflip": prepare_sideflip,
    "air_backward": prepare_air_backward,
    "jump_kick_wall": prepare_jump_kick_wall,
    "metal": prepare_metal,
    "bowser": prepare_bowser,
    "shell": prepare_shell,
    "switches": prepare_switches,
    "one_up": prepare_one_up,
    "clam": prepare_clam,
    "hud": prepare_hud,
}
