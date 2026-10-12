#!/usr/bin/env python3
"""Exercise focused selection and compile reuse with a small real C fixture."""
from contextlib import redirect_stderr, redirect_stdout
import io
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest
from unittest.mock import patch

import regression_mutations
from regression_mutations import MutationSource, mutations
from regression_support import HarnessError, Source
import run_regressions as runner


class RunnerTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory(prefix="sm64-runner-test-")
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.source = self.root / "source.c"
        self.source.write_text("int first(void) { return 1; }\nint second(void) { return 2; }\n")
        shutil.copyfile(runner.HERE / "test_support.h", self.root / "test_support.h")
        (self.root / "fixture.c").write_text(
            '#include "test_support.h"\n#include "runner_sources.inc"\n'
            'int main(int argc, char **argv) {\n'
            '    test_init(argc, argv);\n'
            '    if (test_selected("runner.first")) test_report("runner.first", first() == 1);\n'
            '    if (test_selected("runner.second")) test_report("runner.second", second() == 2);\n'
            '    return test_finish();\n}\n')
        for target, value in (("HERE", self.root), ("SUITES", {"runner": "fixture.c"})):
            patched = patch.object(runner, target, value)
            patched.start()
            self.addCleanup(patched.stop)
        runner.case_catalog.refresh(self.root, runner.SUITES)
        patched = patch.object(runner, "prepare", self.prepare)
        patched.start()
        self.addCleanup(patched.stop)
        patched = patch.dict(regression_mutations.MUTATORS, {"runner": self.mutation_specs}, clear=True)
        patched.start()
        self.addCleanup(patched.stop)

    @staticmethod
    def prepare(root, suite, temp, overrides):
        source = Source(root / "source.c", overrides.get("source.c"))
        (temp / "runner_sources.inc").write_text(source.function("first") + source.function("second"))

    @staticmethod
    def mutation_specs(root):
        yield "runner", "obsolete", "runner.first", {
            "source.c": MutationSource(root / "source.c").replace_in_function("retired", "old", "new")}
        yield "runner", "wrong_second", "runner.second", {
            "source.c": MutationSource(root / "source.c").replace_in_function("second", "return 2;", "return 3;")}
        yield "runner", "missing_file", "runner.first", {
            "missing.c": MutationSource(root / "missing.c").replace_unique("old", "new")}

    def invoke(self, *arguments):
        output, errors = io.StringIO(), io.StringIO()
        with redirect_stdout(output), redirect_stderr(errors):
            status = runner.main(["--source-root", str(self.root), *arguments])
        return status, output.getvalue(), errors.getvalue()

    def test_multiple_cases_share_one_real_compilation(self):
        stats = runner.RunStats()
        real_run = subprocess.run
        with patch.object(runner.subprocess, "run", wraps=real_run) as processes:
            result = runner.run_suite(self.root, "runner", ["runner.first", "runner.second", "runner.first"],
                                      quiet=True, stats=stats)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stdout, "PASS runner.first\nPASS runner.second\n")
        compiles = [call for call in processes.call_args_list if "-o" in call.args[0]]
        self.assertEqual(len(compiles), 1)
        self.assertEqual(stats.counts, {"prepare": 1, "compile": 1, "execute": 2})

    def test_empty_case_selectors_rejected_before_preparation(self):
        for selection in ("", ["runner.first", ""]):
            with self.subTest(selection=selection):
                stats = runner.RunStats()
                with patch.object(runner, "prepare", side_effect=AssertionError("prepared")) as prepare, \
                        patch.object(runner.subprocess, "run", side_effect=AssertionError("compiled")) as processes:
                    with self.assertRaisesRegex(HarnessError, "empty case selector"):
                        runner.run_suite(self.root, "runner", selection, quiet=True, stats=stats)
                prepare.assert_not_called()
                processes.assert_not_called()
                self.assertEqual(stats.counts, {"prepare": 0, "compile": 0, "execute": 0})

    def test_listing_does_not_prepare_compile_or_execute(self):
        with patch.object(runner, "prepare", side_effect=AssertionError("prepared")), \
                patch.object(runner.subprocess, "run", side_effect=AssertionError("compiled")):
            status, output, errors = self.invoke("--list", "--timings")
        self.assertEqual(status, 0, errors)
        self.assertIn("runner.first\nrunner.second\n", output)
        self.assertIn("TIMING compile: 0.00s; 0 calls", output)

    def test_stale_catalog_can_refresh_with_a_broken_fixture(self):
        fixture = self.root / "fixture.c"
        fixture.write_text(fixture.read_text() + "\nthis is invalid C\n")
        status, _, errors = self.invoke("--list")
        self.assertEqual(status, 2)
        self.assertIn("catalog missing or stale", errors)
        with patch.object(runner.subprocess, "run", side_effect=AssertionError("compiled")):
            self.assertEqual(self.invoke("--refresh-case-catalog")[0], 0)
            status, output, errors = self.invoke("--list", "--case", "runner.second")
        self.assertEqual(status, 0, errors)
        self.assertEqual(output, "runner.second\n")

    def test_listing_matches_all_query_words_without_compilation(self):
        with patch.object(runner, "prepare", side_effect=AssertionError("prepared")), \
                patch.object(runner.subprocess, "run", side_effect=AssertionError("compiled")):
            status, output, errors = self.invoke("--list", "--match", "RUNNER SECOND")
            self.assertEqual(status, 0, errors)
            self.assertEqual(output, "runner.second\n")
            self.assertEqual(self.invoke("--list", "--match", "runner absent")[0], 2)
            with self.assertRaises(SystemExit) as error:
                self.invoke("--match", "second")
            self.assertEqual(error.exception.code, 2)

    def test_catalog_rejects_modified_names_and_unknown_filters(self):
        catalog = self.root / runner.case_catalog.CATALOG
        catalog.write_text(catalog.read_text().replace("runner.first", "runner.forged"))
        self.assertEqual(self.invoke("--list")[0], 2)
        runner.case_catalog.refresh(self.root, runner.SUITES)
        self.assertEqual(self.invoke("--list", "--case", "runner.unknown")[0], 2)

    def test_catalog_rejects_duplicate_registration_names(self):
        fixtures = {
            "literal": ('int main(void) { RUN("runner.first", first); '
                        'RUN("runner.first", second); return 0; }'),
            "table": ('int main(void) { struct Entry cases[] = {'
                      '{"runner.first", first}, {"runner.first", second}}; '
                      'if (test_selected(cases[i].name)) {} return 0; }'),
            "prefix": ('int main(void) {\n'
                       '#define RUN(name, fn) if (test_selected("runner." name)) {}\n'
                       'RUN("first", first); RUN("first", second); return 0; }'),
        }
        for registration, text in fixtures.items():
            with self.subTest(registration=registration):
                path = self.root / "duplicate.c"
                path.write_text(text)
                with self.assertRaisesRegex(HarnessError, "duplicate case registrations"):
                    runner.case_catalog.discover(path, "runner")

    def test_compiled_catalog_verification(self):
        status, output, errors = self.invoke("--verify-case-catalog")
        self.assertEqual(status, 0, errors)
        self.assertIn("catalog matches compiled", output)

    def test_batch_preserves_behavior_failures(self):
        changed = self.source.read_text().replace("return 2;", "return 3;")
        result = runner.run_suite(self.root, "runner", ["runner.first", "runner.second"],
                                  overrides={"source.c": changed}, quiet=True)
        self.assertEqual(result.returncode, 1)
        self.assertEqual(result.stdout, "PASS runner.first\nFAIL runner.second\n")

    def test_unknown_case_in_batch_is_a_harness_error(self):
        with self.assertRaisesRegex(HarnessError, "no case matched"):
            runner.run_suite(self.root, "runner", ["runner.first", "runner.unknown"], quiet=True)

    def test_cli_batches_cases_and_reports_timings(self):
        status, output, errors = self.invoke("--case", "runner.first", "--case", "runner.second", "--timings")
        self.assertEqual(status, 0, errors)
        self.assertIn("2 cases passed", output)
        self.assertRegex(output, r"TIMING compile: [0-9.]+s; 1 calls")

    def test_case_mutations_skip_stale_and_missing_unselected_source(self):
        selected = list(mutations(self.root, ["runner"], cases=["runner.second"]))
        self.assertEqual([item[1] for item in selected], ["wrong_second"])
        self.assertIn("return 3;", selected[0][3]["source.c"])
        status, output, errors = self.invoke("--case", "runner.second", "--self-check")
        self.assertEqual(status, 0, errors)
        self.assertIn("DETECTED wrong_second: runner.second", output)
        self.assertEqual(output.count("PASS "), 1)

    def test_named_mutation_runs_only_its_baseline_case(self):
        status, output, errors = self.invoke("--self-check", "--mutation", "wrong_second", "--timings")
        self.assertEqual(status, 0, errors)
        self.assertIn("PASS runner.second", output)
        self.assertNotIn("PASS runner.first", output)
        self.assertRegex(output, r"TIMING compile: [0-9.]+s; 2 calls")

    def test_selected_stale_mutation_is_still_a_harness_error(self):
        status, _, errors = self.invoke("--self-check", "--mutation", "obsolete")
        self.assertEqual(status, 2)
        self.assertIn("retired", errors)

    def test_failed_baseline_prevents_mutation_compilation(self):
        self.source.write_text(self.source.read_text().replace("return 2;", "return 4;"))
        status, output, errors = self.invoke("--self-check", "--mutation", "wrong_second", "--timings")
        self.assertEqual(status, 1, errors)
        self.assertIn("FAIL runner.second", output)
        self.assertNotIn("DETECTED", output)
        self.assertRegex(output, r"TIMING compile: [0-9.]+s; 1 calls")

    def test_unknown_or_incompatible_mutation_does_not_compile(self):
        for arguments in (("--mutation", "typo"),
                          ("--case", "runner.first", "--mutation", "wrong_second")):
            with self.subTest(arguments=arguments), patch.object(runner, "run_suite") as run_suite:
                status, _, errors = self.invoke("--self-check", *arguments)
            self.assertEqual(status, 2)
            self.assertIn("no selected mutation matched", errors)
            run_suite.assert_not_called()

    def test_listing_mutations_neither_reads_source_nor_compiles(self):
        self.source.unlink()
        with patch.object(runner, "run_suite") as run_suite:
            status, output, errors = self.invoke("--list-mutations")
        self.assertEqual(status, 0, errors)
        self.assertIn("obsolete: runner.first", output)
        self.assertIn("missing_file: runner.first", output)
        run_suite.assert_not_called()

    def test_deferred_whole_file_region_and_chained_edits(self):
        source = MutationSource(self.source)
        edited = source.replace_unique("return 1;", "return 4;")
        edited = MutationSource(self.source, edited).replace_in_function("second", "return 2;", "return 5;")
        self.assertEqual(edited.resolve(), "int first(void) { return 4; }\nint second(void) { return 5; }\n")
        region = source.replace_in_region("int first", "int second", "return 1;", "return 6;")
        self.assertIn("first(void) { return 6; }", region.resolve())
        self.assertIn("second(void) { return 2; }", region.resolve())
        with self.assertRaises(HarnessError):
            source.replace_unique("return", "yield").resolve()
        with self.assertRaises(HarnessError):
            source.replace_in_region("int second", "int first", "return 2;", "return 6;").resolve()
        self.assertEqual(source.resolve(), self.source.read_text())


if __name__ == "__main__":
    unittest.main()
