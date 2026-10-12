#!/usr/bin/env python3
"""Check extraction and failure reporting when changing the test harness."""
from contextlib import redirect_stderr, redirect_stdout
import io
from pathlib import Path
import os
import subprocess
import tempfile
import unittest
from unittest.mock import patch

from regression_support import HarnessError, Source, animation_metadata
from run_regressions import HERE, main, mutations, run_suite


class ExtractionTests(unittest.TestCase):
    def source(self, text):
        return Source(HERE / "fixture.c", text)

    def test_multiline_function_ignores_prototypes_comments_and_strings(self):
        source = self.source('int target(void);\n/* int target(void) { } */\n'
                             'static\nint *\ntarget(void) {\n'
                             '    const char *text = "} \\\" {"; // }\n'
                             "    char brace = '}';\n    return 0;\n}\n"
                             'int other(void) { return 1; }\n')
        extracted = source.function("target")
        self.assertTrue(extracted.startswith('#line 3 '))
        self.assertIn('return 0;', extracted)
        self.assertNotIn('int other', extracted)

    def test_missing_duplicate_and_unclosed_functions_are_errors(self):
        for text in ('int target(void);', 'int target(void) {',
                     'int target(void) {}\nint target(void) {}'):
            with self.subTest(text=text), self.assertRaises(HarnessError):
                self.source(text).function("target")

    def test_macros_ignore_comments_and_preserve_continuation(self):
        source = self.source('/*\n#define VALUE 99\n*/\n#define VALUE (1 + \\\n 2)\n')
        extracted = source.macros("VALUE")
        self.assertTrue(extracted.startswith('#line 4 '))
        self.assertIn(' 2)', extracted)
        self.assertNotIn('99', extracted)

    def test_declaration_ignores_quoted_braces(self):
        self.assertIn("CLOSE = '}'", self.source("enum Flags { CLOSE = '}' };\n").declaration("enum", "Flags"))
        with self.assertRaises(HarnessError):
            self.source('struct Item { int n; } item;').declaration('struct', 'Item')

    def test_missing_or_ambiguous_region_is_an_error(self):
        for text in ('START\n', 'START\nSTART\nEND', 'END\nSTART'):
            with self.subTest(text=text), self.assertRaises(HarnessError):
                self.source(text).region('^START', '^END')

    def test_mutations_require_a_unique_match_inside_the_function(self):
        source = self.source('int target(void) { return 1; }\nint other(void) { return 1; }')
        changed = source.replace_in_function('target', 'return 1;', 'return 2;')
        self.assertIn('other(void) { return 1; }', changed)
        with self.assertRaises(HarnessError):
            source.replace_in_function('target', 'missing', 'return 2;')

    def test_animation_adapter_rejects_unsupported_playback(self):
        self.assertEqual(animation_metadata(self.source('const int anim_66[] = {1, 0, 0, 0, 20, 0};')),
                         (1, 0, 0, 20))
        for header in ('0,0,0,0,20', '3,0,0,0,20', '1,0,1,0,20', '1,0,0,2,20',
                       '1,0,0,0,2', 'FLAGS,0,0,0,20', '1,0'):
            with self.subTest(header=header), self.assertRaises(HarnessError):
                animation_metadata(self.source('const int anim_66[] = {' + header + '};'))


