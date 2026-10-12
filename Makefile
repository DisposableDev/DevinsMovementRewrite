# Makefile to rebuild SM64 split image

include util.mk

# Queries, cleanup, and the default wrapper do not prepare build inputs.
QUALITY_CONFIG_ONLY := $(filter default clean distclean print-% quality-flags,$(or $(MAKECMDGOALS),default))
DRY_RUN := $(findstring n,$(firstword -$(MAKEFLAGS)))

# Default target
default:
	@$(MAKE) --no-print-directory $(default-job-flags) -C tools
	@$(MAKE) --no-print-directory $(default-job-flags) all

TARGET_STRING := sm64

# Preprocessor definitions
DEFINES :=

SRC_DIRS :=
USE_DEBUG := 0

#==============================================================================#
# Build Options                                                                #
#==============================================================================#

# These options can either be set by building with 'make SETTING=value'.
# 'make clean' may be required first.

# Build for the N64 (turn this off for ports)
TARGET_N64 ?= 1

# Platform: Nintendo 64 only.
CONSOLE ?= n64
$(eval $(call validate-option,CONSOLE,n64))
INCLUDE_DIRS += include/n64
LIBS_DIR := lib/n64

# Game builds use GCC. Clang remains the source-quality analysis frontend.
COMPILER ?= gcc
ifeq ($(MAKECMDGOALS),quality-flags)
  $(eval $(call validate-option,COMPILER,gcc clang))
else
  $(eval $(call validate-option,COMPILER,gcc))
endif
QUALITY_CLANG := $(if $(filter clang,$(COMPILER)),1,0)

# Experimental LTO within the resident engine segment; normal builds stay native.
ENGINE_LTO ?= 0
$(eval $(call validate-option,ENGINE_LTO,0 1))
ENGINE_LTO_BUILD_SUFFIX :=
ifeq ($(ENGINE_LTO),1)
  ifneq ($(COMPILER),gcc)
    $(error ENGINE_LTO requires COMPILER=gcc)
  endif
  ifneq ($(TARGET_N64),1)
    $(error ENGINE_LTO requires TARGET_N64=1)
  endif
  ifneq ($(CONSOLE),n64)
    $(error ENGINE_LTO requires CONSOLE=n64)
  endif
  ENGINE_LTO_BUILD_SUFFIX := _engine_lto
endif

# Branch-counter capture; separate output, disabled in ordinary builds.
GCC_PROFILE ?= 0
$(eval $(call validate-option,GCC_PROFILE,0 1))
GCC_PROFILE_BUILD_SUFFIX :=
ifeq ($(GCC_PROFILE),1)
  ifneq ($(COMPILER),gcc)
    $(error GCC_PROFILE requires COMPILER=gcc)
  endif
  ifneq ($(TARGET_N64),1)
    $(error GCC_PROFILE requires TARGET_N64=1)
  endif
  ifneq ($(CONSOLE),n64)
    $(error GCC_PROFILE requires CONSOLE=n64)
  endif
  ifneq ($(ENGINE_LTO),0)
    $(error GCC_PROFILE requires ENGINE_LTO=0)
  endif
  GCC_PROFILE_BUILD_SUFFIX := _gcc_profile
endif
DEFINES += GCC_PROFILE=$(GCC_PROFILE)

# GP-relative resident engine; permanent small data is boot-loaded.
# Set ENGINE_G16=0 to restore ordinary addressing.
ENGINE_G16 ?= 1
$(eval $(call validate-option,ENGINE_G16,0 1))
ENGINE_G16_BUILD_SUFFIX :=
ENGINE_G16_LDFLAGS :=
ifeq ($(ENGINE_G16),1)
  # Clang may read the enabled source for quality checks without building a ROM.
  ifeq ($(QUALITY_CONFIG_ONLY),)
    ifneq ($(COMPILER),gcc)
      $(error ENGINE_G16 requires COMPILER=gcc)
    endif
  endif
  ifneq ($(TARGET_N64),1)
    $(error ENGINE_G16 requires TARGET_N64=1)
  endif
  ifneq ($(CONSOLE),n64)
    $(error ENGINE_G16 requires CONSOLE=n64)
  endif
  ifneq ($(ENGINE_LTO),0)
    $(error Test ENGINE_G16 separately from ENGINE_LTO)
  endif
  ENGINE_G16_LDFLAGS := -G 16 --wrap=osCreateThread
else
  ENGINE_G16_BUILD_SUFFIX := _engine_g0
endif
DEFINES += ENGINE_G16=$(ENGINE_G16)


# LIBGCCDIR - selects the libgcc configuration for checking for dividing by zero
#   trap - GCC default behavior, uses teq instructions which some emulators don't like
#   divbreak - this is similar to IDO behavior, and is default.
#   nocheck - never checks for dividing by 0. Technically fastest, but also UB so not recommended
LIBGCCDIR ?= divbreak
$(eval $(call validate-option,LIBGCCDIR,trap divbreak nocheck))


# Persistent storage: 32 KB SRAM is required by player statistics.
SAVETYPE ?= sram
$(eval $(call validate-option,SAVETYPE,sram))
DEFINES += SRAM=1

# VERSION - selects the version of the game to build
#   jp - builds the 1996 Japanese version
#   us - builds the 1996 North American version
#   eu - builds the 1997 PAL version
#   sh - builds the 1997 Japanese Shindou version, with rumble pak support
VERSION ?= us
$(eval $(call validate-option,VERSION,jp us eu sh))

# Conservative audio-quality trial; 0 restores the original synthesis settings.
AUDIO_QUALITY_TRIAL ?= 0
$(eval $(call validate-option,AUDIO_QUALITY_TRIAL,0 1))
DEFINES += AUDIO_QUALITY_TRIAL=$(AUDIO_QUALITY_TRIAL)
# Use the accepted combined trial for US/JP; 0 restores normal audio.
ifneq ($(filter us jp,$(VERSION)),)
AUDIO_TRIAL ?= 10
else
AUDIO_TRIAL ?= 0
endif
$(eval $(call validate-option,AUDIO_TRIAL,0 1 2 3 4 5 6 7 8 9 10))
DEFINES += AUDIO_TRIAL=$(AUDIO_TRIAL)
# Experimental command format requires an RSP that executes the custom microcode.
AUDIO_LINEAR_ENVELOPES ?= 0
$(eval $(call validate-option,AUDIO_LINEAR_ENVELOPES,0 1))
DEFINES += AUDIO_LINEAR_ENVELOPES=$(AUDIO_LINEAR_ENVELOPES)
AUDIO_LOW_LATENCY ?= 1
$(eval $(call validate-option,AUDIO_LOW_LATENCY,0 1))
DEFINES += AUDIO_LOW_LATENCY=$(AUDIO_LOW_LATENCY)
AUDIO_MUPEN_SAMPLE_CLOCK ?= 1
$(eval $(call validate-option,AUDIO_MUPEN_SAMPLE_CLOCK,0 1))
DEFINES += AUDIO_MUPEN_SAMPLE_CLOCK=$(AUDIO_MUPEN_SAMPLE_CLOCK)
ifneq ($(AUDIO_TRIAL),0)
  ifneq ($(filter 1,$(AUDIO_QUALITY_TRIAL) $(AUDIO_LINEAR_ENVELOPES)),)
    $(error AUDIO_TRIAL must be tested separately from the older audio experiments)
  endif
  ifeq ($(VERSION),sh)
    ifneq ($(filter 2 3 4 5 6 7 8 10,$(AUDIO_TRIAL)),)
      $(error This AUDIO_TRIAL uses the US/JP/EU audio interface, not Shindou)
    endif
  endif
  ifeq ($(VERSION),eu)
    ifneq ($(filter 8 10,$(AUDIO_TRIAL)),)
      $(error The released-tail AUDIO_TRIAL currently supports US/JP)
    endif
  endif
endif
# Normal builds keep the established ROM path used by play/aplay.
AUDIO_BUILD_SUFFIX :=
ifeq ($(AUDIO_LINEAR_ENVELOPES),1)
  AUDIO_BUILD_SUFFIX := _audio_linear
else ifeq ($(AUDIO_QUALITY_TRIAL),1)
  AUDIO_BUILD_SUFFIX := _audio_trial
else ifneq ($(AUDIO_TRIAL),$(if $(filter us jp,$(VERSION)),10,0))
  AUDIO_BUILD_SUFFIX := _audio_trial_$(AUDIO_TRIAL)
endif

ifeq ($(AUDIO_LOW_LATENCY),0)
  AUDIO_BUILD_SUFFIX := $(AUDIO_BUILD_SUFFIX)_audio_buffered
endif
ifeq ($(AUDIO_MUPEN_SAMPLE_CLOCK),0)
  AUDIO_BUILD_SUFFIX := $(AUDIO_BUILD_SUFFIX)_audio_clock_off
endif

ifeq      ($(VERSION),jp)
  DEFINES += VERSION_JP=1
