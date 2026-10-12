#!/usr/bin/env python3
"""Production menu glyphs and HUD draw isolation; no emulator required."""

from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class PresentationTests(unittest.TestCase):
    def test_castle_pause_axes(self):
        menu = Source(ROOT / 'src/game/ingame_menu.c')
        options = Source(ROOT / 'src/game/game_options.c')
        stats = Source(ROOT / 'src/game/player_stats_menu.c')
        definitions = Source(ROOT / 'src/game/ingame_menu.h')
        fixture = r'''
#include <assert.h>
#include <stdint.h>
typedef int32_t s32; typedef uint32_t u32; typedef int16_t s16;
typedef int8_t s8; typedef uint8_t u8; typedef uint16_t u16;
#define MENU_OPT_DEFAULT 1
#define MENU_OPT_EXIT_COURSE 2
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define INPUT_DISPLAY
#define INPUT_DISPLAY_CONTENT_LEFT 112
#define DIALOG_STATE_HORIZONTAL 2
#define HUD_LUT_GLOBAL 0
#define TEXT_PAUSE 0xff
#define MENU_OPT_STATISTICS 4
#define MENU_OPT_OPTIONS 5
#define MENU_SCROLL_VERTICAL 0
#define MENU_SCROLL_HORIZONTAL 1
#define MENU_SCROLL_HORIZONTAL_DPAD 2
#define MENU_SCROLL_VERTICAL_WRAP 3
#define MENU_MODE_RENDER_PAUSE_SCREEN 1
#define U_JPAD 0x800
#define D_JPAD 0x400
#define L_JPAD 0x200
#define R_JPAD 0x100
#define Z_TRIG 0x2000
#define TEXT_CONTINUE ASCII_TO_DIALOG('C'),ASCII_TO_DIALOG('O'),ASCII_TO_DIALOG('N'),ASCII_TO_DIALOG('T'),ASCII_TO_DIALOG('I'),ASCII_TO_DIALOG('N'),ASCII_TO_DIALOG('U'),ASCII_TO_DIALOG('E'),0xff
#define TEXT_EXIT_COURSE ASCII_TO_DIALOG('E'),ASCII_TO_DIALOG('X'),ASCII_TO_DIALOG('I'),ASCII_TO_DIALOG('T'),DIALOG_CHAR_SPACE,ASCII_TO_DIALOG('C'),ASCII_TO_DIALOG('O'),ASCII_TO_DIALOG('U'),ASCII_TO_DIALOG('R'),ASCII_TO_DIALOG('S'),ASCII_TO_DIALOG('E'),0xff
#define TEXT_COIN_X 0x30,0xff
#define TEXT_STAR_X 0x31,0xff
#define LANGUAGE_ARRAY(x) (x)
#define COURSE_MIN 1
#define COURSE_STAGES_MAX 15
#define COURSE_BONUS_STAGES 16
#define COURSE_MAX 25
#define COURSE_NUM_TO_INDEX(x) ((x)-1)
#define MENU_MTX_PUSH 0
#define SOUND_MENU_PAUSE_CHANGE_SELECT 1
#define SOUND_MENU_CHANGE_SELECT 2
#define gSPDisplayList(...) ((void)0)
#define gDPSetEnvColor(...) ((void)0)
#define gSPPopMatrix(...) ((void)0)
enum {GO_MENU_NAVIGATION,GO_MENU_STICK_RELEASE};
static s32 navigation=2,requireRelease;
static s32 game_options_get(s32 id) {return id==GO_MENU_NAVIGATION ? navigation : requireRelease;}
static struct {s16 rawStickX,rawStickY;u16 buttonDown;} controller,*gPlayer1Controller=&controller;
static u32 gGlobalTimer,sPauseCastleOptionsFrame;
static s8 sPauseCastleSelection=MENU_OPT_DEFAULT,gDialogLineNum;
static u8 sPauseCastleOptionsHeld,sPauseCastleOptionsStick,sMenuNavigationStick,gMenuHoldKeyIndex,sMenuNavigationDirection;
static s32 gMenuMode=MENU_MODE_RENDER_PAUSE_SCREEN,gGlobalSoundSource;
static s16 gCurrSaveFileNum=1,gInGameLanguage;
static void *courseNames[26];
static void **languageTable[1][2]={{0,courseNames}};
static void *segmented_to_virtual(void *pointer) {return pointer;}
static s32 sounds,draws;
static s16 drawX[4],drawY[4],drawWidth[4],cursorX,cursorY,titleX,titleY;
static s32 overlay;
static s32 gDialogBoxState=DIALOG_STATE_HORIZONTAL;
static s32 input_display_enabled(void) {return overlay;}
extern u8 gDialogCharWidths[256];
static s32 get_string_width(u8 *text) {
    s32 width=0;
    while (*text!=0xff) width+=gDialogCharWidths[*text++];
    return width;
}
static s16 get_str_x_pos_from_center(s16 center,u8 *text,float scale) {
    (void)scale;return center-get_string_width(text)/2;
}
static void print_hud_lut_string(s8 lut,s16 x,s16 y,const u8 *text) {
    (void)lut;(void)text;titleX=x;titleY=y;
}
static void play_sound(s32 sound,s32 source) {(void)sound;(void)source;sounds++;}
static void record_text(s16 x,s16 y,s16 width) {
    if (draws<4) {drawX[draws]=x;drawY[draws]=y;drawWidth[draws]=width;} draws++;
}
static void print_generic_string(s16 x,s16 y,const u8 *text) {record_text(x,y,get_string_width((u8 *)text));}
static void create_dl_translation_matrix(s32 push,s16 x,s16 y,s32 z) {
    (void)push;(void)z;cursorX=x;cursorY=y;
}
static s32 save_file_get_course_star_count(s32 file,s32 course) {
    (void)file;return course==0 || course==2;
}
static s32 save_file_get_course_coin_score(s32 file,s32 course) {(void)file;(void)course;return 0;}
static s32 save_file_get_total_star_count(s32 file,s32 low,s32 high) {(void)file;(void)low;(void)high;return 0;}
static void render_pause_castle_course_stars(s16 x,s16 y,s16 file,s16 course) {(void)x;(void)y;(void)file;(void)course;}
static void int_to_str(s32 number,u8 *text) {(void)number;text[0]=0xff;}
'''
        fixture += definitions.macros('ASCII_TO_DIALOG|DIALOG_CHAR_PLUS|DIALOG_CHAR_FORWARD_SLASH')
        fixture += definitions.declaration('enum', 'DialogMark')
        fixture += definitions.declaration('enum', 'DialogSpecialChars')
        fixture += menu.region(r'^u8 gDialogCharWidths', r'^s8 gDialogBoxState')
        fixture += menu.macros('PAUSE_CASTLE_OPTIONS_Y|Y_VAL8')
        fixture += stats.macros('STATS_MENU_TEXT_SIZE')
        fixture += Source(ROOT / 'src/game/game_options.h').macros('GO_MENU_WAIT_FOR_NEUTRAL')
        for name in ['menu_glyph', 'menu_encode_text', 'player_stats_menu_text_centered']:
            fixture += stats.function(name)
        for name in ['game_options_menu_direction', 'game_options_menu_stick', 'game_options_menu_dpad']:
            fixture += options.function(name)
        for name in ['handle_menu_scrolling', 'pause_menu_center_x', 'print_hud_pause_colorful_str',
                     'render_pause_castle_options', 'render_pause_course_options',
                     'render_pause_castle_main_strings']:
            fixture += menu.function(name)
        fixture += r'''
static void sample(u16 buttons,s16 x,s16 y) {
    controller.buttonDown=buttons;controller.rawStickX=x;controller.rawStickY=y;
    gGlobalTimer++;draws=0;
    print_hud_pause_colorful_str();
    render_pause_castle_options();
    assert(draws==3);
    s16 center=overlay ? 216 : 160;
    assert(titleX+32==center);
    for(s32 i=0;i<3;i++) assert(drawX[i]+drawWidth[i]/2==center);
    assert(SCREEN_HEIGHT-drawY[0]-16-(titleY+16)>=10);
    assert(drawY[0]>drawY[1] && drawY[1]>drawY[2]);
    s32 row=sPauseCastleSelection==1 ? 0 : sPauseCastleSelection==4 ? 1 : 2;
    assert(cursorX<drawX[row] && cursorY==drawY[row]+1);
    render_pause_castle_main_strings(104,60);
}
int main(void) {
    static u8 courseName[]={0,0,0,0x12,0xff};
    for(s32 course=0;course<26;course++) courseNames[course]=courseName;
    sample(0,0,0);
    sample(D_JPAD,0,0);assert(sPauseCastleSelection==4 && gDialogLineNum==0);
    sample(D_JPAD,0,0);assert(sPauseCastleSelection==4); // Held direction is one press.
    sample(0,0,0);sample(D_JPAD,0,0);assert(sPauseCastleSelection==5);
    sample(0,0,0);sample(D_JPAD,0,0);assert(sPauseCastleSelection==1); // Wrap.
    sample(0,0,0);sample(U_JPAD,0,0);assert(sPauseCastleSelection==5 && gDialogLineNum==0);
    sample(0,0,0);sample(R_JPAD,0,0);assert(sPauseCastleSelection==5 && gDialogLineNum==2); // Skip empty course.
    sample(0,0,0);sample(L_JPAD,0,0);assert(sPauseCastleSelection==5 && gDialogLineNum==0);
    sample(0,0,0);sample(L_JPAD,0,0);assert(gDialogLineNum==15); // Secret stars wrap.
    sample(0,0,0);sample(R_JPAD,0,0);assert(gDialogLineNum==0);
    sample(0,0,0);sample(Z_TRIG|R_JPAD,0,0);assert(gDialogLineNum==0 && sPauseCastleSelection==5);
    sample(0,0,0);sample(0,0,80);assert(sPauseCastleSelection==4 && gDialogLineNum==0);
    sample(0,0,0);sample(0,80,0);assert(gDialogLineNum==2 && sPauseCastleSelection==4);
    navigation=1;sample(0,0,0);sample(0,0,-80);assert(sPauseCastleSelection==4);
    navigation=0;sample(0,0,0);sample(D_JPAD,0,0);assert(sPauseCastleSelection==4);
    assert(sounds>0);
    navigation=2;
    requireRelease=1;sPauseCastleSelection=1;sample(0,0,0);
    sample(D_JPAD,0,0);assert(sPauseCastleSelection==4);
    for(s32 frame=0;frame<30;frame++) {sample(D_JPAD,0,0);assert(sPauseCastleSelection==4);}
    sample(U_JPAD,0,0);assert(sPauseCastleSelection==4);
    sample(0,0,0);sample(U_JPAD,0,0);assert(sPauseCastleSelection==1);
    requireRelease=0;
    for(overlay=0;overlay<=1;overlay++) {
        const s8 castleChoices[]={MENU_OPT_DEFAULT,MENU_OPT_STATISTICS,MENU_OPT_OPTIONS};
        const s8 courseChoices[]={MENU_OPT_DEFAULT,MENU_OPT_EXIT_COURSE,MENU_OPT_STATISTICS,MENU_OPT_OPTIONS};
        for(s32 i=0;i<3;i++) {sPauseCastleSelection=castleChoices[i];sample(0,0,0);}
        for(s32 i=0;i<4;i++) {
            draws=0;gDialogLineNum=courseChoices[i];
            render_pause_course_options(pause_menu_center_x(),93,&gDialogLineNum,15);
            assert(draws==4 && gDialogLineNum==courseChoices[i]);
            for(s32 row=0;row<4;row++) assert(drawX[row]+drawWidth[row]/2==(overlay ? 216 : 160));
            assert(cursorX==drawX[i]-14 && cursorY==drawY[i]);
        }
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix='pause-axes-') as temporary:
            path = Path(temporary) / 'fixture.c'
            path.write_text(fixture)
            executable = Path(temporary) / 'test'
            subprocess.run(['cc', '-std=gnu99', '-Wall', '-Wextra', '-Werror',
                            '-fsanitize=address,undefined', "-fno-sanitize-recover=all", '-fno-omit-frame-pointer',
                            str(path), '-o', str(executable)], check=True, timeout=30)
            subprocess.run([str(executable)], check=True,
                           env={**os.environ, 'ASAN_OPTIONS': 'detect_leaks=0'}, timeout=10)

    def test_independent_overlays_and_sprite_size(self):
        source = Source(ROOT / "src/game/input_display.c")
        fixture = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
typedef int32_t s32; typedef uint32_t u32; typedef float f32;
typedef uint8_t Texture; typedef int Gfx;
typedef uint8_t ColorRGBA[4];
#define FALSE 0
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define G_IM_FMT_RGBA 0
#define G_IM_SIZ_16b 0
#define bzero(p,n) memset(p,0,n)
#define gDPPipeSync(...) ((void)0)
#define gDPSetCycleType(...) ((void)0)
#define gDPSetTexturePersp(...) ((void)0)
#define gDPSetTextureFilter(...) ((void)0)
#define gDPSetRenderMode(...) ((void)0)
#define gDPSetCombineLERP(...) ((void)0)
#define gDPSetCombineMode(...) ((void)0)
#define gDPSetScissor(...) ((void)0)
enum { GO_INPUT_DISPLAY, GO_CONTROLLER_DISPLAY };
static s32 preferences[2];
static s32 game_options_get(s32 id) { return preferences[id]; }
static struct { s32 open; } gOptionsMenu;
static struct { s32 length; } sInputHistory = {1};
static s32 historyCalls,controllerCalls,lastMini;
static s32 draws;
static f32 drawnX,drawnY,drawnWidth,drawnHeight;
static uint8_t gCurrEnvCol[4];
static Gfx commands[32],*gDisplayListHead=commands;
struct InputDisplaySprite { const Texture *texture; uint16_t width,height; };
static const struct InputDisplaySprite probe = {0,96,64};
static s32 input_display_left(void) { return 0; }
static void print_set_envcolour(int r,int g,int b,int a) { (void)r;(void)g;(void)b;(void)a; }
static void draw_sprite(Gfx **dl,const void *texture,s32 fmt,s32 size,s32 bilerp,
                        u32 width,u32 height,f32 x,f32 y,f32 w,f32 h) {
    assert(dl==&gDisplayListHead && texture==0 && fmt==0 && size==0 && bilerp==FALSE);
    assert(width==96 && height==64);
    draws++;drawnX=x;drawnY=y;drawnWidth=w;drawnHeight=h;
}
'''
        fixture += source.function("input_display_enabled")
        fixture += source.function("input_display_sprite")
        fixture += r'''
static void input_display_history(void) { historyCalls++; }
static void input_display_controller(s32 miniController) {
    controllerCalls++;lastMini=miniController;
    input_display_sprite(&probe,6,160,96,64);
}
'''
        fixture += source.function("render_input_display")
        fixture += r'''
int main(void) {
    render_input_display();
    assert(!draws && !input_display_enabled());
    preferences[GO_CONTROLLER_DISPLAY]=1;
    render_input_display();
    assert(draws==1 && controllerCalls==1 && !historyCalls && !lastMini);
    assert(drawnX==6 && drawnY==158 && drawnWidth==96 && drawnHeight==64);
    preferences[GO_CONTROLLER_DISPLAY]=2;
    render_input_display();
    assert(draws==2 && controllerCalls==2 && !historyCalls && lastMini);
    assert(drawnX==6 && drawnY==158 && drawnWidth==96 && drawnHeight==64);
    preferences[GO_INPUT_DISPLAY]=1;
    render_input_display();
    assert(draws==3 && historyCalls==1 && controllerCalls==3 && lastMini);
    gOptionsMenu.open=1;
    render_input_display();
    assert(draws==3 && historyCalls==1 && controllerCalls==3);
    gOptionsMenu.open=0;
    sInputHistory.length=0;
    render_input_display();
    assert(draws==3 && historyCalls==1 && controllerCalls==3);
    return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix="overlay-options-") as temporary:
            folder = Path(temporary)
            path = folder / "fixture.c"
            path.write_text(fixture)
            executable = folder / "test"
            subprocess.run(["cc", "-std=gnu99", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=address,undefined", "-fno-sanitize-recover=all", "-fno-omit-frame-pointer",
                            str(path), "-o", str(executable)], check=True, timeout=30)
            subprocess.run([str(executable)], check=True,
                           env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"}, timeout=10)

    def test_mini_uses_native_pixels(self):
        from PIL import Image
        source = Source(ROOT / "src/game/input_display.c")
        fixture = r'''