class ReportingTests(unittest.TestCase):
    def test_full_suite_uses_individual_case_deadlines_and_preserves_failures(self):
        results = [
            subprocess.CompletedProcess([], 0, "", ""),
            subprocess.CompletedProcess([], 0, "audio.first\naudio.second\n", ""),
            subprocess.CompletedProcess([], 0, "PASS audio.first\n", ""),
            subprocess.CompletedProcess([], 1, "FAIL audio.second\n", "second failed\n"),
        ]
        with patch('run_regressions.prepare'), patch('run_regressions.subprocess.run',
                                                    side_effect=results) as execute:
            result = run_suite(HERE.parent, 'audio', quiet=True)
        calls = execute.call_args_list
        self.assertEqual(len(calls), 4)
        self.assertEqual(calls[1].args[0][-1], '--list')
        for call, name in zip(calls[2:], ('audio.first', 'audio.second')):
            self.assertEqual(call.args[0][-2:], ['--case', name])
            self.assertEqual(call.kwargs['timeout'], 10)
        self.assertEqual(result.returncode, 1)
        self.assertIn('PASS audio.first', result.stdout)
        self.assertIn('FAIL audio.second', result.stdout)
        self.assertIn('second failed', result.stderr)

    def invoke(self, arguments):
        output, errors = io.StringIO(), io.StringIO()
        with redirect_stdout(output), redirect_stderr(errors):
            status = main(arguments)
        return status, output.getvalue(), errors.getvalue()

    def test_list_and_named_case(self):
        status, output, errors = self.invoke(['--suite', 'audio', '--list'])
        self.assertEqual(status, 0, errors)
        self.assertIn('audio.queued\n', output)
        status, output, errors = self.invoke(['--case', 'audio.queued'])
        self.assertEqual(status, 0, errors)
        self.assertEqual(output.count('PASS '), 1)

    def test_unknown_case_is_a_harness_error(self):
        status, _, errors = self.invoke(['--case', 'audio.no_such_case'])
        self.assertEqual(status, 2)
        self.assertIn('no case matched', errors)

    def test_invalid_compiler_is_a_harness_error(self):
        for compiler in ('', '"unclosed', str(HERE / 'no-such-compiler')):
            with self.subTest(compiler=compiler), patch.dict(os.environ, CC=compiler):
                status, _, errors = self.invoke(['--suite', 'audio'])
                self.assertEqual(status, 2)
                self.assertIn('HARNESS ERROR', errors)

    def test_compiler_diagnostics_point_to_original_source(self):
        path = 'src/audio/external.c'
        source = Source(HERE.parent / path)
        changed = source.replace_in_function('stop_sound', 'request->soundBits = NO_SOUND;',
                                             'REGRESSION_INTENTIONAL_COMPILE_ERROR;')
        line = changed[:changed.index('REGRESSION_INTENTIONAL_COMPILE_ERROR')].count('\n') + 1
        with self.assertRaises(HarnessError) as error:
            run_suite(HERE.parent, 'audio', overrides={path: changed}, quiet=True)
        self.assertIn(f'{source.path}:{line}:', str(error.exception))
        self.assertIn('not a gameplay verdict', str(error.exception))

    def test_punch_defect_reports_sequence_and_recent_inputs(self):
        suite, _, case, overrides = next(mutation for mutation in mutations(HERE.parent, ['punch'])
                                           if mutation[1] == 'forget_pair_mode')
        result = run_suite(HERE.parent, suite, case, overrides=overrides, quiet=True)
        self.assertEqual(result.returncode, 1)
        self.assertIn(f'FAIL {case}', result.stdout)
        for detail in ('Expected sequence:', 'Observed sequence:', 'Recent ticks:', 'B=', case):
            self.assertIn(detail, result.stderr)


class NumericAssertionTests(unittest.TestCase):
    def test_nonfinite_comparisons_fail_and_finite_tolerances_are_preserved(self):
        fixtures = (
            ("geometry_transitions_test.c", "NEAR", "0.01f", 1),
            ("bugfix_transitions_test.c", "NEAR", "0.002f", 1),
            ("object_platform_motion_test.c", "NEAR", "0.04f", 1),
            ("swimming_collision_test.c", "NEAR", "0.02f", 0),
            ("landing_movement_test.c", "EXPECT_NEAR", "0.001f", 1),
        )
        for filename, macro, tolerance, boundary_passes in fixtures:
            with self.subTest(fixture=filename), tempfile.TemporaryDirectory(
                    prefix="numeric-assertion-") as directory:
                folder = Path(directory)
                source = Source(HERE / filename).macros(macro)
                witness = """#include <math.h>
#include <stdio.h>
typedef float f32;
#define FALSE 0
static const char *sCurrentCase = "numeric contract";
""" + source + rf"""
static int compare(float actual, float expected) {{
    {macro}(actual, expected);
    return 1;
}}
int main(void) {{
    const float tolerance = {tolerance};
    if (!compare(0, 0) || !compare(tolerance / 2, 0)
        || compare(tolerance, 0) != {boundary_passes}
        || compare(nextafterf(tolerance, INFINITY), 0)) {{
        return 1;
    }}
    const float invalid[][2] = {{
        {{ NAN, 0 }}, {{ 0, NAN }}, {{ NAN, NAN }},
        {{ INFINITY, 0 }}, {{ -INFINITY, 0 }},
        {{ INFINITY, INFINITY }}, {{ -INFINITY, -INFINITY }}
    }};
    for (unsigned i = 0; i < sizeof(invalid) / sizeof(invalid[0]); i++) {{
        if (compare(invalid[i][0], invalid[i][1])) {{
            fprintf(stderr, "accepted nonfinite comparison %u\n", i);
            return 1;
        }}
    }}
    return 0;
}}
"""
                (folder / "witness.c").write_text(witness)
                subprocess.run(["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror",
                                str(folder / "witness.c"), "-o", str(folder / "witness"), "-lm"],
                               check=True, capture_output=True, text=True, timeout=30)
                result = subprocess.run([str(folder / "witness")], capture_output=True,
                                        text=True, timeout=10)
                self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == '__main__':
    unittest.main()