else ifeq ($(VERSION),us)
  DEFINES += VERSION_US=1
else ifeq ($(VERSION),eu)
  DEFINES += VERSION_EU=1
else ifeq ($(VERSION),sh)
  DEFINES += VERSION_SH=1
endif

# FIXLIGHTS - converts light objects to light color commands for assets, needed for vanilla-style lighting
FIXLIGHTS ?= 1

#----------------
# Debug Switches - Comment these out to remove the functionality, then run `make clean`
#----------------
# Export symbols and stack trace data for crash screen
DEBUG_EXPORT_SYMBOLS := 1
# Include line data, at the cost of a significantly longer link time and a few extra megabytes of ROM
DEBUG_EXPORT_ALL_LINES := 0
# Crash Screen: enable symbol-assisted stack trace. Adds a bit more code to (somewhat) intelligently walk the stack.
# Requires DEBUG_EXPORT_SYMBOLS to be enabled too
# If disabled, stack trace will just dump the memory at the stack pointer, from which one can cross-reference with the function map.
DEBUG_FULL_STACK_TRACE := 1

ifeq ($(DEBUG_EXPORT_SYMBOLS),1)
  ifeq ($(DEBUG_EXPORT_ALL_LINES),1)
  	DEBUG_EXPORT_ALL_LINES_FLAG += --all-lines
  	DEFINES += DEBUG_EXPORT_ALL_LINES=1
  endif
  DEFINES += DEBUG_EXPORT_SYMBOLS=1
  GENERATE_DWARF := -gdwarf-4
  DEFAULT_OPT_FLAGS += $(GENERATE_DWARF)
  SYMBOL_TABLE = $(BUILD_DIR)/sm64.sym.o
endif
ifeq ($(DEBUG_FULL_STACK_TRACE),1)
  DEFINES += DEBUG_FULL_STACK_TRACE=1
endif

TARGET := sm64


# GRUCODE - selects which RSP microcode to use.
#   f3dex   -
#   f3dex2  -
#   l3dex2  - F3DEX2 version that only renders in wireframe
#   f3dzex  - newer, experimental microcode used in Animal Crossing
#   super3d - extremely experimental version of Fast3D lacking many features for speed
GRUCODE ?= f3dzex
$(eval $(call validate-option,GRUCODE,f3dex f3dex2 f3dex2pl f3dzex super3d l3dex2))

ifeq ($(GRUCODE),f3dex) # Fast3DEX
  DEFINES += F3DEX_GBI=1 F3DEX_GBI_SHARED=1
else ifeq ($(GRUCODE),f3dex2) # Fast3DEX2
  DEFINES += F3DEX_GBI_2=1 F3DEX_GBI_SHARED=1
else ifeq ($(GRUCODE),l3dex2) # Line3DEX2
  DEFINES += L3DEX2_GBI=1 L3DEX2_ALONE=1 F3DEX_GBI_2=1 F3DEX_GBI_SHARED=1
else ifeq ($(GRUCODE),f3dex2pl) # Fast3DEX2_PosLight
  DEFINES += F3DEX2PL_GBI=1 F3DEX_GBI_2=1 F3DEX_GBI_SHARED=1
else ifeq ($(GRUCODE),f3dzex) # Fast3DZEX (2.08J / Animal Forest - Dōbutsu no Mori)
  DEFINES += F3DZEX_NON_GBI_2=1 F3DEX_GBI_2=1 F3DEX_GBI_SHARED=1
else ifeq ($(GRUCODE),super3d) # Super3D
  $(warning Super3D is experimental. Try at your own risk.)
  DEFINES += SUPER3D_GBI=1 F3D_NEW=1
endif

# TEXT ENGINES
#   s2dex_text_engine - Text Engine by someone2639
TEXT_ENGINE := none
$(eval $(call validate-option,TEXT_ENGINE,none s2dex_text_engine))

ifeq ($(TEXT_ENGINE), s2dex_text_engine)
  DEFINES += S2DEX_GBI_2=1 S2DEX_TEXT_ENGINE=1
  SRC_DIRS += src/s2d_engine
endif
# add more text engines here

#==============================================================================#
# Optimization flags                                                           #
#==============================================================================#

# The general code optimization flag that governs the compiler's overall strategy.
# Some valid values are: [-O0, -O1, -O2, -O3, -Ofast, -Os]
# Set to -O0 (or comment out) to turn off code optimization, then run `make clean`
GENERAL_OPTIMIZATION_LEVEL := -Os

# Default non-gcc opt flags
DEFAULT_OPT_FLAGS += $(GENERAL_OPTIMIZATION_LEVEL) -ffinite-math-only -fno-signed-zeros -fno-math-errno
# Note: -fno-associative-math is used here to suppress warnings, ideally we would enable this as an optimization but
# this conflicts with -ftrapping-math apparently.
# HACKERSM64_DO: Figure out how to allow -fassociative-math to be enabled
SAFETY_OPT_FLAGS = -ftrapping-math -fno-associative-math

# Main opt flags
GCC_MAIN_OPT_FLAGS = \
  $(DEFAULT_OPT_FLAGS) $(SAFETY_OPT_FLAGS) \
  --param case-values-threshold=20 \
  --param max-completely-peeled-insns=10 \
  --param max-unrolled-insns=10 \
  -finline-limit=1 \
  -freorder-blocks-algorithm=simple  \
  -ffunction-sections \
  -fdata-sections

# Surface Collision
GCC_COLLISION_OPT_FLAGS = \
  $(DEFAULT_OPT_FLAGS) $(SAFETY_OPT_FLAGS) \
  --param case-values-threshold=20 \
  --param max-completely-peeled-insns=100 \
  --param max-unrolled-insns=100 \
  -finline-limit=0 \
  -fno-inline \
  -freorder-blocks-algorithm=simple  \
  -ffunction-sections \
  -fdata-sections \
  -falign-functions=32

# Math Util
GCC_MATH_UTIL_OPT_FLAGS = \
  $(DEFAULT_OPT_FLAGS) $(SAFETY_OPT_FLAGS) \
  -fno-unroll-loops \
  -fno-peel-loops \
  --param case-values-threshold=20  \
  -ffunction-sections \
  -fdata-sections \
  -falign-functions=32
#   - setting any sort of -finline-limit has shown to worsen performance with math_util.c,
#     lower values were the worst, the higher you go - the closer performance gets to not setting it at all

# Rendering graph node
GCC_GRAPH_NODE_OPT_FLAGS = \
  $(DEFAULT_OPT_FLAGS) $(SAFETY_OPT_FLAGS) \
  --param case-values-threshold=20 \
  --param max-completely-peeled-insns=100 \
  --param max-unrolled-insns=100 \
  -finline-limit=0 \
  -freorder-blocks-algorithm=simple  \
  -ffunction-sections \
  -fdata-sections \
  -falign-functions=32
#==============================================================================#

ifeq ($(QUALITY_CLANG),0)
  MIPSISET     := -mips3
  OPT_FLAGS           := $(GCC_MAIN_OPT_FLAGS)
  COLLISION_OPT_FLAGS  = $(GCC_COLLISION_OPT_FLAGS)
  MATH_UTIL_OPT_FLAGS  = $(GCC_MATH_UTIL_OPT_FLAGS)
  GRAPH_NODE_OPT_FLAGS = $(GCC_GRAPH_NODE_OPT_FLAGS)
else
  # Analysis-only frontend settings; no Clang game build is supported.
  # Clang doesn't support ABI 'o32' for 'mips3'
  MIPSISET     := -mips2
  OPT_FLAGS    := $(DEFAULT_OPT_FLAGS)
  COLLISION_OPT_FLAGS  = $(DEFAULT_OPT_FLAGS)
  MATH_UTIL_OPT_FLAGS  = $(DEFAULT_OPT_FLAGS)
  GRAPH_NODE_OPT_FLAGS = $(DEFAULT_OPT_FLAGS)
endif

# UNF - whether to use UNFLoader flashcart library
#   1 - includes code in ROM
#   0 - does not
UNF ?= 0
$(eval $(call validate-option,UNF,0 1))

# if `unf` is a target, make sure that UNF is set
ifneq ($(filter unf,$(MAKECMDGOALS)),)
	UNF = 1
endif

ifeq ($(UNF),1)
  DEFINES += UNF=1
  SRC_DIRS += src/usb
endif

# ISVPRINT - whether to fake IS-Viewer presence,
# allowing for usage of CEN64 (and possibly Project64) to print messages to terminal.
#   1 - includes code in ROM
#   0 - does not
ISVPRINT ?= 0
$(eval $(call validate-option,ISVPRINT,0 1))
ifeq ($(ISVPRINT),1)
  DEFINES += ISVPRINT=1
  USE_DEBUG := 1
endif

ifeq ($(USE_DEBUG),1)
  ULTRALIB := ultra_d
  DEFINES += DEBUG=1 OVERWRITE_OSPRINT=1
