"""Host checks for page fitting, scrolling, clipping and dialogue arrows."""
from pathlib import Path
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class DialogBoxSizeTests(unittest.TestCase):
    def test_page_height_and_render_controls(self):
        source = Source(ROOT / 'src/game/ingame_menu.c')
        functions = source.function('get_dialog_page_line_count') + source.function('get_dialog_box_line_count')
        tail = source.function('render_dialog_entries').split('    s8 boxLines', 1)[1]
        code = r'''
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef int32_t s32;
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define DIALOG_CHAR_NEWLINE 0xFE
#define DIALOG_CHAR_TERMINATOR 0xFF
#define DIALOG_CHAR_SPACE 0x9E
#define DIALOG_CHAR_SLASH 0xD0
#define DIALOG_CHAR_COLOR 0xDF
#define DIALOG_CHAR_DAKUTEN 0xF0
#define DIALOG_CHAR_PERIOD_OR_HANDAKUTEN 0xF1
#define DIALOG_STATE_VERTICAL 1
#define DIALOG_STATE_HORIZONTAL 2
#define DIALOG_STATE_OPENING 0
#define DIALOG_STATE_CLOSING 3
#define DIAG_VAL2 240
#define DIAG_VAL3 132
#define DIAG_VAL4 5
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define G_SC_NON_INTERLACE 0
struct DialogEntry { s8 linesPerBox; const u8 *str; int leftOffset, width; };
static int gDialogTextPos, gDialogBoxState, gLastDialogPageStrPos, gLastDialogResponse;
static int gLastDialogLineNum, gBorderHeight, boxHeight, nextHeight, choiceHeight, clipBottom, scissorCalls;
#define segmented_to_virtual(p) (p)
static int ensure_nonnegative(int v) { return MAX(v,0); }
#define gDPSetScissor(dl,mode,left,top,right,bottom) ((void)(mode),(void)(left),(void)(top),(void)(right),(scissorCalls++%2==0 ? (clipBottom=(bottom)) : 0))
static void render_dialog_box_type(struct DialogEntry *dialog, s8 lines, s16 width) { (void)width; (void)dialog; boxHeight=lines; }
static void handle_dialog_text_and_pages(int color, struct DialogEntry *dialog, int bound, int style) {
    (void)color; (void)dialog; (void)bound; (void)style; gLastDialogLineNum=99;
}
static void render_dialog_triangle_choice(void) { choiceHeight=gLastDialogLineNum; }
static void render_dialog_triangle_next(s8 lines) { nextHeight=lines; }
/* FUNCTIONS */
static s16 get_named_dialog_width(const u8 *source, const u8 *text, int style) { (void)source; (void)text; (void)style; return 132; }
static void draw(struct DialogEntry *dialog) {
    const u8 *sourceText=dialog->str;
    int lowerBound=1, style=0;
    s8 boxLines/* TAIL */
int main(void) {
    const u8 padded[] = {1,0xFE,0xFE,0xFE,0xFE, 2,0xFE,3,0xFE,4,0xFE,0xFE, 5,0xFF};
    struct DialogEntry dialog = {4,padded,30,200};
    s16 next;
    assert(get_dialog_page_line_count(&dialog,0,&next)==1 && next==5);
    assert(get_dialog_page_line_count(&dialog,5,&next)==3 && next==12);
    assert(get_dialog_page_line_count(&dialog,12,&next)==1 && next==-1);
    // Fit the current page, preserving authored padding as pagination only.
    gDialogBoxState=DIALOG_STATE_VERTICAL; gLastDialogPageStrPos=5;
    draw(&dialog); assert(boxHeight==1 && nextHeight==1 && clipBottom==56);
    // Scrolling in either direction fits both displayed pages.
    gDialogBoxState=DIALOG_STATE_HORIZONTAL;
    assert(get_dialog_box_line_count(&dialog)==3);
    gDialogTextPos=5; assert(get_dialog_box_line_count(&dialog)==3);
    gDialogTextPos=12; assert(get_dialog_box_line_count(&dialog)==1);
    gDialogBoxState=DIALOG_STATE_VERTICAL;
    gLastDialogPageStrPos=-1; gLastDialogResponse=1;
    draw(&dialog); assert(boxHeight==1 && choiceHeight==1);
    // Empty lines inside text remain; final spaces and control payloads do not inflate height.
    const u8 gaps[] = {1,0xFE,0xFE,2,0xFE,0x9E,0xD0,0xFF};
    dialog.str=gaps; gDialogTextPos=0;
    assert(get_dialog_box_line_count(&dialog)==3);
    const u8 color[] = {1,0xFE,0xDF,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF};
    dialog.str=color; assert(get_dialog_box_line_count(&dialog)==1);
    const u8 variable[] = {0xFE,0xD1,0xFE,0xD2,0xFE,0xE0,0xFF};
    dialog.str=variable; assert(get_dialog_box_line_count(&dialog)==4);
    const u8 empty[] = {0x9E,0xFF};
    dialog.str=empty; assert(get_dialog_box_line_count(&dialog)==1);
    // Opening/closing and both box themes use the same fitted rendering path.
    for (gDialogBoxState=0; gDialogBoxState<=3; gDialogBoxState++) {
        draw(&dialog); assert(boxHeight==1);
    }
    return 0;
}
'''
        code = code.replace('/* FUNCTIONS */',functions).replace('/* TAIL */',tail)
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            (folder/'test.c').write_text(code)
            for wide in (False,True):
                cmd = ['cc','-std=c99','-Wall','-Wextra','-Werror','-fsanitize=undefined',
                       str(folder/'test.c'),'-o',str(folder/'test')]
                if wide:
                    cmd.append('-DWIDESCREEN')
                subprocess.run(cmd,check=True)
                subprocess.run([str(folder/'test')],check=True)


if __name__ == '__main__':
    unittest.main()