#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
typedef int32_t s32; typedef uint32_t u32; typedef uint16_t u16; typedef int16_t s16; typedef float f32;
typedef uint8_t Texture; typedef int Gfx;
#define FALSE 0
#define G_IM_FMT_RGBA 0
#define G_IM_SIZ_16b 0
#define MAX(a,b) ((a)>(b)?(a):(b))
#define absf fabsf
#define gDPPipeSync(...) ((void)0)
#define gDPSetCycleType(...) ((void)0)
#define gDPSetTexturePersp(...) ((void)0)
#define gDPSetTextureFilter(...) ((void)0)
#define gDPSetRenderMode(...) ((void)0)
#define gDPSetCombineLERP(...) ((void)0)
#define L_TRIG 0x20
#define R_TRIG 0x10
#define START_BUTTON 0x1000
#define B_BUTTON 0x4000
#define A_BUTTON 0x8000
#define Z_TRIG 0x2000
#define INPUT_D_UP 1
#define INPUT_C_UP 9
static u16 gInputHistoryButtons[13]={0,0x800,0x100,0x400,0x200,0,0,0,0,8,1,2,4};
static f32 sInputStickX,sInputStickY,sInputStickMagnitude;
static u16 sStickAngleBinary=0x4000,sStickPowerPercent100=10000;
static s32 numbers,expectedSmall=1,textCalls;
enum { GO_CONTROLLER_NUMBERS };
static s32 game_options_get(s32 id) {assert(id==GO_CONTROLLER_NUMBERS);return numbers;}
struct InputHistoryEntry {u16 buttons;};
static struct {struct InputHistoryEntry entries[1];} sInputHistory;
#define FONT_OUTLINE 0
#define PRINT_TEXT_ALIGN_RIGHT 0
#define PRINT_ALL 0
static void input_display_text(const char *text,s32 x,s32 y) {
    assert(x==6 && y==227 && !strcmp(text,"090.00"));textCalls++;
}
static s32 get_text_width(const char *text,s32 font) {(void)font;return strlen(text)*6;}
static void print_small_text_light(s32 x,s32 y,const char *text,s32 align,s32 all,s32 font) {
    (void)align;(void)all;(void)font;
    assert(x==104 && y==225 && !strcmp(text,"100.00%"));textCalls++;
}
static Gfx command,*gDisplayListHead=&command;
static s32 draws;
static f32 capX,capY;
struct InputDisplaySprite { const Texture *texture; u16 width,height; };
static s32 input_display_left(void) {return 0;}
static void print_set_envcolour(int r,int g,int b,int a) {(void)r;(void)g;(void)b;(void)a;}
'''
        for name in ['base', 'start', 'a', 'b', 'z', 'stick', 'shoulder_l', 'shoulder_r',
                     'c_0', 'c_1', 'c_2', 'c_3', 'd_0', 'd_1', 'd_2', 'd_3']:
            with Image.open(ROOT / ('textures/segment2/input_display_mini_' + name + '.rgba16.png')) as image:
                width, height = image.size
            symbol = ''.join(part.capitalize() for part in name.split('_'))
            fixture += (f'static const Texture tex{symbol};\n'
                        f'static const struct InputDisplaySprite sInputDisplayMini{symbol}'
                        f'={{&tex{symbol},{width},{height}}};\n')
        for name in ['base', 'start', 'a', 'b', 'z', 'stick', 'shoulder_l', 'shoulder_r',
                     'c_0', 'c_1', 'c_2', 'c_3', 'd_0', 'd_1', 'd_2', 'd_3', 'degree']:
            with Image.open(ROOT / ('textures/segment2/input_display_' + name + '.rgba16.png')) as image:
                width, height = image.size
            symbol = ''.join(part.capitalize() for part in name.split('_'))
            fixture += (f'static const Texture texLarge{symbol};\n'
                        f'static const struct InputDisplaySprite sInputDisplay{symbol}'
                        f'={{&texLarge{symbol},{width},{height}}};\n')
        fixture += 'static const struct InputDisplaySprite *sCButtons[]={&sInputDisplayC0,&sInputDisplayC1,&sInputDisplayC2,&sInputDisplayC3};\n'
        fixture += 'static const struct InputDisplaySprite *sDButtons[]={&sInputDisplayD0,&sInputDisplayD1,&sInputDisplayD2,&sInputDisplayD3};\n'
        fixture += r'''