else ifeq ($(UNF),1)
  ULTRALIB := ultra
  DEFINES += _FINALROM=1 NDEBUG=1 OVERWRITE_OSPRINT=1
else
  ULTRALIB := ultra_rom
  DEFINES += _FINALROM=1 NDEBUG=1 OVERWRITE_OSPRINT=0
endif

# Reject requests to enable the retired movie decoder.
ifneq ($(filter-out 0,$(HVQM)),)
  $(error HVQM movie playback has been retired)
endif

# LIBPL - whether to include libpl library for interfacing with Parallel Launcher
# (library will be pulled into repo after building with this enabled for the first time)
#   1 - includes code in ROM
#   0 - does not
LIBPL ?= 0
LIBPL_DIR := lib/libpl
$(eval $(call validate-option,LIBPL,0 1))
ifeq ($(LIBPL),1)
  DEFINES += LIBPL=1
  SRC_DIRS += $(LIBPL_DIR)
endif

FIRE_SPITTER_SLEEP_TRIAL ?= 0
$(eval $(call validate-option,FIRE_SPITTER_SLEEP_TRIAL,0 1))
DEFINES += FIRE_SPITTER_SLEEP_TRIAL=$(FIRE_SPITTER_SLEEP_TRIAL)

SKEETER_WATER_SLEEP_TRIAL ?= 0
$(eval $(call validate-option,SKEETER_WATER_SLEEP_TRIAL,0 1))
DEFINES += SKEETER_WATER_SLEEP_TRIAL=$(SKEETER_WATER_SLEEP_TRIAL)

# Ground Skeeter patrol trial: 2 enables it; Whomp and Chuckya patrols sleep by default.
ENEMY_PATROL_SLEEP_TRIAL ?= 0
$(eval $(call validate-option,ENEMY_PATROL_SLEEP_TRIAL,0 2))
DEFINES += ENEMY_PATROL_SLEEP_TRIAL=$(ENEMY_PATROL_SLEEP_TRIAL)

PERFORMANCE_CAPTURE ?= 0
COLLISION_PROFILING ?= 0
PERSISTENT_DYNAMIC_SURFACES ?= 0
DORRIE_STATIONARY_SURFACES ?= 1
JRB_STATIONARY_SURFACES ?= 1
$(eval $(call validate-option,PERFORMANCE_CAPTURE,0 1))
$(eval $(call validate-option,COLLISION_PROFILING,0 1))
$(eval $(call validate-option,PERSISTENT_DYNAMIC_SURFACES,0 1))
$(eval $(call validate-option,DORRIE_STATIONARY_SURFACES,0 1))
$(eval $(call validate-option,JRB_STATIONARY_SURFACES,0 1))
DEFINES += PERFORMANCE_CAPTURE=$(PERFORMANCE_CAPTURE) COLLISION_PROFILING=$(COLLISION_PROFILING) PERSISTENT_DYNAMIC_SURFACES=$(PERSISTENT_DYNAMIC_SURFACES)
DEFINES += DORRIE_STATIONARY_SURFACES=$(DORRIE_STATIONARY_SURFACES) JRB_STATIONARY_SURFACES=$(JRB_STATIONARY_SURFACES)
ifeq ($(GCC_PROFILE),1)
  ifneq ($(PERFORMANCE_CAPTURE),1)
    $(error GCC_PROFILE requires PERFORMANCE_CAPTURE=1 for ISViewer capture)
  endif
  ifneq ($(ENGINE_G16),1)
    $(error This initial GCC_PROFILE setup requires ENGINE_G16=1)
  endif
endif
PERFORMANCE_BUILD_SUFFIX :=
ifeq ($(PERFORMANCE_CAPTURE),1)
  PERFORMANCE_BUILD_SUFFIX := _capture
endif
ifeq ($(COLLISION_PROFILING),1)
  PERFORMANCE_BUILD_SUFFIX := $(PERFORMANCE_BUILD_SUFFIX)_collision
endif
ifeq ($(PERSISTENT_DYNAMIC_SURFACES),1)
  PERFORMANCE_BUILD_SUFFIX := $(PERFORMANCE_BUILD_SUFFIX)_persistent_surfaces
endif
ifeq ($(JRB_STATIONARY_SURFACES),0)
  PERFORMANCE_BUILD_SUFFIX := $(PERFORMANCE_BUILD_SUFFIX)_no_jrb_stationary
endif
ifeq ($(DORRIE_STATIONARY_SURFACES),0)
  PERFORMANCE_BUILD_SUFFIX := $(PERFORMANCE_BUILD_SUFFIX)_no_dorrie_stationary
endif

ifeq ($(FIRE_SPITTER_SLEEP_TRIAL),1)
  PERFORMANCE_BUILD_SUFFIX := $(PERFORMANCE_BUILD_SUFFIX)_fire_spitter_sleep
endif

ifeq ($(SKEETER_WATER_SLEEP_TRIAL),1)
  PERFORMANCE_BUILD_SUFFIX := $(PERFORMANCE_BUILD_SUFFIX)_skeeter_water_sleep
endif

ifneq ($(ENEMY_PATROL_SLEEP_TRIAL),0)
  PERFORMANCE_BUILD_SUFFIX := $(PERFORMANCE_BUILD_SUFFIX)_patrol_sleep_$(ENEMY_PATROL_SLEEP_TRIAL)
endif

BUILD_DIR_BASE := build
# BUILD_DIR is the location where all build artifacts are placed
BUILD_DIR      := $(BUILD_DIR_BASE)/$(VERSION)_$(CONSOLE)$(AUDIO_BUILD_SUFFIX)$(PERFORMANCE_BUILD_SUFFIX)$(ENGINE_LTO_BUILD_SUFFIX)$(ENGINE_G16_BUILD_SUFFIX)$(GCC_PROFILE_BUILD_SUFFIX)

COMPRESS ?= lz4t
$(eval $(call validate-option,COMPRESS,lz4t))
DEFINES += LZ4T=1

# Whether to hide commands or not
VERBOSE ?= 0
ifeq ($(VERBOSE),0)
  V := @
endif

# Whether to colorize build messages
COLOR ?= 1

#==============================================================================#
# Universal Dependencies                                                       #
#==============================================================================#

TOOLS_DIR := tools


# (This is a bit hacky, but a lot of rules implicitly depend
# on tools and assets, and we use directory globs further down
# in the makefile that we want should cover assets.)

PYTHON := python3

ifeq ($(QUALITY_CONFIG_ONLY)$(DRY_RUN),)

  # Extract assets if necessary
  NOEXTRACT ?= 0
  ifeq ($(NOEXTRACT),0)
    # Parse-time subprocesses cannot inherit the recursive build's jobserver descriptors.
    DUMMY != MAKEFLAGS="$(filter-out -j% --jobserver-auth=% --jobserver-fds=%,$(MAKEFLAGS))" \
      $(PYTHON) extract_assets.py >&2 || echo FAIL
    ifeq ($(DUMMY),FAIL)
      $(error Failed to extract assets from found baseroms)
    endif
  endif

  # Check tools serially during parsing; default builds prepare them in parallel.
  $(info Building tools...)
  DUMMY != MAKEFLAGS="$(filter-out -j% --jobserver-auth=% --jobserver-fds=%,$(MAKEFLAGS))" \
    "$(MAKE)" -s -C $(TOOLS_DIR) >&2 || echo FAIL
    ifeq ($(DUMMY),FAIL)
      $(error Failed to build tools)
    endif

  # Clone any needed submodules
  ifeq ($(LIBPL),1)
    ifeq ($(wildcard $(LIBPL_DIR)/*.h),)
      $(info Cloning libpl submodule...)
      DUMMY != git submodule update --init $(LIBPL_DIR) > /dev/null || echo FAIL
      ifeq ($(DUMMY),FAIL)
        $(error Failed to clone libpl submodule)
      endif
    endif
  endif

  $(info Building ROM...)

endif


#==============================================================================#
# Target Executable and Sources                                                #
#==============================================================================#

BUILD_DIR_BASE := build
# BUILD_DIR is the location where all build artifacts are placed
BUILD_DIR      := $(BUILD_DIR_BASE)/$(VERSION)_$(CONSOLE)$(AUDIO_BUILD_SUFFIX)$(PERFORMANCE_BUILD_SUFFIX)$(ENGINE_LTO_BUILD_SUFFIX)$(ENGINE_G16_BUILD_SUFFIX)$(GCC_PROFILE_BUILD_SUFFIX)
ROM            := $(BUILD_DIR)/$(TARGET_STRING).z64
ELF            := $(BUILD_DIR)/$(TARGET_STRING).elf
LD_SCRIPT      := sm64.ld
SOUND_BIN_DIR  := $(BUILD_DIR)/sound
TEXTURE_DIR    := textures
ACTOR_DIR      := actors
LEVEL_DIRS     := $(patsubst levels/%,%,$(dir $(wildcard levels/*/header.h)))

