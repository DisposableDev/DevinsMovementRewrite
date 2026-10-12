"""Source-derived checks for name-expanded dialogue and score-owner bounds."""
from pathlib import Path
import json
import re
import subprocess
import tempfile
import unittest

from regression_support import Source
from run_regressions import prepare

ROOT = Path(__file__).resolve().parents[1]


def encoded(text):
    values = []
    punctuation = {' ': 0x9E, '.': 0x3F, ',': 0x6F, '!': 0xF2,
                   '?': 0xF4, "'": 0x3E, '-': 0x9F, '\n': 0xFE}
    for char in text:
        if 'A' <= char <= 'Z':
            values.append(ord(char) - ord('A') + 0x0A)
        elif 'a' <= char <= 'z':
            values.append(ord(char) - ord('a') + 0x24)
        elif char.isdigit() and char.isascii():
            values.append(int(char))
        else:
            values.append(punctuation[char])
    return '{' + ','.join(map(str, values + [0xFF])) + '}'


class AuditUiFixTests(unittest.TestCase):
    def test_name_expansion_fits_without_changing_authored_pages(self):
        menu = Source(ROOT / 'src/game/ingame_menu.c')
        # Authored lines identified by the audit, including native and new dialogue.
        lines = ['coins! Mario gains more', 'Mario gains more courage',
                 'controller to move Mario', "Sorry, Mario. I can't -",
                 'Ahh! Mario! I made it']
        checks = []
        for line in lines:
            checks.append('''{
                const u8 original[] = %s;
                u8 named[256];
                const u8 *text=mario_character_text_replace_name(MARIO_TEXT_HE_THICC,
                    MARIO_TEXT_DIALOG,original,named);
                int width=get_named_dialog_width(original,text,MARIO_TEXT_HE_THICC);
                assert(width >= 132 && width <= SCREEN_WIDTH-16);
                if(text!=original) {
                    struct DialogEntry dialog={0,4,30,200,(u8 *)text};
                    clear_output(); gDialogTextPos=0; gDialogBoxState=DIALOG_STATE_VERTICAL;
                    handle_dialog_text_and_pages(0,&dialog,0,MARIO_TEXT_HE_THICC);
                    assert(sAdvance <= width);
                    assert(maxGlyphRight <= width);
                    assert(gLastDialogPageStrPos==-1);
                } else assert(width==132);
                assert(get_named_dialog_width(original,original,MARIO_TEXT_STANDARD)==132);
            }''' % encoded(line))
        extra = '''
#define DIAG_VAL3 132
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#include <assert.h>
''' + menu.function('get_named_dialog_width') + '''
int main(void) {
    begin(MARIO_PALETTE_HE_THICC);
''' + '\n'.join(checks) + '''
    const u8 original[]={0x16,0x24,0x35,0x2C,0x32,0xFE,0x16,0x24,0x35,0x2C,0x32,0xFF};
    u8 named[128];
    const u8 *text=mario_character_text_replace_name(MARIO_TEXT_HE_THICC,MARIO_TEXT_DIALOG,original,named);
    assert(text[8]==DIALOG_CHAR_NEWLINE); // replacement retains the authored line boundary.
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            prepare(ROOT, 'character', directory, {})
            fixture = (ROOT / 'tests/character_text_test.c').read_text()
            adapters = (
                (r"\bint main\(([^)]*)\)", r"int original_main(\1)"),
                (r"(static void render_generic_char\(u8 glyph\)\s*\{)",
                 r"static f32 maxGlyphRight;\n\1\n"
                 r"if(sAdvance+16>maxGlyphRight) maxGlyphRight=sAdvance+16;"),
                (r"(static void clear_output\(void\)\s*\{)", r"\1\nmaxGlyphRight=0;"),
            )
            for pattern, replacement in adapters:
                fixture, injections = re.subn(pattern, replacement, fixture, count=1)
                self.assertEqual(injections, 1, "dialogue bounds adapter was not installed")
            (directory / 'probe.c').write_text(fixture + extra)
            compiled = subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror',
                                       '-Wno-unused-parameter', '-I', str(directory),
                                       '-I', str(ROOT / 'tests'), str(directory / 'probe.c'),
                                       '-o', str(directory / 'probe'), '-lm'], capture_output=True, text=True)
            self.assertEqual(compiled.returncode, 0, compiled.stderr)
            run = subprocess.run([str(directory / 'probe')], capture_output=True, text=True)
            self.assertEqual(run.returncode, 0, run.stderr)

    def test_tas_owner_glyph_rectangles_fit_native_width(self):
        source = Source(ROOT / 'src/menu/file_select.c')
        menu = Source(ROOT / 'src/game/ingame_menu.c')
        # Use actual menu font advances and the complete final 8px glyph rectangle.
        widths_region = menu.region(r'^u8 gDialogCharWidths', r'^s8 gDialogBoxState')
        import re
        # The native table has conditionals; preprocess it instead of guessing indices.
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            code = '#include <stdio.h>\ntypedef unsigned char u8;\n' + widths_region
            code += 'int main(void) { for(int i=0;i<256;i++) printf("%u\\n",gDialogCharWidths[i]); }\n'
            (directory / 'width.c').write_text(code)
            result = subprocess.run(['cc', '-I', str(ROOT / 'include'), str(directory / 'width.c'), '-o', str(directory / 'width')], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            widths = list(map(int, subprocess.check_output([str(directory / 'width')], text=True).splitlines()))
        offsets = {name: int(re.search(r'#define\s+' + name + r'\s+(\d+)', source.text)[1])
                   for name in ('HISCORE_COIN_ICON_X', 'HISCORE_COIN_TEXT_X', 'HISCORE_COIN_NAMES_X')}
        self.assertEqual(offsets['HISCORE_COIN_TEXT_X'] - offsets['HISCORE_COIN_ICON_X'], 16)
        self.assertEqual(offsets['HISCORE_COIN_NAMES_X'] - offsets['HISCORE_COIN_TEXT_X'], 26)
        for label in ('TAS THICC', 'HE THICC', 'MARIO A', 'MARIO B', 'MARIO C', 'MARIO D'):
            glyphs = json.loads(encoded(label).replace('{', '[').replace('}', ']'))[:-1]
            x = 213 + offsets['HISCORE_COIN_NAMES_X']
            for glyph in glyphs:
                if glyph == 0x9E:
                    x += 4
                else:
                    self.assertLessEqual(x + 8, 320, label)
                    x += widths[glyph]


if __name__ == '__main__':
    unittest.main()