static void draw_sprite(Gfx **dl,const void *texture,s32 fmt,s32 size,s32 bilerp,
                        u32 width,u32 height,f32 x,f32 y,f32 w,f32 h) {
    assert(dl==&gDisplayListHead && fmt==0 && size==0 && bilerp==FALSE);
    assert(w==width && h==height);
    assert(x==floorf(x) && y==floorf(y));
    if (texture==&texLargeDegree) {
        assert(numbers && y==226); // Below both controller graphics.
    } else if (expectedSmall) {
        s32 shift=numbers ? -6 : 0;
        assert(x>=19 && x+w<=69 && y>=191+shift && y+h<=226+shift);
        if (!draws) {assert(texture==&texBase && width==50 && height==35);
            assert(x==19 && y==191+shift);}
    } else {
        assert(x>=6 && x+w<=106 && y>=157 && y+h<=226);
    }
    if (texture==&texStick) {capX=x;capY=y;}
    draws++;
}
'''
        for function in ['input_display_sprite', 'input_display_texture', 'input_display_mini_controller',
                         'input_display_controller_numbers', 'input_display_controller']:
            fixture += source.function(function)
        fixture += r'''
int main(void) {
    for (u32 buttons=0;buttons<=0xffff;buttons++) {
        draws=0;
        input_display_mini_controller(buttons);
        assert(draws>=2 && capX==27 && capY==199);
    }
    for (s32 x=-128;x<=127;x++) {
        for (s32 y=-128;y<=127;y++) {
            sInputStickX=x;sInputStickY=y;sInputStickMagnitude=sqrtf(x*x+y*y);
            draws=0;
            input_display_mini_controller(0);
            assert(draws==2 && fabsf(capX-27)<=3 && fabsf(capY-199)<=3);
            if (x||y) {assert(capX!=27 || capY!=199);}
        }
    }
    sInputStickX=sInputStickY=sInputStickMagnitude=0;
    for (s32 small=0;small<=1;small++) {
        expectedSmall=small;
        for (numbers=0;numbers<=1;numbers++) {
            textCalls=draws=0;
            input_display_controller(small);
            assert(textCalls==numbers*2 && draws==2+numbers);
            if (small) {assert(capX==27 && capY==(numbers ? 193 : 199));}
        }
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix="mini-pixels-") as temporary:
            path = Path(temporary) / 'fixture.c'
            path.write_text(fixture)
            executable = Path(temporary) / 'test'
            subprocess.run(['cc', '-std=gnu99', '-Wall', '-Wextra', '-Werror',
                            '-fsanitize=address,undefined', "-fno-sanitize-recover=all", '-fno-omit-frame-pointer',
                            str(path), '-lm', '-o', str(executable)], check=True, timeout=30)
            subprocess.run([str(executable)], check=True,
                           env={**os.environ, 'ASAN_OPTIONS': 'detect_leaks=0'}, timeout=10)

    def test_menu_glyphs_and_hud_isolation(self):
        menu_header = Source(ROOT / "src/game/ingame_menu.h")
        hud = Source(ROOT / "src/game/hud.c")
        fixture = r'''
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
typedef int16_t s16; typedef int32_t s32; typedef uint8_t u8; typedef float f32;
#define CAM_MOVE_PAUSE_SCREEN 1
#define HUD_DISPLAY_NONE 0
#define HUD_DISPLAY_FLAG_COIN_COUNT 1
#define HUD_DISPLAY_FLAG_STAR_COUNT 2
#define HUD_DISPLAY_FLAG_KEYS 4
#define HUD_DISPLAY_FLAG_CAMERA_AND_POWER 8
#define HUD_DISPLAY_FLAG_TIMER 16
#define POWER_METER_HIDDEN 0
#define POWER_METER_EMPHASIZED 1
#define POWER_METER_DEEMPHASIZING 2
#define POWER_METER_HIDING 3
#define POWER_METER_VISIBLE 4
#define HUD_POWER_METER_Y 200
#define CAMERA_MODE_INSIDE_CANNON 1
enum { GO_HUD, GO_INPUT_DISPLAY, GO_CONTROLLER_DISPLAY, GO_ASPECT_RATIO, GO_HUD_WIDE };
static s32 preferences[] = {1,0,0,0,0};
static s32 game_options_get(s32 id) { return preferences[id]; }
static struct { s32 open; } gOptionsMenu;
static struct { s16 flags,wedges; } gHudDisplay;
static struct { s32 animation; s16 y; } sPowerMeterHUD;
static s32 sPowerMeterStoredHealth, sPowerMeterVisibleTimer, gCameraMovementFlags;
static u8 sCounterAlpha=255, sCameraAlpha=255, sCapTimerAlpha=255;
static struct { s32 mode; } camera;
static struct { typeof(camera) *camera; } area = { &camera }, *gCurrentArea = &area;
static s32 draws,matrices,meterY;
static void create_dl_ortho_matrix(void) { matrices++; }
static void render_hud_cannon_reticle(void) { draws++; }
static void print_text_set_alpha(u8 alpha) { (void)alpha; }
static void render_hud_coins(void) { draws++; }
static void render_hud_stars(u8 alpha) { (void)alpha; draws++; }
static void render_hud_keys(void) { draws++; }
static void handle_power_meter_actions(s16 health) { (void)health; }
static void animate_power_meter_emphasized(void) { sPowerMeterHUD.y++; }
static void animate_power_meter_deemphasizing(void) { sPowerMeterHUD.y++; }
static void animate_power_meter_hiding(void) { sPowerMeterHUD.y++; }
static void render_dl_power_meter(s16 health) { (void)health; meterY=sPowerMeterHUD.y; draws++; }
static void render_hud_camera_status(u8 alpha) { (void)alpha; draws++; }
static void render_hud_timer(void) { draws++; }
static void render_hud_cap_timer(u8 alpha) { if (alpha) draws++; }
static void game_timing_render_hud(u8 alpha) { (void) alpha; }
'''
        fixture += menu_header.macros("ASCII_TO_DIALOG|DIALOG_CHAR_PLUS|DIALOG_CHAR_FORWARD_SLASH")
        fixture += menu_header.declaration("enum", "DialogMark")
        fixture += menu_header.declaration("enum", "DialogSpecialChars")
        fixture += Source(ROOT / "src/game/player_stats_menu.c").function("menu_glyph")
        fixture += Source(ROOT / "src/game/game_options.c").function("game_options_hud_inset")
        fixture += hud.function("hud_meter_draw_y")
        fixture += hud.function("render_hud_power_meter")
        fixture += hud.function("render_hud")
        fixture += r'''
int main(void) {
    assert(menu_glyph('%') == 0xF3);
    assert(menu_glyph('*') == DIALOG_CHAR_STAR_FILLED);
    assert(menu_glyph('>') == DIALOG_CHAR_SPACE);
    assert(menu_glyph('0') == 0 && menu_glyph('9') == 9);
    gHudDisplay.flags = 31;
    render_hud();
    assert(draws == 6);
    gOptionsMenu.open = 1;
    draws = 0;
    sPowerMeterStoredHealth = 3;
    sPowerMeterVisibleTimer = 17;
    sPowerMeterHUD.animation = 4;
    render_hud();
    assert(draws == 0 && gHudDisplay.flags == 31);
    assert(sPowerMeterStoredHealth == 3 && sPowerMeterVisibleTimer == 17);
    assert(sPowerMeterHUD.animation == 4);
    gHudDisplay.flags = 0;
    render_hud();
    assert(sPowerMeterHUD.animation == 4);
    gOptionsMenu.open = 0;
    render_hud();
    assert(sPowerMeterHUD.animation == POWER_METER_HIDDEN);
    assert(sPowerMeterStoredHealth == 8 && sPowerMeterVisibleTimer == 0);
    gHudDisplay.flags=31;
    preferences[GO_HUD]=2;
    sPowerMeterHUD.animation=POWER_METER_EMPHASIZED;
    sPowerMeterHUD.y=166;
    draws=0;
    render_hud();
    assert(draws==0 && sPowerMeterHUD.y==167 && sPowerMeterVisibleTimer==1);
    preferences[GO_HUD]=0;
    render_hud_power_meter();
    assert(draws==1 && meterY==200 && sPowerMeterHUD.y==168);
    sPowerMeterHUD.animation=POWER_METER_HIDDEN;
    sPowerMeterHUD.y=300;
    render_hud_power_meter();
    assert(draws==2 && meterY==200 && sPowerMeterHUD.y==300);
    assert(sPowerMeterHUD.animation==POWER_METER_HIDDEN);
    preferences[GO_HUD]=1;
    render_hud_power_meter();
    assert(draws==2);
    preferences[GO_ASPECT_RATIO]=1;
    assert(game_options_hud_inset()==40);
    assert(hud_meter_draw_y(200)==168 && hud_meter_draw_y(166)==166);
    preferences[GO_HUD_WIDE]=1;
    assert(game_options_hud_inset()==0);
    assert(hud_meter_draw_y(200)==200);
    preferences[GO_ASPECT_RATIO]=0;
    preferences[GO_HUD_WIDE]=0;
    assert(game_options_hud_inset()==0);
    return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix="presentation-options-") as temporary:
            folder = Path(temporary)
            path = folder / "fixture.c"
            path.write_text(fixture)
            executable = folder / "test"
            subprocess.run(["cc", "-std=gnu99", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=address,undefined", "-fno-sanitize-recover=all", "-fno-omit-frame-pointer",
                            str(path), "-o", str(executable)], check=True, timeout=30)
            subprocess.run([str(executable)], check=True,
                           env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"}, timeout=10)


if __name__ == "__main__":
    unittest.main()