# Directories containing source files
SRC_DIRS += src src/boot src/game src/engine src/audio src/menu src/buffers src/debugger lib/librtc actors levels bin data assets asm lib sound
GODDARD_SRC_DIRS := src/goddard src/goddard/dynlists
BIN_DIRS := bin bin/$(VERSION)

# File dependencies and variables for specific files
include Makefile.split

# Source code files
LEVEL_C_FILES     := $(wildcard levels/*/leveldata.c) $(wildcard levels/*/script.c) $(wildcard levels/*/geo.c)
C_FILES           := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c)) $(LEVEL_C_FILES)
CPP_FILES         := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
GODDARD_C_FILES   := $(foreach dir,$(GODDARD_SRC_DIRS),$(wildcard $(dir)/*.c))
S_FILES           := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.s))
GENERATED_C_FILES := $(BUILD_DIR)/assets/mario_anim_data.c $(BUILD_DIR)/assets/demo_data.c

# Ignore all .inc.c files
C_FILES           := $(filter-out %.inc.c,$(C_FILES))

# Sound files
SOUND_BANK_FILES    := $(wildcard sound/sound_banks/*.json)
SOUND_SAMPLE_DIRS   := $(wildcard sound/samples/*)
SOUND_SAMPLE_AIFFS  := $(foreach dir,$(SOUND_SAMPLE_DIRS),$(wildcard $(dir)/*.aiff))
SOUND_SAMPLE_TABLES := $(foreach file,$(SOUND_SAMPLE_AIFFS),$(BUILD_DIR)/$(file:.aiff=.table))
SOUND_SAMPLE_AIFCS  := $(foreach file,$(SOUND_SAMPLE_AIFFS),$(BUILD_DIR)/$(file:.aiff=.aifc))
SOUND_SEQUENCE_DIRS := sound/sequences sound/sequences/$(VERSION)
# all .m64 files in SOUND_SEQUENCE_DIRS, plus all .m64 files that are generated from .s files in SOUND_SEQUENCE_DIRS
SOUND_SEQUENCE_FILES := \
  $(foreach dir,$(SOUND_SEQUENCE_DIRS),\
    $(wildcard $(dir)/*.m64) \
    $(foreach file,$(wildcard $(dir)/*.s),$(BUILD_DIR)/$(file:.s=.m64)) \
  )

# Object files
O_FILES := $(foreach file,$(C_FILES),$(BUILD_DIR)/$(file:.c=.o)) \
           $(foreach file,$(CPP_FILES),$(BUILD_DIR)/$(file:.cpp=.o)) \
           $(foreach file,$(S_FILES),$(BUILD_DIR)/$(file:.s=.o)) \
           $(foreach file,$(GENERATED_C_FILES),$(file:.c=.o))

# Keep original objects for compilation/dependencies, and replace only the
# resident C inputs at final link. SDK, audio, assembly and overlays stay native.
LINK_O_FILES := $(O_FILES)
ifeq ($(ENGINE_LTO),1)
  ENGINE_LTO_C_FILES := $(filter src/game/%.c src/engine/%.c,$(C_FILES))
  ENGINE_LTO_O_FILES := $(addprefix $(BUILD_DIR)/,$(ENGINE_LTO_C_FILES:.c=.o))
  ENGINE_LTO_OBJECT := $(BUILD_DIR)/engine_lto.o
  LINK_O_FILES := $(filter-out $(ENGINE_LTO_O_FILES),$(O_FILES)) $(ENGINE_LTO_OBJECT)
endif

GODDARD_O_FILES := $(foreach file,$(GODDARD_C_FILES),$(BUILD_DIR)/$(file:.c=.o))

# Automatic dependency files
DEP_FILES := $(O_FILES:.o=.d) $(GODDARD_O_FILES:.o=.d) $(BUILD_DIR)/$(LD_SCRIPT).d

# A cached object without its dependency file cannot notice included model or
# texture changes. Recompile it once to regenerate those dependencies.
DEP_TRACKED_OBJECTS := $(filter $(BUILD_DIR)/%.o,$(O_FILES) $(GODDARD_O_FILES))
MISSING_DEP_OBJECTS := $(foreach obj,$(DEP_TRACKED_OBJECTS),$(if $(wildcard $(obj:.o=.d)),,$(obj)))
ifneq ($(strip $(MISSING_DEP_OBJECTS)),)
$(MISSING_DEP_OBJECTS): force_missing_dependencies
.PHONY: force_missing_dependencies
force_missing_dependencies:
endif

#==============================================================================#
# Compiler Options                                                             #
#==============================================================================#

ifneq ($(strip $(N64_TOOLCHAIN_PREFIX)),)
# The workspace pins its default compiler; command-line CROSS overrides allow comparisons.
CROSS := $(N64_TOOLCHAIN_PREFIX)
else ifeq ($(QUALITY_CONFIG_ONLY),)
CROSS := $(call find-mips-toolchain)
else
CROSS := mips-linux-gnu-
endif

LIBRARIES := nustd goddard

LINK_LIBRARIES = $(foreach i,$(LIBRARIES),-l$(i))

export LD_LIBRARY_PATH=./tools

AS        := $(CROSS)as
CC := $(CROSS)gcc
CXX := $(CROSS)g++
$(BUILD_DIR)/actors/%.o: OPT_FLAGS := -Ofast -mlong-calls
$(BUILD_DIR)/levels/%.o: OPT_FLAGS := -Ofast -mlong-calls

GCC_PROFILE_LIB :=
ifeq ($(GCC_PROFILE),1)
  GCC_PROFILE_LIB := $(shell $(CC) -march=vr4300 -mabi=32 -mhard-float -print-file-name=libgcov.a)
  ifeq ($(wildcard $(GCC_PROFILE_LIB)),)
    $(error GCC_PROFILE cannot locate the matching o32 libgcov.a)
  endif
  # Start with one gameplay-thread collision unit; exporter remains native.
  $(BUILD_DIR)/src/engine/surface_collision.o: private CFLAGS += -fprofile-arcs -ftest-coverage -fprofile-info-section -fprofile-abs-path
  $(BUILD_DIR)/src/game/gcc_profile.o: private CFLAGS += -isystem $(dir $(GCC_PROFILE_LIB))../include
endif

ARCH := $(shell uname -p)

# Check processor architecture. ARM users need a different binutils package.
ifeq ($(ARCH), arm)
  LD := tools/mips64-elf-ld-arm
else
  ifneq ($(call find-command,mips-n64-ld),)
    LD        := mips-n64-ld
  else
    LD        := tools/mips64-elf-ld
  endif
endif
AR        := $(CROSS)ar
OBJDUMP   := $(CROSS)objdump
OBJCOPY   := $(CROSS)objcopy
ADDR2LINE := addr2line

ifeq ($(QUALITY_CONFIG_ONLY),)
ifeq ($(LD), tools/mips64-elf-ld)
  ifeq ($(shell ls -la tools/mips64-elf-ld | awk '{print $1}' | grep x),)
    $(warning [ERROR]: A required file in this repository is no longer executable.)
    $(error *    Please run: 'chmod +x tools/mips64-elf-ld', then run `make` again)
  endif
endif
endif

ifeq ($(TARGET_N64),1)
  TARGET_CFLAGS := -nostdinc -DTARGET_N64 -D_LANGUAGE_C
  CC_CFLAGS := -fno-builtin
endif

INCLUDE_DIRS += include $(BUILD_DIR) $(BUILD_DIR)/include src .
ifeq ($(TARGET_N64),1)
  INCLUDE_DIRS += include/libc
endif

C_DEFINES := $(foreach d,$(DEFINES),-D$(d))
DEF_INC_CFLAGS := $(foreach i,$(INCLUDE_DIRS),-I$(i)) $(C_DEFINES)

# Use the production GCC toolchain for preprocessing as well as compilation.
CPP := $(CROSS)gcc
CPPFLAGS := -E -P -x c -Wno-trigraphs $(DEF_INC_CFLAGS)

ifneq ($(DRY_RUN),)
ifeq ($(QUALITY_CONFIG_ONLY),)
# Included makefiles are rebuilt even under -n; read the rules without creating a file.
DRY_LEVEL_RULES := $(shell $(CPP) $(CPPFLAGS) $(VERSION_CFLAGS) -I . levels/level_rules.mk)
ifneq ($(.SHELLSTATUS),0)
  $(error Failed to inspect level make rules)
endif
$(eval $(DRY_LEVEL_RULES))
endif
endif

# C compiler options
SMALL_DATA_THRESHOLD := 0
CFLAGS = -std=gnu17 -G $(SMALL_DATA_THRESHOLD) $(OPT_FLAGS) $(TARGET_CFLAGS) $(MIPSISET) $(DEF_INC_CFLAGS)
ifeq ($(QUALITY_CLANG),0)
  CFLAGS += -mno-shared -march=vr4300 -mfix4300 -mabi=32 -mhard-float -mdivide-breaks -fno-stack-protector -fno-common -fno-zero-initialized-in-bss -fno-PIC -mno-abicalls -fno-strict-aliasing -fno-inline-functions -ffreestanding -fwrapv -Wall -Wextra
  CFLAGS += -Wno-missing-braces
  # Fold eligible side-effect-free boolean operations without a short-circuit branch.
  CFLAGS += --param=logical-op-non-short-circuit=1
  # Recompile all affected C/C++ objects when these global settings change.
  COMPILER_FLAG_O_FILES := $(addprefix $(BUILD_DIR)/,$(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o)) $(GODDARD_O_FILES) $(GENERATED_C_FILES:.c=.o)
  $(COMPILER_FLAG_O_FILES): Makefile
else
  # Clang flags are only returned by the read-only quality-flags target.
  CFLAGS += -mfpxx -target mips -mabi=32 -G 0 -mhard-float -fomit-frame-pointer -fno-stack-protector -fno-common -I include -I src/ -I $(BUILD_DIR)/include -fno-PIC -mno-abicalls -fno-strict-aliasing -fno-inline-functions -ffreestanding -fwrapv -Wall -Wextra
  CFLAGS += -Wno-missing-braces
endif
ASMFLAGS = -G 0 $(OPT_FLAGS) $(TARGET_CFLAGS) -mips3 $(DEF_INC_CFLAGS) -mno-shared -march=vr4300 -mfix4300 -mabi=32 -mhard-float -mdivide-breaks -fno-stack-protector -fno-common -fno-zero-initialized-in-bss -fno-PIC -mno-abicalls -fno-strict-aliasing -fno-inline-functions -ffreestanding -fwrapv -Wall -Wextra

ifeq ($(ENGINE_G16),1)
  ENGINE_G16_C_FILES := $(filter src/game/%.c src/engine/%.c,$(C_FILES))
  ENGINE_G16_O_FILES := $(addprefix $(BUILD_DIR)/,$(ENGINE_G16_C_FILES:.c=.o))
  $(ENGINE_G16_O_FILES): SMALL_DATA_THRESHOLD := 16
  $(ENGINE_G16_O_FILES): CFLAGS += -DENGINE_SMALL_DATA=1
  $(ENGINE_G16_O_FILES): CFLAGS += -mframe-header-opt
  # Keep resident function entries aligned to the 32-byte instruction-cache line.
  $(ENGINE_G16_O_FILES): CFLAGS += -fmin-function-alignment=32
  # Recompile resident objects when these compiler settings change.
  $(ENGINE_G16_O_FILES): Makefile

  # Match the bank expected by resident extern references without changing
  # boot/audio code to GP-relative addressing.
  GP_PROVIDER_C_FILES := $(filter src/boot/%.c src/audio/%.c,$(C_FILES))
  GP_PROVIDER_O_FILES := $(addprefix $(BUILD_DIR)/,$(GP_PROVIDER_C_FILES:.c=.o))
  $(GP_PROVIDER_O_FILES): SMALL_DATA_THRESHOLD := 16
  $(GP_PROVIDER_O_FILES): CFLAGS += -mno-gpopt
  $(GP_PROVIDER_O_FILES): Makefile
endif

ifeq ($(ENGINE_LTO),1)
  $(ENGINE_LTO_O_FILES): CFLAGS += -flto=1 -fno-fat-lto-objects
  # Apply shared safety/size flags at pre-link, retaining per-unit compile profiles.
  $(ENGINE_LTO_OBJECT): private OPT_FLAGS = $(DEFAULT_OPT_FLAGS) $(SAFETY_OPT_FLAGS) -ffunction-sections -fdata-sections
  $(ENGINE_LTO_OBJECT): $(ENGINE_LTO_O_FILES) Makefile
	$(call print,Pre-linking resident engine LTO:,$@)
	$(V)$(CC) $(CFLAGS) -flto=1 -flto-partition=balanced -flinker-output=nolto-rel -r -nostdlib -no-pie -Wl,--build-id=none -o $@ $(ENGINE_LTO_O_FILES)
endif

ASFLAGS     := -march=vr4300 -mabi=32 $(foreach i,$(INCLUDE_DIRS),-I$(i)) $(foreach d,$(DEFINES),--defsym $(d))
RSPASMFLAGS := $(foreach d,$(DEFINES),-definelabel $(subst =, ,$(d)))

# C preprocessor flags

#==============================================================================#
# Miscellaneous Tools                                                          #
#==============================================================================#

# N64 tools
LZ4TPACK              := $(TOOLS_DIR)/lz4tpack
FILESIZER             := $(TOOLS_DIR)/filesizer
N64CKSUM              := $(TOOLS_DIR)/n64cksum
N64GRAPHICS           := $(TOOLS_DIR)/n64graphics
TEXTCONV              := $(TOOLS_DIR)/textconv
AIFF_EXTRACT_CODEBOOK := $(TOOLS_DIR)/aiff_extract_codebook
VADPCM_ENC            := $(TOOLS_DIR)/vadpcm_enc
EXTRACT_DATA_FOR_MIO  := $(TOOLS_DIR)/extract_data_for_mio
SKYCONV               := $(TOOLS_DIR)/skyconv
FIXLIGHTS_PY          := $(TOOLS_DIR)/fixlights.py
FLIPS                 := $(TOOLS_DIR)/flips
# Use the system installed armips if available. Otherwise use the one provided with this repository.
ifneq (,$(call find-command,armips))
  RSPASM              := armips
else
  RSPASM              := $(TOOLS_DIR)/armips
endif

ifneq (,$(call find-command,wslview))
    EMULATOR = "/mnt/c/Program Files (x86)/parallel-launcher/parallel-launcher.exe"
else
    EMULATOR = parallel-launcher
endif

EMU_FLAGS =

# Adding a txt file to this location will then reference a UNFLoader path specified in the file, instead of locally.
# This is expecially important for WSL users because UNFLoader.exe is incredibly slow when run within WSL's filesystem, so this can be used to point to the C drive.
# The file should only contain the directory path that contains UNFLoader[.exe] (do not specify the filename).
LOADER_DIR_FILE_SPECIFICATION_PATH = ~/.local/share/HackerSM64/UNFLoader-dir.txt
LOADER_DIR = ./$(TOOLS_DIR)

ifneq (,$(wildcard $(LOADER_DIR_FILE_SPECIFICATION_PATH)))
  LOADER_DIR = $(shell cat $(LOADER_DIR_FILE_SPECIFICATION_PATH))
endif
ifneq (,$(call find-command,wslview))
  UNFLOADER_EXEC = $(LOADER_DIR)/UNFLoader.exe
else
  UNFLOADER_EXEC = $(LOADER_DIR)/UNFLoader
endif

SHA1SUM = sha1sum
PRINT = printf

ifeq ($(COLOR),1)
NO_COL  := \033[0m
RED     := \033[0;31m
GREEN   := \033[0;32m
BLUE    := \033[0;34m
YELLOW  := \033[0;33m
BLINK   := \033[32;5m
endif

# For non-IDO, use objcopy instead of extract_data_for_mio
ifneq ($(COMPILER),ido)
  EXTRACT_DATA_FOR_MIO := $(OBJCOPY) -O binary --only-section=.data
endif

# Common build print status function
define print
  @$(PRINT) "$(GREEN)$(1) $(YELLOW)$(2)$(GREEN) -> $(BLUE)$(3)$(NO_COL)\n"
endef

#==============================================================================#
# Main Targets                                                                 #
#==============================================================================#

all: $(ROM)
	@$(SHA1SUM) $(ROM)
	@$(PRINT) "${BLINK}Build succeeded.\n$(NO_COL)"
	@$(PRINT) "==== Build Options ====$(NO_COL)\n"
	@$(PRINT) "${GREEN}Version:        $(BLUE)$(VERSION)$(NO_COL)\n"
	@$(PRINT) "${GREEN}Microcode:      $(BLUE)$(GRUCODE)$(NO_COL)\n"
	@$(PRINT) "${GREEN}Console:        $(BLUE)$(CONSOLE)$(NO_COL)\n"

clean:
	$(RM) -r $(BUILD_DIR_BASE)

rebuildtools:
	"$(MAKE)" -C tools distclean
	"$(MAKE)" -C tools

distclean: clean
	$(PYTHON) extract_assets.py --clean
	"$(MAKE)" -C $(TOOLS_DIR) clean

test: $(ROM)
	$(EMULATOR) $(EMU_FLAGS) $<

test-pj64: $(ROM)
	wine ~/Desktop/new64/Project64.exe $<
# someone2639

# download and extract most recent unfloader build if needed
$(UNFLOADER_EXEC):
ifeq (,$(wildcard $(UNFLOADER_EXEC)))
	@$(PRINT) "Downloading latest UNFLoader...$(NO_COL)\n"
	$(PYTHON) $(TOOLS_DIR)/get_latest_unfloader.py $(LOADER_DIR)
endif

load: $(ROM) $(UNFLOADER_EXEC)
	$(UNFLOADER_EXEC) -r $<

unf: $(ROM) $(UNFLOADER_EXEC)
	$(UNFLOADER_EXEC) -d -r $<

libultra: $(BUILD_DIR)/libultra.a

patch: $(ROM)
  ifeq ($(shell uname), Darwin)
    ifeq ($(MAKECMDGOALS), patch)
      $(error "The 'make patch' command is not supported on macOS.")
    endif
  else
	  $(FLIPS) --create --bps "$$($(PYTHON) tools/detect_baseroms.py $(VERSION))" "$(ROM)" "$(BUILD_DIR)/$(TARGET_STRING).bps"
  endif

# Extra object file dependencies
$(BUILD_DIR)/asm/ipl3.o:              $(IPL3_RAW_FILES)
$(BUILD_DIR)/src/game/fasttext.o:     $(FASTTEXT_TEXTURE_C_FILES)
$(BUILD_DIR)/src/debugger/crash_screen.o: $(CRASH_TEXTURE_C_FILES)
$(BUILD_DIR)/src/game/version.o:      $(BUILD_DIR)/src/game/version_data.h
$(BUILD_DIR)/lib/aspMain.o:           $(BUILD_DIR)/rsp/audio.bin $(BUILD_DIR)/rsp/audio_data.bin
$(SOUND_BIN_DIR)/sound_data.o:        $(SOUND_BIN_DIR)/sound_data.ctl $(SOUND_BIN_DIR)/sound_data.tbl $(SOUND_BIN_DIR)/sequences.bin $(SOUND_BIN_DIR)/bank_sets sound/pcm/dunkey_pause.s16be sound/pcm/dunkey_pause_30k.s16be sound/pcm/yoshi_coin_cake.s16be sound/pcm/yoshi_coin_cake_30k.s16be
$(SOUND_BIN_DIR)/sound_data.o: sound/pcm/simpleflips_scream.s16be sound/pcm/simpleflips_scream_30k.s16be
$(BUILD_DIR)/levels/scripts.o:        $(BUILD_DIR)/include/level_headers.h

ifeq ($(VERSION),sh)
  $(BUILD_DIR)/src/audio/load_sh.o: $(SOUND_BIN_DIR)/bank_sets.inc.c $(SOUND_BIN_DIR)/sequences_header.inc.c $(SOUND_BIN_DIR)/ctl_header.inc.c $(SOUND_BIN_DIR)/tbl_header.inc.c
endif

$(CRASH_TEXTURE_C_FILES): TEXTURE_ENCODING := u32

ifeq ($(VERSION),eu)
  TEXT_DIRS := text/de text/us text/fr

  # EU encoded text inserted into individual segment 0x19 files,
  # and course data also duplicated in leveldata.c
  $(BUILD_DIR)/bin/eu/translation_en.o: $(BUILD_DIR)/text/us/define_text.inc.c
  $(BUILD_DIR)/bin/eu/translation_de.o: $(BUILD_DIR)/text/de/define_text.inc.c
  $(BUILD_DIR)/bin/eu/translation_fr.o: $(BUILD_DIR)/text/fr/define_text.inc.c
  $(BUILD_DIR)/levels/menu/leveldata.o: $(BUILD_DIR)/include/text_strings.h
  $(BUILD_DIR)/levels/menu/leveldata.o: $(BUILD_DIR)/text/us/define_courses.inc.c
  $(BUILD_DIR)/levels/menu/leveldata.o: $(BUILD_DIR)/text/de/define_courses.inc.c
  $(BUILD_DIR)/levels/menu/leveldata.o: $(BUILD_DIR)/text/fr/define_courses.inc.c
else
  ifeq ($(VERSION),sh)
    TEXT_DIRS := text/jp
    $(BUILD_DIR)/bin/segment2.o: $(BUILD_DIR)/text/jp/define_text.inc.c
  else
    TEXT_DIRS := text/$(VERSION)
    # non-EU encoded text inserted into segment 0x02
    $(BUILD_DIR)/bin/segment2.o: $(BUILD_DIR)/text/$(VERSION)/define_text.inc.c
  endif
endif

$(BUILD_DIR)/src/usb/usb.o: OPT_FLAGS := $(GENERATE_DWARF) -O0
$(BUILD_DIR)/src/usb/usb.o: CFLAGS += -Wno-unused-variable -Wno-sign-compare -Wno-unused-function
$(BUILD_DIR)/src/usb/debug.o: OPT_FLAGS := $(GENERATE_DWARF) -O0
$(BUILD_DIR)/src/usb/debug.o: CFLAGS += -Wno-unused-parameter -Wno-maybe-uninitialized
# File specific opt flags
$(BUILD_DIR)/src/audio/heap.o:          OPT_FLAGS := $(GENERATE_DWARF) $(GENERAL_OPTIMIZATION_LEVEL) -fno-jump-tables
$(BUILD_DIR)/src/audio/synthesis.o:     OPT_FLAGS := $(GENERATE_DWARF) $(GENERAL_OPTIMIZATION_LEVEL) -fno-jump-tables

$(BUILD_DIR)/src/engine/surface_collision.o:  OPT_FLAGS := $(COLLISION_OPT_FLAGS)
$(BUILD_DIR)/src/engine/math_util.o:          OPT_FLAGS := $(MATH_UTIL_OPT_FLAGS)
$(BUILD_DIR)/src/game/rendering_graph_node.o: OPT_FLAGS := $(GRAPH_NODE_OPT_FLAGS)

# $(info OPT_FLAGS:            $(OPT_FLAGS))
# $(info COLLISION_OPT_FLAGS:  $(COLLISION_OPT_FLAGS))
# $(info MATH_UTIL_OPT_FLAGS:  $(MATH_UTIL_OPT_FLAGS))
# $(info GRAPH_NODE_OPT_FLAGS: $(GRAPH_NODE_OPT_FLAGS))

ALL_DIRS := $(BUILD_DIR) $(addprefix $(BUILD_DIR)/,$(SRC_DIRS) $(GODDARD_SRC_DIRS) $(ULTRA_BIN_DIRS) $(BIN_DIRS) $(TEXTURE_DIRS) $(TEXT_DIRS) $(SOUND_SAMPLE_DIRS) $(addprefix levels/,$(LEVEL_DIRS)) rsp include) $(SOUND_BIN_DIR) $(SOUND_BIN_DIR)/sequences/$(VERSION)

# Make sure build directory exists before compiling anything
ifeq ($(QUALITY_CONFIG_ONLY)$(DRY_RUN),)
DUMMY != mkdir -p $(ALL_DIRS)
ifneq ($(.SHELLSTATUS),0)
  $(error Failed to create build directories)
endif
endif

$(BUILD_DIR)/include/text_strings.h: $(BUILD_DIR)/include/text_menu_strings.h
$(BUILD_DIR)/src/menu/file_select.o: $(BUILD_DIR)/include/text_strings.h
$(BUILD_DIR)/src/menu/star_select.o: $(BUILD_DIR)/include/text_strings.h
$(BUILD_DIR)/src/game/ingame_menu.o: $(BUILD_DIR)/include/text_strings.h
$(BUILD_DIR)/src/game/hud.o:         $(BUILD_DIR)/include/text_strings.h
INPUT_DISPLAY_TEXTURES := $(wildcard textures/segment2/input_display_*.rgba16.png)
$(BUILD_DIR)/src/game/input_display.o: $(addprefix $(BUILD_DIR)/,$(INPUT_DISPLAY_TEXTURES:.png=.inc.c))



#==============================================================================#
# Texture Generation                                                           #
#==============================================================================#
TEXTURE_ENCODING := u8

define convert_texture
$$(BUILD_DIR)/%$(1).inc.c: %$(1).png
	$$(call print,Converting:,$$<,$$@)
	$$(V)$$(N64GRAPHICS) -s $$(TEXTURE_ENCODING) -i $$@ -g $$< $(2)
$$(BUILD_DIR)/%$(1).preswap.inc.c: %$(1).preswap.png
	$$(call print,Converting:,$$<,$$@)
	$$(V)$$(N64GRAPHICS) -s $$(TEXTURE_ENCODING) -i $$@ -g $$< $(2) -S
endef

$(eval $(call convert_texture,,-f $$(lastword ,$$(subst ., ,$$*))))
$(eval $(call convert_texture,.ci4_rgba16,-p $$(patsubst %.inc.c,%.pal,$$@) -f ci4 -c rgba16))
$(eval $(call convert_texture,.ci8_rgba16,-p $$(patsubst %.inc.c,%.pal,$$@) -f ci8 -c rgba16))
$(eval $(call convert_texture,.ci4_ia16,-p $$(patsubst %.inc.c,%.pal,$$@) -f ci4 -c ia16))
$(eval $(call convert_texture,.ci8_ia16,-p $$(patsubst %.inc.c,%.pal,$$@) -f ci8 -c ia16))
$(eval $(call convert_texture,.ci4,-p $$(patsubst %.ci4.inc.c,%.rgba16.pal,$$@) -f ci4 -c rgba16))
$(eval $(call convert_texture,.ci8,-p $$(patsubst %.ci8.inc.c,%.rgba16.pal,$$@) -f ci8 -c rgba16))

WATER_SPLASH_CI8 := $(foreach frame,0 1 2 3 4 5 6 7,$(BUILD_DIR)/actors/water_splash/water_splash_$(frame).ci8.inc.c $(BUILD_DIR)/actors/water_splash/water_splash_$(frame).ci8.pal)
$(BUILD_DIR)/actors/group0.o: $(WATER_SPLASH_CI8)

# Both outputs come from the existing RGBA16 source, without palette quantization.
$(BUILD_DIR)/actors/water_splash/%.ci8.inc.c $(BUILD_DIR)/actors/water_splash/%.ci8.pal: actors/water_splash/%.rgba16.png
	$(call print,Converting:,$<,$@)
	$(V)$(N64GRAPHICS) -s $(TEXTURE_ENCODING) -i $(BUILD_DIR)/actors/water_splash/$*.ci8.inc.c -g $< -p $(BUILD_DIR)/actors/water_splash/$*.ci8.pal -f ci8 -c rgba16 -w 32 -h 64

# Convert image files to binary - used primarily for IPL3 "IA1" textures
$(BUILD_DIR)/%: %.png
	$(call print,Converting:,$<,$@)
	$(V)$(N64GRAPHICS) -s raw -i $@ -g $< -f $(lastword $(subst ., ,$@))


#==============================================================================#
# Compressed Segment Generation                                                #
#==============================================================================#

# Link segment file to resolve external labels
# HACKERSM64_DO: ideally this would be `-Trodata-segment=0x07000000` but that doesn't set the address
$(BUILD_DIR)/%.elf: $(BUILD_DIR)/%.o
	$(call print,Linking ELF file:,$<,$@)
	$(V)$(LD) -e 0 -Ttext=$(SEGMENT_ADDRESS) -Map $@.map -o $@ $<
# Override for leveldata.elf, which otherwise matches the above pattern.
# Has to be a static pattern rule for make-4.4 and above to trigger the second
# expansion.
.SECONDEXPANSION:
$(LEVEL_ELF_FILES): $(BUILD_DIR)/levels/%/leveldata.elf: $(BUILD_DIR)/levels/%/leveldata.o $(BUILD_DIR)/bin/$$(TEXTURE_BIN).elf
	$(call print,Linking ELF file:,$<,$@)
	$(V)$(LD) -e 0 -Ttext=$(SEGMENT_ADDRESS) -Map $@.map --just-symbols=$(BUILD_DIR)/bin/$(TEXTURE_BIN).elf -o $@ $<

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(call print,Extracting compressible data from:,$<,$@)
	$(V)$(EXTRACT_DATA_FOR_MIO) $< $@

$(BUILD_DIR)/levels/%/leveldata.bin: $(BUILD_DIR)/levels/%/leveldata.elf
	$(call print,Extracting compressible data from:,$<,$@)
	$(V)$(EXTRACT_DATA_FOR_MIO) $< $@

include compression/lz4trules.mk

#==============================================================================#
# Sound File Generation                                                        #
#==============================================================================#

$(BUILD_DIR)/%.table: %.aiff
	$(call print,Extracting codebook:,$<,$@)
	$(V)$(AIFF_EXTRACT_CODEBOOK) $< $@

$(BUILD_DIR)/%.aifc: $(BUILD_DIR)/%.table %.aiff
	$(call print,Encoding ADPCM:,$(word 2,$^),$@)
	$(V)$(VADPCM_ENC) -c $^ $@

$(SOUND_BIN_DIR)/sound_data.ctl: sound/sound_banks/ $(SOUND_BANK_FILES) $(SOUND_SAMPLE_AIFCS)
	@$(PRINT) "$(GREEN)Generating:  $(BLUE)$@ $(NO_COL)\n"
	$(V)$(PYTHON) $(TOOLS_DIR)/assemble_sound.py $(BUILD_DIR)/sound/samples/ sound/sound_banks/ $(SOUND_BIN_DIR)/sound_data.ctl $(SOUND_BIN_DIR)/ctl_header $(SOUND_BIN_DIR)/sound_data.tbl $(SOUND_BIN_DIR)/tbl_header $(C_DEFINES)

$(SOUND_BIN_DIR)/sound_data.tbl: $(SOUND_BIN_DIR)/sound_data.ctl
	@true

$(SOUND_BIN_DIR)/ctl_header: $(SOUND_BIN_DIR)/sound_data.ctl
	@true

$(SOUND_BIN_DIR)/tbl_header: $(SOUND_BIN_DIR)/sound_data.ctl
	@true

$(SOUND_BIN_DIR)/sequences.bin: $(SOUND_BANK_FILES) sound/sequences.json $(SOUND_SEQUENCE_DIRS) $(SOUND_SEQUENCE_FILES)
	@$(PRINT) "$(GREEN)Generating:  $(BLUE)$@ $(NO_COL)\n"
	$(V)$(PYTHON) $(TOOLS_DIR)/assemble_sound.py --sequences $@ $(SOUND_BIN_DIR)/sequences_header $(SOUND_BIN_DIR)/bank_sets sound/sound_banks/ sound/sequences.json $(SOUND_SEQUENCE_FILES) $(C_DEFINES)

$(SOUND_BIN_DIR)/bank_sets: $(SOUND_BIN_DIR)/sequences.bin
	@true

$(SOUND_BIN_DIR)/sequences_header: $(SOUND_BIN_DIR)/sequences.bin
	@true

$(SOUND_BIN_DIR)/%.m64: $(SOUND_BIN_DIR)/%.o
	$(call print,Converting to M64:,$<,$@)
	$(V)$(OBJCOPY) -j .rodata $< -O binary $@


#==============================================================================#
# Generated Source Code Files                                                  #
#==============================================================================#

# Convert binary file to a comma-separated list of byte values for inclusion in C code
$(BUILD_DIR)/%.inc.c: $(BUILD_DIR)/%
	$(call print,Converting to C:,$<,$@)
	$(V)hexdump -v -e '1/1 "0x%X,"' $< > $@
	$(V)echo >> $@

# Generate animation data
$(BUILD_DIR)/assets/mario_anim_data.c: $(wildcard assets/anims/*.inc.c) tools/mario_anims_converter.py tools/source_asset.py assets/rom/arrays.json.gz
	@$(PRINT) "$(GREEN)Generating animation data $(NO_COL)\n"
	$(V)$(PYTHON) $(TOOLS_DIR)/mario_anims_converter.py > $@

# Generate demo input data
$(BUILD_DIR)/assets/demo_data.c: assets/demo_data.json $(wildcard assets/demos/*.bin)
	@$(PRINT) "$(GREEN)Generating demo data $(NO_COL)\n"
	$(V)$(PYTHON) $(TOOLS_DIR)/demo_data_converter.py assets/demo_data.json $(DEF_INC_CFLAGS) > $@

# Encode in-game text strings
$(BUILD_DIR)/include/text_strings.h: include/text_strings.h.in
	$(call print,Encoding:,$<,$@)
	$(V)$(TEXTCONV) charmap.txt $< $@
$(BUILD_DIR)/include/text_menu_strings.h: include/text_menu_strings.h.in
	$(call print,Encoding:,$<,$@)
	$(V)$(TEXTCONV) charmap_menu.txt $< $@
$(BUILD_DIR)/text/%/define_courses.inc.c: text/define_courses.inc.c text/%/courses.h
	@$(PRINT) "$(GREEN)Preprocessing: $(BLUE)$@ $(NO_COL)\n"
	$(V)$(CPP) $(CPPFLAGS) $< -o - -I text/$*/ | $(TEXTCONV) charmap.txt - $@
$(BUILD_DIR)/text/%/define_text.inc.c: text/define_text.inc.c text/%/courses.h text/%/dialogs.h text/simpleflips_dialogs.h include/simpleflips_messages.inc.h charmap.txt
	@$(PRINT) "$(GREEN)Preprocessing: $(BLUE)$@ $(NO_COL)\n"
	$(V)$(CPP) $(CPPFLAGS) $< -o - -I text/$*/ | $(TEXTCONV) charmap.txt - $@

# Level headers
$(BUILD_DIR)/include/level_headers.h: levels/level_headers.h.in levels/level_defines.h
	$(call print,Preprocessing level headers:,$<,$@)
	$(V)$(CPP) $(CPPFLAGS) -I . $< | sed -E 's|(.+)|#include "\1"|' > $@

# Generate version_data.h
$(BUILD_DIR)/src/game/version_data.h: tools/make_version.sh
	@$(PRINT) "$(GREEN)Generating:  $(BLUE)$@ $(NO_COL)\n"
	$(V)sh tools/make_version.sh $(CROSS) > $@

#==============================================================================#
# Compilation Recipes                                                          #
#==============================================================================#

# Compile C code
ifeq ($(QUALITY_CONFIG_ONLY)$(DRY_RUN),)
ifeq ($(FIXLIGHTS),1)
# This must not be run multiple times at once, so we run it ahead of time rather than in a rule
DUMMY != $(PYTHON) $(FIXLIGHTS_PY) actors
ifneq ($(.SHELLSTATUS),0)
  $(error Failed to convert actor lighting)
endif
DUMMY != $(PYTHON) $(FIXLIGHTS_PY) levels
ifneq ($(.SHELLSTATUS),0)
  $(error Failed to convert level lighting)
endif
endif
endif
$(BUILD_DIR)/%.o: %.c
	$(call print,Compiling:,$<,$@)
	$(V)$(CC) -c $(CFLAGS) -MMD -MP -MF $(BUILD_DIR)/$*.d  -o $@ $<
$(BUILD_DIR)/%.o: %.cpp
	$(call print,Compiling (C++):,$<,$@)
	$(V)$(CXX) -c $(CFLAGS) -std=c++17 -Wno-register -MMD -MP -MF $(BUILD_DIR)/$*.d  -o $@ $<
$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.c
	$(call print,Compiling:,$<,$@)
	$(V)$(CC) -c $(CFLAGS) -MMD -MP -MF $(BUILD_DIR)/$*.d  -o $@ $<

# Assemble assembly code
$(BUILD_DIR)/%.o: %.s
	$(call print,Assembling:,$<,$@)
	$(V)$(CROSS)gcc -c $(ASMFLAGS) $(foreach i,$(INCLUDE_DIRS),-Wa,-I$(i)) -x assembler-with-cpp -MMD -MP -MF $(BUILD_DIR)/$*.d  -o $@ $<

# Assemble RSP assembly code
$(BUILD_DIR)/rsp/%.bin $(BUILD_DIR)/rsp/%_data.bin: rsp/%.s
	$(call print,Assembling:,$<,$@)
	$(V)$(RSPASM) -sym $@.sym $(RSPASMFLAGS) -strequ CODE_FILE $(BUILD_DIR)/rsp/$*.bin -strequ DATA_FILE $(BUILD_DIR)/rsp/$*_data.bin $<

# Run linker script through the C preprocessor
$(BUILD_DIR)/$(LD_SCRIPT): $(LD_SCRIPT) $(BUILD_DIR)/goddard.txt
	$(call print,Preprocessing linker script:,$<,$@)
	$(V)$(CPP) $(CPPFLAGS) -DENGINE_LTO=$(ENGINE_LTO) -DBUILD_DIR=$(BUILD_DIR) -DULTRALIB=lib$(ULTRALIB) -MMD -MP -MT $@ -MF $@.d -o $@ $<

# Link libgoddard
$(BUILD_DIR)/libgoddard.a: $(GODDARD_O_FILES)
	@$(PRINT) "$(GREEN)Linking libgoddard:  $(BLUE)$@ $(NO_COL)\n"
	$(V)$(AR) rcs -o $@ $(GODDARD_O_FILES)

# SS2: Goddard rules to get size
$(BUILD_DIR)/sm64_prelim.ld: sm64.ld $(LINK_O_FILES) $(YAY0_OBJ_FILES) $(SEG_FILES) $(BUILD_DIR)/libgoddard.a
	$(call print,Preprocessing preliminary linker script:,$<,$@)
	$(V)$(CPP) $(CPPFLAGS) -DENGINE_LTO=$(ENGINE_LTO) -DPRELIMINARY=1 -DBUILD_DIR=$(BUILD_DIR) -DULTRALIB=lib$(ULTRALIB) -MMD -MP -MT $@ -MF $@.d -o $@ $<

$(BUILD_DIR)/sm64_prelim.elf: $(BUILD_DIR)/sm64_prelim.ld
	@$(PRINT) "$(GREEN)Linking Preliminary ELF file:  $(BLUE)$@ $(NO_COL)\n"
	$(V)$(LD) --gc-sections $(ENGINE_G16_LDFLAGS) -L $(BUILD_DIR) -T $< -Map $(BUILD_DIR)/sm64_prelim.map --no-check-sections $(addprefix -R ,$(SEG_FILES)) -o $@ $(LINK_O_FILES) $(GCC_PROFILE_LIB) -L$(LIBS_DIR) -l$(ULTRALIB) -Llib $(LINK_LIBRARIES) -u sprintf -u osMapTLB -Llib/gcclib/$(LIBGCCDIR) -lgcc

$(BUILD_DIR)/goddard.txt: $(BUILD_DIR)/sm64_prelim.elf
	$(call print,Getting Goddard size...)
	$(V)python3 tools/getGoddardSize.py $(BUILD_DIR)/sm64_prelim.map $(BUILD_DIR)

$(SYMBOL_TABLE): $(BUILD_DIR)/sm64_prelim.elf
	$(call print,Generating symbol table:,$(@F))
	$(V)tools/n64sym $(DEBUG_EXPORT_ALL_LINES_FLAG) --objdump $(OBJDUMP) --addr2line $(ADDR2LINE) $(BUILD_DIR)/sm64_prelim.elf
	$(call print,Assembling:,$@)
	$(LD) -r -b binary -o $@ $(BUILD_DIR)/sm64_prelim.sym

# Link SM64 ELF file
$(ELF): $(BUILD_DIR)/sm64_prelim.elf $(SYMBOL_TABLE) $(LINK_O_FILES) $(YAY0_OBJ_FILES) $(SEG_FILES) $(BUILD_DIR)/$(LD_SCRIPT) $(BUILD_DIR)/libgoddard.a
	@$(PRINT) "$(GREEN)Linking ELF file:  $(BLUE)$@ $(NO_COL)\n"
	$(V)$(LD) --gc-sections $(ENGINE_G16_LDFLAGS) -L $(BUILD_DIR) -T $(BUILD_DIR)/$(LD_SCRIPT) -T goddard.txt -Map $(BUILD_DIR)/sm64.$(VERSION).map --no-check-sections $(addprefix -R ,$(SEG_FILES)) -o $@ $(LINK_O_FILES) $(GCC_PROFILE_LIB) -L$(LIBS_DIR) -l$(ULTRALIB) -Llib $(LINK_LIBRARIES) -u sprintf -u osMapTLB -Llib/gcclib/$(LIBGCCDIR) -lgcc

# Build ROM
$(ROM): $(ELF)
	$(call print,Building ROM:,$<,$@)
	$(V)$(OBJCOPY) --pad-to=0x101000 --gap-fill=0xFF $< $@ -O binary
	$(V)$(N64CKSUM) $@

$(BUILD_DIR)/$(TARGET).objdump: $(ELF)
	$(OBJDUMP) -D $< > $@

.PHONY: all clean distclean default test load rebuildtools
# with no prerequisites, .SECONDARY causes no intermediate target to be removed
.SECONDARY:

# Remove built-in rules, to improve performance
MAKEFLAGS += --no-builtin-rules

-include $(DEP_FILES)

# Read-only launcher metadata: use the same resolved options as the actual build.
.PHONY: print-build-paths
print-build-paths: ; $(info BUILD_ROM=$(ROM)) $(info BUILD_ELF=$(ELF)) $(info BUILD_AUDIO_TRIAL=$(AUDIO_TRIAL)) $(info BUILD_AUDIO_QUALITY_TRIAL=$(AUDIO_QUALITY_TRIAL)) $(info BUILD_AUDIO_LINEAR_ENVELOPES=$(AUDIO_LINEAR_ENVELOPES)) $(info BUILD_AUDIO_LOW_LATENCY=$(AUDIO_LOW_LATENCY)) $(info BUILD_GRUCODE=$(GRUCODE)) $(info BUILD_SAVETYPE=$(SAVETYPE)) $(info BUILD_FIRE_SPITTER_SLEEP_TRIAL=$(FIRE_SPITTER_SLEEP_TRIAL)) $(info BUILD_SKEETER_WATER_SLEEP_TRIAL=$(SKEETER_WATER_SLEEP_TRIAL)) $(info BUILD_ENEMY_PATROL_SLEEP_TRIAL=$(ENEMY_PATROL_SLEEP_TRIAL)) $(info BUILD_ENGINE_LTO=$(ENGINE_LTO)) $(info BUILD_ENGINE_G16=$(ENGINE_G16)) $(info BUILD_GCC_PROFILE=$(GCC_PROFILE)) @true

print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true

.PHONY: quality-flags
quality-flags: ; $(info QUALITY_CFLAGS=$(CFLAGS)) $(info QUALITY_C_FILES=$(C_FILES) $(GODDARD_C_FILES)) $(info QUALITY_BUILD_DIR=$(BUILD_DIR)) @true

# .incbin inputs are not discovered by the C preprocessor dependency scanner.
$(BUILD_DIR)/lib/gspF3DZEX2.NoN_PosLight.fifo.o: \
    lib/PR/f3dzex/fifo/F3DZEX_NoN.bin lib/PR/f3dzex/fifo/F3DZEX_NoN_data.bin
