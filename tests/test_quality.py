#!/usr/bin/env python3
"""Exercise change selection, tool failures, and source/index preservation in isolated repos."""

import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from types import SimpleNamespace
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import check_quality as quality
import quality_tidy
import quality_snapshot


def available_tool(name):
    try:
        return quality.clang_tool(ROOT, name, "18.1.3")
    except quality.SetupError:
        return None


FORMATTER = available_tool("clang-format")
TIDY = available_tool("clang-tidy")
CLANG = available_tool("clang")


class QualityTests(unittest.TestCase):
    def setUp(self):
        git_environment = dict(os.environ)
        for variable in ("GIT_INDEX_FILE", "GIT_DIR", "GIT_WORK_TREE", "GIT_COMMON_DIR",
                         "GIT_OBJECT_DIRECTORY", "GIT_ALTERNATE_OBJECT_DIRECTORIES", "GIT_PREFIX"):
            git_environment.pop(variable, None)
        environment_patch = patch.dict(os.environ, git_environment, clear=True)
        environment_patch.start()
        self.addCleanup(environment_patch.stop)
        self.temporary = tempfile.TemporaryDirectory(prefix="sm64-quality-test-")
        self.addCleanup(self.temporary.cleanup)
        self.directory = Path(self.temporary.name)
        self.root = self.directory / "repo"
        self.root.mkdir()
        self.git("init", "--quiet")
        self.config = json.loads((ROOT / ".quality.json").read_text())
        self.config["validation"] = []
        self.write(".quality.json", json.dumps(self.config) + "\n")
        self.write(".clang-format", (ROOT / ".clang-format").read_text())
        self.write(".clang-tidy", (ROOT / ".clang-tidy").read_text())
        self.write("Makefile", "quality-flags: ; $(info QUALITY_CFLAGS=-std=gnu17) "
                   "$(info QUALITY_C_FILES=$(wildcard src/*.c) $(wildcard src/game/*.c)) "
                   "$(info QUALITY_BUILD_DIR=build) @true\n")
        self.write("src/old.c", "int old(void) {\n    return 0;\n}\n")
        self.git("add", ".")
        self.git("-c", "user.name=Quality Tests", "-c", "user.email=quality@example.invalid",
                 "commit", "--quiet", "-m", "fixture baseline")
        self.fake = self.directory / "clang-format"
        self.formatter()
        self.tidy = self.directory / "clang-tidy"
        self.tidy_log = self.directory / "tidy-arguments.json"
        self.tidy.write_text(f"#!{sys.executable}\nimport json, sys\nfrom pathlib import Path\n"
                             "if '--version' in sys.argv:\n"
                             "    print('clang-tidy version 18.1.3')\n"
                             "else:\n"
                             f"    Path({str(self.tidy_log)!r}).write_text(json.dumps(sys.argv[1:]))\n")
        self.tidy.chmod(0o755)
        self.environment = dict(os.environ, CLANG_FORMAT=str(self.fake), CLANG=str(self.fake),
                                CLANG_TIDY=str(self.tidy), PYTHONDONTWRITEBYTECODE="1")

    def git(self, *arguments):
        return subprocess.run(["git", *arguments], cwd=self.root, capture_output=True,
                              check=True).stdout

    def write(self, name, content):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content)
        return path

    def formatter(self, body=None, version="18.1.3"):
        body = body or "print('<replacements incomplete_format=\"false\"></replacements>')"
        self.fake.write_text(f"#!{sys.executable}\nimport sys\n"
                             "if '--version' in sys.argv:\n"
                             f"    print('clang-format version {version}')\n"
                             "else:\n" + "\n".join("    " + line for line in body.splitlines()) + "\n")
        self.fake.chmod(0o755)

    def check(self, *arguments, environment=None):
        return subprocess.run([sys.executable, str(ROOT / "tools/check_quality.py"), *arguments],
                              cwd=self.root, capture_output=True, text=True,
                              env=environment or self.environment, check=False)

    def snapshot(self):
        return {
            "index": (self.root / ".git/index").read_bytes(),
            "entries": self.git("ls-files", "--stage", "-z"),
            "files": {str(p.relative_to(self.root)): p.read_bytes()
                      for p in self.root.rglob("*") if p.is_file() and ".git" not in p.parts}
        }

    def test_fixture_owns_index_and_restores_git_environment(self):
        index = self.root / ".git/index"
        before = index.read_bytes()
        with patch.dict(os.environ, {"GIT_INDEX_FILE": str(index)}):
            nested = QualityTests("test_no_changes_require_no_formatter")
            result = unittest.TestResult()
            nested.run(result)
            self.assertTrue(result.wasSuccessful(), result.errors)
            self.assertEqual(before, index.read_bytes())
            self.assertEqual(os.environ.get("GIT_INDEX_FILE"), str(index))

    def test_no_changes_require_no_formatter(self):
        self.environment["CLANG_FORMAT"] = str(self.directory / "missing")
        result = self.check()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("NOT APPLICABLE", result.stdout)

    def test_whitespace_checks_only_added_lines(self):
        self.write("notes.md", "settled line \n")
        self.git("add", "notes.md")
        self.git("-c", "user.name=Quality Tests", "-c", "user.email=quality@example.invalid",
                 "commit", "--quiet", "-m", "notes")
        self.write("notes.md", "settled line \nbad line \n")
        result = self.check()
        self.assertEqual(result.returncode, 1)
        self.assertIn("trailing whitespace", result.stdout)
        self.assertIn(":2:", result.stdout)
        self.assertNotIn(":1:", result.stdout)

    def test_partial_staging_reads_the_index_including_configuration(self):
        self.write("notes.md", "staged content\n")
        self.git("add", "notes.md")
        self.write("notes.md", "unstaged content \n")
        self.write(".quality.json", "invalid unstaged configuration\n")
        before = self.snapshot()
        result = self.check("--staged")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(before, self.snapshot())
        self.assertEqual(self.check().returncode, 2)

    def test_text_snapshot_omits_unrelated_media_and_source(self):
        self.write("output/unrelated.bin", "x" * 100000)
        self.git("add", "output/unrelated.bin")
        self.git("-c", "user.name=Quality Tests", "-c", "user.email=quality@example.invalid",
                 "commit", "--quiet", "-m", "large unrelated fixture")
        self.write("notes.md", "selected content\n")
        self.git("add", "notes.md")
        index = self.git("ls-files", "--stage", "-z")
        destination = self.directory / "snapshot"
        destination.mkdir()
        copied, total = quality_snapshot.materialize(self.root, destination, index, {"notes.md"})
        self.assertLess(copied, total)
        self.assertEqual((destination / "notes.md").read_text(), "selected content\n")
        self.assertFalse((destination / "output").exists())
        self.assertFalse((destination / "src").exists())

    def test_nested_dependency_uses_staged_bytes_and_texture(self):
        self.write("src/game/owner.c", '#include "../../actors/sample/nested.inc.c"\n')
        self.write("actors/sample/nested.inc.c", '#include "leaf.inc.c"\n')
        self.write("actors/sample/leaf.inc.c", '#include "actors/sample/icon.rgba16.inc.c"\n')
        self.write("actors/sample/icon.png", "staged image")
        self.git("add", ".")
        self.write("actors/sample/leaf.inc.c", "unstaged invalid content\n")
        self.write("actors/sample/icon.png", "unstaged image")
        before = self.snapshot()
        index = self.git("ls-files", "--stage", "-z")
        destination = self.directory / "snapshot"
        destination.mkdir()
        quality_snapshot.materialize(self.root, destination, index, {"src/game/owner.c"}, parsed=True)
        self.assertIn("icon.rgba16", (destination / "actors/sample/leaf.inc.c").read_text())
        self.assertEqual((destination / "actors/sample/icon.png").read_text(), "staged image")
        self.assertEqual(before, self.snapshot())

    def test_unknown_macro_include_keeps_complete_index(self):
        self.write("src/game/owner.c", "#include CUSTOM_INPUT\n")
        self.write("custom/input.txt", "macro-selected dependency\n")
        self.git("add", ".")
        index = self.git("ls-files", "--stage", "-z")
        entries = quality_snapshot.index_entries(index)
        self.assertEqual(quality_snapshot.dependencies(self.root, entries, {"src/game/owner.c"}, True),
                         set(entries))

    def test_staged_defect_is_not_hidden_by_clean_working_content(self):
        self.write("notes.md", "staged content \n")
        self.git("add", "notes.md")
        self.write("notes.md", "working content\n")
        before = self.snapshot()
        result = self.check("--staged")
        self.assertEqual(result.returncode, 1, result.stderr)
        self.assertEqual(before, self.snapshot())

    def test_renames_do_not_format_inherited_content(self):
        self.git("mv", "src/old.c", "src/moved.c")
        self.environment["CLANG_FORMAT"] = str(self.directory / "missing")
        changes = quality.changed_files(self.root, quality.revision(self.root, "HEAD"), False)
        quality.prepare_changes(self.root, self.root, changes, quality.revision(self.root, "HEAD"))
        self.assertEqual(changes["src/moved.c"].lines, [])
        self.assertEqual(self.check().returncode, 0)

    def test_deleted_files_and_binary_assets_do_not_require_formatter(self):
        (self.root / "src/old.c").unlink()
        (self.root / "art.bin").write_bytes(b"\0\xff\n")
        self.environment["CLANG_FORMAT"] = str(self.directory / "missing")
        before = self.snapshot()
        self.assertEqual(self.check().returncode, 0)
        self.assertEqual(before, self.snapshot())

    def test_unusual_file_names_are_passed_without_shell_interpretation(self):
        name = "src/- spaced\tname\n$(touch forbidden).c"
        self.write(name, "int value;\n")
        before = self.snapshot()
        result = self.check()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(before, self.snapshot())
        self.assertFalse((self.root / "forbidden").exists())

    def test_generated_assets_are_excluded_but_behavior_includes_are_checked(self):
        self.write("levels/physics_lab/model.inc.c", "asset \n")
        self.write("src/game/behaviors/example.inc.c", "int behavior; \n")
        result = self.check()
        self.assertEqual(result.returncode, 1, result.stderr)
        self.assertIn("example.inc.c", result.stdout)
        self.assertNotIn("model.inc.c", result.stdout)

    def test_missing_tool_is_a_setup_error_and_changes_nothing(self):
        self.write("src/new.c", "int value;\n")
        self.environment["CLANG_FORMAT"] = str(self.directory / "missing")
        before = self.snapshot()
        result = self.check()
        self.assertEqual(result.returncode, 2)
        self.assertIn("UNAVAILABLE", result.stderr)
        self.assertEqual(before, self.snapshot())

    def test_wrong_tool_version_is_not_a_pass(self):
        self.write("src/new.c", "int value;\n")
        self.formatter(version="17.0.6")
        result = self.check()
        self.assertEqual(result.returncode, 2)
        self.assertIn("expected clang-format 18.1.3", result.stderr)

    def test_incomplete_format_is_a_setup_error(self):
        self.write("src/new.c", "int value;\n")
        self.formatter("print('<replacements incomplete_format=\"true\"></replacements>')")
        self.assertEqual(self.check().returncode, 2)

    def test_formatter_spill_blocks_fix_without_touching_anything(self):
        self.write("src/old.c", "int old(void) {\n return 1;\n}\n")
        self.formatter("print('<replacements incomplete_format=\"false\">"
                       "<replacement offset=\"0\" length=\"3\">long</replacement></replacements>')")
        before = self.snapshot()
        result = self.check("--fix")
        self.assertEqual(result.returncode, 2)
        self.assertIn("surrounding lines", result.stderr)
        self.assertEqual(before, self.snapshot())

    @unittest.skipUnless(FORMATTER, "pinned clang-format unavailable")
    def test_formatter_preserves_unselected_preceding_newline(self):
        self.environment["CLANG_FORMAT"] = FORMATTER
        self.write("src/old.c", "int number =\n2;\n")
        before = self.snapshot()
        result = self.check("--task-scope", self.scope(ranges=[[2, 2]]), "--fix", "--no-tidy")
        self.assertEqual(result.returncode, 2, result.stderr)
        self.assertIn("surrounding lines", result.stderr)
        self.assertEqual(before, self.snapshot())
        result = self.check("--task-scope", self.scope(ranges=[[1, 2]]), "--fix", "--no-tidy")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual((self.root / "src/old.c").read_text(), "int number = 2;\n")
        self.assertEqual(before["index"], self.snapshot()["index"])

    @unittest.skipUnless(FORMATTER, "pinned clang-format unavailable")
    def test_formatter_retains_contextual_newline_for_indentation(self):
        self.environment["CLANG_FORMAT"] = FORMATTER
        self.write("src/old.c", "int old(void) {\n return 1;\n}\n")
        before = self.snapshot()
        result = self.check("--task-scope", self.scope(ranges=[[2, 2]]), "--fix", "--no-tidy")
        self.assertEqual(result.returncode, 0, result.stderr)
        after = self.snapshot()
        self.assertEqual(before["index"], after["index"])
        self.assertEqual(before["entries"], after["entries"])
        self.assertEqual((self.root / "src/old.c").read_text(),
                         "int old(void) {\n    return 1;\n}\n")

    def test_sparse_formatter_scope_preserves_unselected_blank_line(self):
        self.write("src/old.c", "int old(void) {\n    int value = 1;\n\n return value;\n}\n")
        self.formatter("import xml.etree.ElementTree as ET\n"
                       "content=sys.stdin.buffer.read()\n"
                       "document=ET.Element('replacements', incomplete_format='false')\n"
                       "ET.SubElement(document,'replacement',"
                       "offset=str(content.index(b'\\n\\n')+1),length='1')\n"
                       "print(ET.tostring(document).decode())")
        before = self.snapshot()
        result = self.check("--task-scope", self.scope(ranges=[[2, 2], [4, 4]]), "--fix")
        self.assertEqual(result.returncode, 2, result.stderr)
        self.assertIn("surrounding lines", result.stderr)
        self.assertEqual(before, self.snapshot())

    def test_bounded_fix_preserves_index_and_unrelated_content(self):
        self.write("src/old.c", "int old(void) {\n return 1;\n}\n")
        self.formatter("import xml.etree.ElementTree as ET\n"
                       "content=sys.stdin.buffer.read()\n"
                       "document=ET.Element('replacements', incomplete_format='false')\n"
                       "item=ET.SubElement(document,'replacement',"
                       "offset=str(content.index(b' return')),length='1')\n"
                       "item.text='    '\nprint(ET.tostring(document).decode())")
        self.write("notes.md", "unrelated\n")
        self.git("add", "notes.md")
        before = self.snapshot()
        result = self.check("--fix")
        self.assertEqual(result.returncode, 0, result.stderr)
        after = self.snapshot()
        self.assertEqual(before["index"], after["index"])
        self.assertEqual(before["entries"], after["entries"])
        self.assertEqual((self.root / "src/old.c").read_text(), "int old(void) {\n    return 1;\n}\n")
        self.assertEqual(before["files"]["notes.md"], after["files"]["notes.md"])
        self.assertIn("--- a/src/old.c", result.stdout)

    def test_fix_refuses_partially_staged_source(self):
        self.write("src/old.c", "int old(void) {\n    return 1;\n}\n")
        self.git("add", "src/old.c")
        self.write("src/old.c", "int old(void) {\n return 2;\n}\n")
        before = self.snapshot()
        result = self.check("--fix")
        self.assertEqual(result.returncode, 2)
        self.assertIn("staged content", result.stderr)
        self.assertEqual(before, self.snapshot())

    def test_content_changed_during_formatting_is_preserved(self):
        self.write("src/old.c", "int old(void) {\n return 1;\n}\n")
        self.formatter("from pathlib import Path\n"
                       "Path('src/old.c').write_text('int newer;\\n')\n"
                       "print('<replacements incomplete_format=\"false\"></replacements>')")
        result = self.check("--fix")
        self.assertEqual(result.returncode, 2)
        self.assertIn("content changed", result.stderr)
        self.assertEqual((self.root / "src/old.c").read_text(), "int newer;\n")

    def scope(self, *, base=None, digest=None, ranges=None):
        content = (self.root / "src/old.c").read_bytes()
        path = self.directory / "scope.json"
        path.write_text(json.dumps({"base": base or quality.revision(self.root, "HEAD"), "files": {
            "src/old.c": {"sha256": digest or hashlib.sha256(content).hexdigest(),
                          "lines": ranges or [[1, 3]], "reason": "Reviewed fixture ownership"}
        }}))
        return str(path)

    def test_stale_scope_content_and_baseline_fail_closed(self):
        for arguments in ({"digest": "0" * 64}, {"base": "0" * 40}):
            with self.subTest(arguments=arguments):
                before = self.snapshot()
                result = self.check("--scope", self.scope(**arguments), "--fix")
                self.assertEqual(result.returncode, 2)
                self.assertEqual(before, self.snapshot())

    def test_invalid_scope_range_is_not_accepted(self):
        self.assertEqual(self.check("--scope", self.scope(ranges=[[1, 300]])).returncode, 2)

    def test_scope_rejects_content_changed_after_preparation(self):
        base = quality.revision(self.root, "HEAD")
        for task_only in (False, True):
            with self.subTest(task_only=task_only):
                self.write("src/old.c", "int old(void) {\n    return 1;\n}\n")
                changes = quality.changed_files(self.root, base, False, ["src/old.c"])
                quality.prepare_changes(self.root, self.root, changes, base)
                self.write("src/old.c", "int old(void) {\n    return 2; \n}\n")
                scope = self.scope(ranges=[[2, 2]])
                before = self.snapshot()
                with self.assertRaisesRegex(quality.SetupError, "content changed"):
                    quality.apply_scope(self.root, changes, base, scope, task_only=task_only)
                self.assertEqual(before, self.snapshot())

    def test_scope_matching_cached_content_preserves_selected_ranges(self):
        base = quality.revision(self.root, "HEAD")
        for task_only in (False, True):
            with self.subTest(task_only=task_only):
                self.write("src/old.c", "int old(void) {\n    return 1;\n}\n")
                changes = quality.changed_files(self.root, base, False, ["src/old.c"])
                quality.prepare_changes(self.root, self.root, changes, base)
                content = changes["src/old.c"].content
                scope = self.scope(ranges=[[1, 1]])
                before = self.snapshot()
                quality.apply_scope(self.root, changes, base, scope, task_only=task_only)
                self.assertEqual(changes["src/old.c"].content, content)
                self.assertEqual(changes["src/old.c"].lines,
                                 [(1, 1)] if task_only else [(1, 1), (2, 2)])
                self.assertEqual(before, self.snapshot())

    def test_reviewed_scope_can_select_unchanged_lines(self):
        result = self.check("--scope", self.scope(ranges=[[2, 2]]), "--verbose")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("1 C/C++", result.stdout)

    def test_task_paths_ignore_unrelated_dirty_and_invalid_source(self):
        self.write("tools/unrelated.py", "def invalid syntax\n")
        self.write("notes.md", "unrelated whitespace \n")
        self.write("src/old.c", "int old(void) {\n    return 1;\n}\n")
        before = self.snapshot()
        result = self.check("--path", "src/old.c", "--verbose")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("1 text files checked", result.stdout)
        self.assertEqual(before, self.snapshot())
        self.assertEqual(self.check("--path", "notes.md").returncode, 1)
        self.assertEqual(self.check("--path", "tools/unrelated.py").returncode, 2)

    def test_task_scope_replaces_dirty_ranges_in_the_same_file(self):
        self.write("src/old.c", "int old(void) { \n    return 1;\n}\n")
        self.write("tools/unrelated.py", "def invalid syntax\n")
        scope = self.scope(ranges=[[2, 2]])
        result = self.check("--task-scope", scope, "--verbose")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("1 text files checked", result.stdout)
        arguments = json.loads(self.tidy_log.read_text())
        line_filter = next(item for item in arguments if item.startswith("--line-filter="))
        self.assertEqual(json.loads(line_filter.split("=", 1)[1])[0]["lines"], [[2, 2]])
        self.assertEqual(self.check("--scope", scope, "--path", "src/old.c").returncode, 2)
        self.assertEqual(self.check("--task-scope", self.scope(ranges=[[1, 2]])).returncode, 1)

    def test_task_scope_rejects_stale_hash_baseline_and_ranges(self):
        for arguments in ({"digest": "0" * 64}, {"base": "0" * 40}, {"ranges": [[1, 300]]}):
            with self.subTest(arguments=arguments):
                before = self.snapshot()
                result = self.check("--task-scope", self.scope(**arguments), "--fix")
                self.assertEqual(result.returncode, 2)
                self.assertEqual(before, self.snapshot())

    def test_task_selection_cannot_dispatch_broad_validation(self):
        for arguments in (("--path", "src/old.c"), ("--task-scope", self.scope())):
            with self.subTest(arguments=arguments):
                result = self.check(*arguments, "--validate")
                self.assertEqual(result.returncode, 2)
                self.assertIn("run the relevant named tests separately", result.stderr)

    def test_repeated_task_paths_include_only_exact_files(self):
        self.write("notes.md", "selected \n")
        self.write("other.md", "selected \n")
        self.write("notes.md.extra", "unrelated \n")
        result = self.check("--path", "notes.md", "--path", "other.md")
        self.assertEqual(result.returncode, 1)
        self.assertIn("'notes.md':1", result.stdout)
        self.assertIn("'other.md':1", result.stdout)
        self.assertNotIn("notes.md.extra", result.stdout)
        self.assertEqual(self.check("--path", "src").returncode, 2)
        self.assertEqual(self.check("--path", "../outside.py").returncode, 2)

    def test_task_paths_use_staged_content(self):
        self.write("src/old.c", "int old(void) {\n    return 1;\n}\n")
        self.git("add", "src/old.c")
        self.write("src/old.c", "int old(void) { \n    return 2;\n}\n")
        self.write("tools/unrelated.py", "def invalid syntax\n")
        before = self.snapshot()
        result = self.check("--staged", "--path", "src/old.c")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(before, self.snapshot())

    def test_python_parse_failure_is_distinct_from_readability_advice(self):
        self.write("tools/example.py", "x = 1; y = 2\n")
        result = self.check("--verbose")
        self.assertEqual(result.returncode, 0)
        self.assertIn("REVIEW NOTE", result.stdout)
        self.write("tools/example.py", "if broken\n")
        self.assertEqual(self.check().returncode, 2)

    def test_literals_and_comments_do_not_raise_density_advice(self):
        self.write("src/new.c", 'const char *text = "? ? ; ;";\n/* ? ? ; ; */\n')
        self.assertNotIn("REVIEW NOTE: 'src/new.c'", self.check("--verbose").stdout)

    def test_invalid_base_fails_before_checking_or_writing(self):
        self.write("src/new.c", "int value;\n")
        before = self.snapshot()
        self.assertEqual(self.check("--base=--invalid-ref").returncode, 2)
        self.assertEqual(before, self.snapshot())

    def test_tidy_runs_by_default_and_keeps_success_output_short(self):
        self.write("src/new.c", "int value;\n")
        before = self.snapshot()
        result = self.check()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertTrue(self.tidy_log.exists())
        self.assertEqual(result.stdout, "PASS: formatting and parsed braces checks passed.\n")
        self.assertEqual(before, self.snapshot())

    def test_tidy_opt_out_is_explicit_and_reported(self):
        self.write("src/new.c", "int value;\n")
        result = self.check("--no-tidy")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertFalse(self.tidy_log.exists())
        self.assertIn("explicitly disabled", result.stdout)

    def test_missing_tidy_is_not_silently_skipped(self):
        self.write("src/new.c", "int value;\n")
        self.environment["CLANG_TIDY"] = str(self.directory / "missing")
        before = self.snapshot()
        self.assertEqual(self.check().returncode, 2)
        self.assertEqual(before, self.snapshot())

    def test_tidy_configuration_error_cannot_return_a_false_pass(self):
        self.write("src/new.c", "int value;\n")
        self.tidy.write_text(f"#!{sys.executable}\nimport sys\n"
                             "if '--version' in sys.argv:\n"
                             "    print('clang-tidy version 18.1.3')\n"
                             "else:\n"
                             "    print('Error parsing configuration: error: invalid option')\n")
        result = self.check()
        self.assertEqual(result.returncode, 2, result.stdout)
        self.assertIn("invalid option", result.stderr)

    def test_commit_cannot_omit_the_new_checker_helper(self):
        self.write("tools/check_quality.py", "import quality_tidy\n")
        self.git("add", "tools/check_quality.py")
        before = self.snapshot()
        result = self.check("--staged")
        self.assertEqual(result.returncode, 2)
        self.assertIn("include tools/quality_tidy.py", result.stderr)
        self.assertEqual(before, self.snapshot())

    def test_validation_preserves_behavior_failure_and_harness_error(self):
        self.config["validation"] = [{"name": "fixture", "paths": ["trigger.md"],
                                       "commands": [["fixture.py"]]}]
        self.write(".quality.json", json.dumps(self.config) + "\n")
        self.write("trigger.md", "changed\n")
        for status in (0, 1, 2, 7):
            with self.subTest(status=status):
                self.write("fixture.py", f"import sys\nsys.exit({status})\n")
                result = self.check("--validate")
                self.assertEqual(result.returncode, status if status in (0, 1) else 2, result.stderr)

    def test_staged_validation_uses_staged_support_files(self):
        self.config["validation"] = [{"name": "fixture", "paths": ["trigger.md"],
                                       "commands": [["fixture.py"]]}]
        self.write(".quality.json", json.dumps(self.config) + "\n")
        self.write("fixture.py", "import support\nassert support.VALUE == 1\n")
        self.write("support.py", "VALUE = 1\n")
        self.write("trigger.md", "staged\n")
        self.git("add", ".")
        self.write("support.py", "VALUE = 2\n")
        before = self.snapshot()
        result = self.check("--staged", "--validate")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(before, self.snapshot())

    def test_validation_does_not_forward_the_commit_index_to_nested_checks(self):
        self.config["validation"] = [{"name": "fixture", "paths": ["trigger.md"],
                                       "commands": [["fixture.py"]]}]
        self.write(".quality.json", json.dumps(self.config) + "\n")
        self.write("fixture.py", "import os\nassert 'GIT_INDEX_FILE' not in os.environ\n"
                   "assert 'GIT_DIR' not in os.environ\n")
        self.write("trigger.md", "changed\n")
        self.environment.update(GIT_INDEX_FILE=str(self.root / ".git/index"),
                                GIT_DIR=str(self.root / ".git"))
        before = self.snapshot()
        result = self.check("--validate")
        self.assertEqual(result.returncode, 0, result.stderr + result.stdout)
        self.assertEqual(before, self.snapshot())

    def test_symlinks_cannot_escape_the_snapshot(self):
        outside = self.directory / "outside.c"
        outside.write_text("int value;\n")
        (self.root / "src/link.c").symlink_to(outside)
        self.assertEqual(self.check().returncode, 2)
        self.assertEqual(outside.read_text(), "int value;\n")

    def test_internal_configuration_link_replaces_regular_file(self):
        self.write(".codex/config.toml", "setting = true\n")
        self.git("add", ".codex/config.toml")
        self.git("-c", "user.name=Quality Tests", "-c", "user.email=quality@example.invalid",
                 "commit", "--quiet", "-m", "configuration baseline")
        self.write("llm/codex/config.toml", "setting = true\n")
        link = self.root / ".codex/config.toml"
        link.unlink()
        link.symlink_to("../llm/codex/config.toml")
        result = self.check()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.git("add", ".codex/config.toml", "llm/codex/config.toml")
        before = self.snapshot()
        result = self.check("--staged")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(before, self.snapshot())
        # The staged link remains valid even if the working link later escapes.
        link.unlink()
        link.symlink_to(self.directory / "outside.toml")
        result = self.check("--staged")
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_staged_configuration_link_requires_internal_regular_target(self):
        self.write("llm/config.toml", "setting = true\n")
        link = self.root / "config.toml"
        for target in ("../outside.toml", str(self.root / "llm/config.toml"), "missing.toml"):
            with self.subTest(target=target):
                if link.is_symlink():
                    link.unlink()
                link.symlink_to(target)
                self.git("add", "config.toml", "llm/config.toml")
                result = self.check("--staged")
                self.assertEqual(result.returncode, 2, result.stdout)

    def test_internal_link_does_not_hide_target_whitespace(self):
        self.write("llm/config.toml", "setting = true \n")
        (self.root / "config.toml").symlink_to("llm/config.toml")
        self.git("add", "config.toml", "llm/config.toml")
        result = self.check("--staged")
        self.assertEqual(result.returncode, 1, result.stderr)
        self.assertIn("trailing whitespace", result.stdout)

    def install_hook(self):
        self.write("tools/hooks/pre-commit", (ROOT / "tools/hooks/pre-commit").read_text())
        return subprocess.run([sys.executable, str(ROOT / "tools/install_quality_hook.py")],
                              cwd=self.root, capture_output=True, text=True, check=False)

    def test_hook_installation_is_idempotent_and_preserves_index(self):
        index = (self.root / ".git/index").read_bytes()
        self.assertEqual(self.install_hook().returncode, 0)
        hook = self.root / ".git/hooks/pre-commit"
        content = hook.read_bytes()
        self.assertEqual(self.install_hook().returncode, 0)
        self.assertEqual(content, hook.read_bytes())
        self.assertTrue(hook.stat().st_mode & 0o111)
        self.assertEqual(index, (self.root / ".git/index").read_bytes())

    def test_hook_installer_preserves_existing_hooks_and_configuration(self):
        hook = self.root / ".git/hooks/pre-commit"
        hook.write_text("#!/bin/sh\nexit 7\n")
        self.assertEqual(self.install_hook().returncode, 2)
        self.assertEqual(hook.read_text(), "#!/bin/sh\nexit 7\n")
        hook.unlink()
        self.git("config", "core.hooksPath", "custom-hooks")
        self.assertEqual(self.install_hook().returncode, 2)
        self.assertEqual(self.git("config", "--get", "core.hooksPath").strip(), b"custom-hooks")
        self.assertFalse(hook.exists())

    def test_source_nul_and_invalid_configuration_are_setup_errors(self):
        (self.root / "src/old.c").write_bytes(b"int value;\0\n")
        self.assertEqual(self.check().returncode, 2)
        self.write("src/old.c", "int value;\n")
        self.write(".quality.json", "[]\n")
        self.assertEqual(self.check().returncode, 2)

    @unittest.skipUnless(FORMATTER, "pinned clang-format unavailable")
    def test_real_formatter_handles_project_macros_and_behavior_includes(self):
        self.environment["CLANG_FORMAT"] = FORMATTER
        self.write("src/game/behaviors/example.inc.c", "#define STEP(x) ((x) + 1)\n"
                   "int value(int x) {\n    if (x) {\n        return STEP(x);\n"
                   "    }\n    return 0;\n}\n")
        before = self.snapshot()
        self.assertEqual(self.check("--no-tidy").returncode, 0)
        self.assertEqual(before, self.snapshot())
        self.write("src/game/behaviors/example.inc.c", "int value(int x){return x;}\n")
        result = self.check()
        self.assertEqual(result.returncode, 1, result.stderr)

    @unittest.skipUnless(FORMATTER and TIDY and CLANG, "pinned LLVM tools unavailable")
    def test_real_tidy_reports_braces_and_parse_errors_separately(self):
        self.environment.update(CLANG_FORMAT=FORMATTER, CLANG_TIDY=TIDY, CLANG=CLANG)
        source = self.write("src/new.c", "int value(int x) {\n    if (x)\n"
                            "        return 1;\n    return 0;\n}\n")
        database = self.directory / "build"
        database.mkdir()
        (database / "compile_commands.json").write_text(json.dumps([{
            "directory": str(self.root), "file": str(source),
            "arguments": ["cc", "-std=gnu17", "-DVERSION_US=1", "-c", str(source)]
        }]))
        before = self.snapshot()
        result = self.check("--tidy", str(database))
        self.assertEqual(result.returncode, 1, result.stderr + result.stdout)
        self.assertIn("add braces", result.stdout)
        self.assertEqual(before, self.snapshot())
        self.write("src/new.c", "int value(void) {\n    return missing;\n}\n")
        result = self.check("--tidy", str(database))
        self.assertEqual(result.returncode, 2, result.stderr + result.stdout)

    @unittest.skipUnless(FORMATTER and TIDY and CLANG, "pinned LLVM tools unavailable")
    def test_default_tidy_checks_staged_source_and_configuration(self):
        self.environment.update(CLANG_FORMAT=FORMATTER, CLANG_TIDY=TIDY, CLANG=CLANG)
        self.write("src/new.c", "int value(int x) {\n    if (x)\n"
                   "        return 1;\n    return 0;\n}\n")
        self.git("add", "src/new.c")
        self.write("src/new.c", "int value(int x) {\n    return x;\n}\n")
        self.write(".clang-tidy", "invalid working configuration\n")
        before = self.snapshot()
        result = self.check("--staged")
        self.assertEqual(result.returncode, 1, result.stderr + result.stdout)
        self.assertIn("add braces", result.stdout)
        self.assertEqual(before, self.snapshot())

    @unittest.skipUnless(FORMATTER and TIDY and CLANG, "pinned LLVM tools unavailable")
    def test_fix_does_not_hide_braces_findings_after_line_wrapping(self):
        self.environment.update(CLANG_FORMAT=FORMATTER, CLANG_TIDY=TIDY, CLANG=CLANG)
        source = self.write("src/new.c", "int value(int x){if(x)return 1;return 0;}\n")
        index = (self.root / ".git/index").read_bytes()
        result = self.check("--fix")
        self.assertEqual(result.returncode, 1, result.stderr + result.stdout)
        self.assertIn("add braces", result.stdout)
        self.assertGreater(len(source.read_text().splitlines()), 1)
        self.assertEqual(index, (self.root / ".git/index").read_bytes())

    @unittest.skipUnless(FORMATTER and TIDY and CLANG, "pinned LLVM tools unavailable")
    def test_default_tidy_checks_headers_through_an_active_owner(self):
        self.environment.update(CLANG_FORMAT=FORMATTER, CLANG_TIDY=TIDY, CLANG=CLANG)
        self.write("src/policy.h", "static int policy(int x) {\n    if (x)\n"
                   "        return 1;\n    return 0;\n}\n")
        self.write("src/old.c", '#include "policy.h"\nint old(void) {\n    return policy(1);\n}\n')
        before = self.snapshot()
        result = self.check()
        self.assertEqual(result.returncode, 1, result.stderr + result.stdout)
        self.assertIn("policy.h", result.stdout)
        self.assertIn("add braces", result.stdout)
        self.assertEqual(before, self.snapshot())

    @unittest.skipUnless(FORMATTER and TIDY and CLANG, "pinned LLVM tools unavailable")
    def test_staged_tidy_uses_staged_make_settings(self):
        self.environment.update(CLANG_FORMAT=FORMATTER, CLANG_TIDY=TIDY, CLANG=CLANG)
        makefile = (self.root / "Makefile").read_text()
        self.write("Makefile", makefile.replace("-std=gnu17", "-std=gnu17 -DREVIEWED=1"))
        self.write("src/new.c", "#ifndef REVIEWED\n#error wrong compiler settings\n#endif\n"
                   "int value(void) {\n    return REVIEWED;\n}\n")
        self.git("add", "Makefile", "src/new.c")
        self.write("Makefile", makefile)
        before = self.snapshot()
        result = self.check("--staged")
        self.assertEqual(result.returncode, 0, result.stderr + result.stdout)
        self.assertEqual(before, self.snapshot())

    @unittest.skipUnless(FORMATTER and TIDY and CLANG, "pinned LLVM tools unavailable")
    def test_inactive_header_is_not_claimed_as_checked(self):
        self.environment.update(CLANG_FORMAT=FORMATTER, CLANG_TIDY=TIDY, CLANG=CLANG)
        self.write("src/policy.h", "int value;\n")
        self.write("src/old.c", '#if 0\n#include "policy.h"\n#endif\n'
                   'int old(void) {\n    return 0;\n}\n')
        before = self.snapshot()
        result = self.check()
        self.assertEqual(result.returncode, 2, result.stderr + result.stdout)
        self.assertIn("no active, parseable translation unit", result.stderr)
        self.assertEqual(before, self.snapshot())

    @unittest.skipUnless(FORMATTER and TIDY and CLANG, "pinned LLVM tools unavailable")
    def test_explicit_database_is_remapped_into_the_staged_snapshot(self):
        self.environment.update(CLANG_FORMAT=FORMATTER, CLANG_TIDY=TIDY, CLANG=CLANG)
        source = self.write("src/new.c", "int value(int x) {\n    if (x)\n"
                            "        return 1;\n    return 0;\n}\n")
        self.git("add", "src/new.c")
        self.write("src/new.c", "int value(int x) {\n    return x;\n}\n")
        database = self.directory / "database"
        database.mkdir()
        (database / "compile_commands.json").write_text(json.dumps([{
            "directory": str(self.root), "file": str(source),
            "arguments": ["cc", "-std=gnu17", "-c", str(source)]
        }]))
        before = self.snapshot()
        result = self.check("--staged", "--tidy", str(database))
        self.assertEqual(result.returncode, 1, result.stderr + result.stdout)
        self.assertIn("add braces", result.stdout)
        self.assertEqual(before, self.snapshot())

    @unittest.skipUnless(FORMATTER and TIDY and CLANG, "pinned LLVM tools unavailable")
    def test_explicit_root_include_cannot_hide_a_staged_header_defect(self):
        self.environment.update(CLANG_FORMAT=FORMATTER, CLANG_TIDY=TIDY, CLANG=CLANG)
        header = self.write("policy.h", "static int policy(int x) {\n    return x;\n}\n")
        source = self.write("src/old.c", '#include <policy.h>\n'
                            'int old(void) {\n    return policy(1);\n}\n')
        self.git("add", "policy.h", "src/old.c")
        self.git("-c", "user.name=Quality Tests", "-c", "user.email=quality@example.invalid",
                 "commit", "--quiet", "-m", "header owner")
        self.write("policy.h", "static int policy(int x) {\n    if (x)\n"
                   "        return 1;\n    return 0;\n}\n")
        self.git("add", "policy.h")
        self.write("policy.h", "static int policy(int x) {\n    return x;\n}\n")
        database = self.directory / "database"
        database.mkdir()
        for includes in (["-I", str(self.root)], ["-I" + str(self.root)]):
            with self.subTest(includes=includes):
                (database / "compile_commands.json").write_text(json.dumps([{
                    "directory": str(self.root), "file": str(source),
                    "arguments": ["cc", "-std=gnu17", *includes, "-c", str(source)]
                }]))
                before = self.snapshot()
                result = self.check("--staged", "--tidy", str(database))
                self.assertEqual(result.returncode, 1, result.stderr + result.stdout)
                self.assertIn(header.name, result.stdout)
                self.assertIn("add braces", result.stdout)
                self.assertEqual(before, self.snapshot())

    def test_host_tool_database_does_not_use_n64_compiler_flags(self):
        source = self.write("tools/host_tool.c", '#include <dlfcn.h>\nint host_tool;\n')
        temporary = self.directory / "host-inputs"
        temporary.mkdir()
        with patch.object(quality_tidy, "fixture_commands", return_value=[]):
            with patch.object(quality_tidy, "make_settings") as settings:
                owners = quality_tidy.prepare_database(
                    self.root, self.root, [SimpleNamespace(path="tools/host_tool.c")],
                    temporary, "clang", "auto")
        settings.assert_not_called()
        self.assertEqual(owners, [source])
        entry = json.loads((temporary / "compile_commands.json").read_text())[0]
        self.assertEqual(entry["arguments"],
                         ["clang", "-std=gnu17", "-I", str(source.parent), "-c", str(source)])

    def texture_snapshot(self):
        snapshot = self.directory / "snapshot"
        temporary = self.directory / "inputs"
        (snapshot / "bin").mkdir(parents=True)
        (snapshot / "tools").mkdir()
        (temporary / "include").mkdir(parents=True)
        source = snapshot / "bin/textures.c"
        source.write_text('#include "textures/sample.ia8.inc.c"\n')
        return snapshot, temporary, [SimpleNamespace(path="bin/textures.c")]

    def test_texture_conversion_uses_selected_png_and_preswap(self):
        snapshot, temporary, changes = self.texture_snapshot()
        source = snapshot / "bin/textures.c"
        source.write_text('#include "textures/sample.ia8.preswap.inc.c"\n')
        png = snapshot / "textures/sample.ia8.preswap.png"
        png.parent.mkdir()
        png.write_bytes(b"selected PNG")
        (temporary / "n64graphics").touch()
        calls = []

        def convert(arguments, directory):
            calls.append(arguments)
            if "-E" in arguments:
                return 'QUALITY_TEXTURE("textures/sample.ia8.preswap.inc.c")'
            self.assertEqual(Path(arguments[arguments.index("-g") + 1]), png)
            self.assertEqual(arguments[arguments.index("-f") + 1], "ia8")
            self.assertIn("-S", arguments)
            return ""

        with patch.object(quality_tidy, "command", side_effect=convert):
            quality_tidy.generated_textures(self.root, snapshot, temporary, [], "clang", changes)
        self.assertEqual(len(calls), 2)
        self.assertEqual(png.read_bytes(), b"selected PNG")

    def test_vanilla_texture_uses_verified_rom_instead_of_working_png(self):
        snapshot, temporary, changes = self.texture_snapshot()
        working_png = self.root / "textures/sample.ia8.png"
        working_png.parent.mkdir()
        working_png.write_bytes(b"unstaged texture")
        rom = b"vanilla bytes"
        (self.root / "baserom.sh.z64").write_bytes(rom)
        (snapshot / "tools/detect_baseroms.py").write_text(
            "sha1_LUT = " + repr({"sh": hashlib.sha1(rom).hexdigest()}) + "\n")
        (snapshot / "assets.json").write_text(json.dumps(
            {"textures/sample.ia8.png": [2, 1, 2, {"sh": [1]}]}))
        with patch.object(quality_tidy, "command", return_value='QUALITY_TEXTURE("textures/sample.ia8.inc.c")'):
            quality_tidy.generated_textures(self.root, snapshot, temporary,
                                           ["-DVERSION_US=1"], "clang", changes)
        self.assertEqual((temporary / "include/textures/sample.ia8.inc.c").read_text(),
                         "0x61, 0x6E,\n")
        self.assertEqual(working_png.read_bytes(), b"unstaged texture")
        self.assertFalse((snapshot / "textures").exists())

    def test_texture_setup_follows_selected_level_include_owner(self):
        snapshot, temporary, _ = self.texture_snapshot()
        level = snapshot / "levels/sample"
        level.mkdir(parents=True)
        (level / "collision.inc.c").write_text("const int collision[] = {1};\n")
        (level / "texture.inc.c").write_text('#include "levels/sample/0.rgba16.inc.c"\n')
        (level / "leveldata.c").write_text(
            '#include "collision.inc.c"\n#include "texture.inc.c"\n')
        png = level / "0.rgba16.png"
        png.write_bytes(b"selected level PNG")
        (temporary / "n64graphics").touch()
        calls = []

        def convert(arguments, directory):
            calls.append(arguments)
            if "-E" in arguments:
                return 'QUALITY_TEXTURE("levels/sample/0.rgba16.inc.c")'
            self.assertEqual(Path(arguments[arguments.index("-g") + 1]), png)
            self.assertEqual(arguments[arguments.index("-f") + 1], "rgba16")
            Path(arguments[arguments.index("-i") + 1]).write_text("0x01,\n")
            return ""

        with patch.object(quality_tidy, "command", side_effect=convert):
            quality_tidy.generated_textures(
                self.root, snapshot, temporary, ["-I", str(snapshot)], "clang",
                [SimpleNamespace(path="levels/sample/collision.inc.c")],
                ["levels/sample/leveldata.c", "bin/textures.c"])
        self.assertEqual(sum("-g" in arguments for arguments in calls), 1)
        self.assertTrue(any(Path(arguments[-1]).name == "leveldata.c"
                            for arguments in calls if "-E" in arguments))

    def test_texture_setup_prepares_nested_models_before_parent_probe(self):
        snapshot, temporary, changes = self.texture_snapshot()
        actor = snapshot / "actors/sample"
        actor.mkdir(parents=True)
        (actor / "model.inc.c").write_text('#include "actors/sample/face.ia8.inc.c"\n')
        (snapshot / "bin/textures.c").write_text(
            '#include "textures/sample.ia8.inc.c"\n#include "actors/sample/model.inc.c"\n')
        textures = snapshot / "textures"
        textures.mkdir()
        (textures / "sample.ia8.png").write_bytes(b"parent PNG")
        (actor / "face.ia8.png").write_bytes(b"nested PNG")
        (temporary / "n64graphics").touch()
        real_command = quality_tidy.command

        def convert(arguments, directory):
            if "-E" in arguments:
                return real_command(arguments, directory)
            Path(arguments[arguments.index("-i") + 1]).write_text("0x01,\n")
            return ""

        with patch.object(quality_tidy, "command", side_effect=convert):
            quality_tidy.generated_textures(
                self.root, snapshot, temporary,
                ["-I", str(temporary / "include"), "-I", str(snapshot)], "cc", changes)
        self.assertEqual((temporary / "include/actors/sample/face.ia8.inc.c").read_text(), "0x01,\n")
        self.assertEqual((temporary / "include/textures/sample.ia8.inc.c").read_text(), "0x01,\n")

    def test_texture_setup_rejects_modified_baserom(self):
        snapshot, temporary, changes = self.texture_snapshot()
        (self.root / "baserom.us.z64").write_bytes(b"modified")
        (snapshot / "tools/detect_baseroms.py").write_text("sha1_LUT = {'us': 'wrong'}\n")
        (snapshot / "assets.json").write_text(json.dumps(
            {"textures/sample.ia8.png": [2, 1, 2, {"us": [0]}]}))
        with patch.object(quality_tidy, "command", return_value='QUALITY_TEXTURE("textures/sample.ia8.inc.c")'):
            with self.assertRaisesRegex(quality_tidy.TidySetupError, "unmodified"):
                quality_tidy.generated_textures(self.root, snapshot, temporary,
                                               ["-DVERSION_US=1"], "clang", changes)
        self.assertFalse((temporary / "include/textures/sample.ia8.inc.c").exists())

    def test_texture_setup_retains_caller_macros_for_repeated_include(self):
        snapshot, temporary, changes = self.texture_snapshot()
        actor = snapshot / "actors/sample"
        actor.mkdir(parents=True)
        (actor / "palette.inc.c").write_text(
            '#if VARIANT\n#include "actors/sample/alternate.rgba16.inc.c"\n'
            '#else\n#include "actors/sample/native.rgba16.inc.c"\n#endif\n')
        (snapshot / "bin/textures.c").write_text(
            '#define VARIANT 0\n#include "actors/sample/palette.inc.c"\n'
            '#undef VARIANT\n#define VARIANT 1\n#include "actors/sample/palette.inc.c"\n')
        for name in ("native", "alternate"):
            (actor / (name + ".rgba16.png")).write_bytes(name.encode())
        (temporary / "n64graphics").touch()
        real_command = quality_tidy.command
        converted = []

        def convert(arguments, directory):
            if "-E" in arguments:
                return real_command(arguments, directory)
            png = Path(arguments[arguments.index("-g") + 1])
            converted.append(png.name)
            Path(arguments[arguments.index("-i") + 1]).write_text("0x01,\n")
            return ""

        with patch.object(quality_tidy, "command", side_effect=convert):
            quality_tidy.generated_textures(
                self.root, snapshot, temporary,
                ["-I", str(temporary / "include"), "-I", str(snapshot)], "cc", changes)
        self.assertEqual(sorted(converted), ["alternate.rgba16.png", "native.rgba16.png"])
        self.assertFalse((actor / "alternate.rgba16.inc.c").exists())
        # An inactive default variant must not require its PNG or a baserom.
        (actor / "native.rgba16.png").unlink()
        for name in ("native", "alternate"):
            (temporary / "include/actors/sample" / (name + ".rgba16.inc.c")).unlink()
        (snapshot / "bin/textures.c").write_text(
            '#define VARIANT 1\n#include "actors/sample/palette.inc.c"\n')
        converted.clear()
        with patch.object(quality_tidy, "command", side_effect=convert):
            quality_tidy.generated_textures(
                self.root, snapshot, temporary,
                ["-I", str(temporary / "include"), "-I", str(snapshot)], "cc", changes)
        self.assertEqual(converted, ["alternate.rgba16.png"])

    def test_ci_splash_recovers_rgba_source_and_both_outputs(self):
        snapshot, temporary, changes = self.texture_snapshot()
        stem = "actors/water_splash/water_splash_0"
        (snapshot / "bin/textures.c").write_text(
            '#include "' + stem + '.ci8.inc.c"\n#include "' + stem + '.ci8.pal"\n')
        rom = bytes(range(32))
        (self.root / "baserom.us.z64").write_bytes(rom)
        (snapshot / "tools/detect_baseroms.py").write_text(
            "sha1_LUT = " + repr({"us": hashlib.sha1(rom).hexdigest()}) + "\n")
        (snapshot / "assets.json").write_text(json.dumps(
            {stem + ".rgba16.png": [32, 64, 16, {"us": [4]}]}))
        (temporary / "n64graphics").touch()
        real_command = quality_tidy.command
        conversions = []

        def convert(arguments, directory):
            if "-E" in arguments:
                return real_command(arguments, directory)
            conversions.append(arguments)
            self.assertEqual(arguments[arguments.index("-w") + 1], "32")
            self.assertEqual(arguments[arguments.index("-h") + 1], "64")
            if "-e" in arguments:
                self.assertEqual(Path(arguments[arguments.index("-e") + 1]).read_bytes(), rom[4:20])
                self.assertEqual(arguments[arguments.index("-f") + 1], "rgba16")
                Path(arguments[arguments.index("-g") + 1]).write_bytes(b"recovered PNG")
            else:
                self.assertEqual(Path(arguments[arguments.index("-g") + 1]).read_bytes(), b"recovered PNG")
                self.assertEqual(arguments[arguments.index("-f") + 1], "ci8")
                self.assertEqual(arguments[arguments.index("-c") + 1], "rgba16")
                Path(arguments[arguments.index("-i") + 1]).write_text("0x01,\n")
                Path(arguments[arguments.index("-p") + 1]).write_text("0xFF, 0xFF,\n")
            return ""

        with patch.object(quality_tidy, "command", side_effect=convert):
            quality_tidy.generated_textures(
                self.root, snapshot, temporary,
                ["-DVERSION_US=1", "-I", str(temporary / "include")], "cc", changes)
        self.assertEqual(len(conversions), 2)
        self.assertEqual((temporary / "include" / (stem + ".ci8.inc.c")).read_text(), "0x01,\n")
        self.assertEqual((temporary / "include" / (stem + ".ci8.pal")).read_text(), "0xFF, 0xFF,\n")
        self.assertFalse((snapshot / "actors").exists())

    def test_project_make_wrapper_and_maintenance_do_not_prepare_build_inputs(self):
        directory = self.directory / "make-directory-settings"
        directory.mkdir()
        files = [ROOT / name for name in ("Makefile", "Makefile.split", "util.mk")]
        files += list((ROOT / "compression").glob("*.mk"))
        for source in files:
            destination = directory / source.relative_to(ROOT)
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(source, destination)
        commands = directory / "commands"
        commands.mkdir()
        mkdir = commands / "mkdir"
        mkdir.write_text('#!/bin/sh\nprintf "%s\\n" "$@" >> mkdir.log\n')
        mkdir.chmod(0o755)
        environment = dict(os.environ, PATH=str(commands) + os.pathsep + os.environ["PATH"])
        arguments = ["make", "-s", "N64_TOOLCHAIN_PREFIX=fixture-", "LD=true"]
        result = subprocess.run(arguments + ["print-ALL_DIRS"], cwd=directory,
                                env=environment, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        directories = result.stdout.split("set to [", 1)[1].split("]", 1)[0].split()
        self.assertTrue(directories)
        self.assertTrue(all(path.startswith("build/us_n64") for path in directories), directories)
        self.assertFalse((directory / "mkdir.log").exists())
        recursive = commands / "recursive-make"
        recursive.write_text('#!/bin/sh\nprintf "%s\\n" "$*" >> recursive-make.log\n')
        recursive.chmod(0o755)
        result = subprocess.run(arguments + ["MAKE=" + str(recursive)], cwd=directory,
                                env=environment, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        calls = (directory / "recursive-make.log").read_text().splitlines()
        self.assertEqual(len(calls), 2)
        self.assertTrue(calls[0].endswith("-C tools"), calls)
        self.assertTrue(calls[1].endswith(" all"), calls)
        self.assertFalse((directory / "build").exists())
        self.assertFalse((directory / "mkdir.log").exists())
        build = directory / "build"
        build.mkdir()
        (build / "old-output").touch()
        result = subprocess.run(arguments + ["clean"], cwd=directory,
                                env=environment, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertFalse(build.exists())
        self.assertFalse((directory / "mkdir.log").exists())

    def test_project_make_refreshes_removed_header_dependencies(self):
        cc = shutil.which("cc")
        if cc is None:
            self.skipTest("host C compiler is unavailable")
        directory = self.directory / "make-header-dependencies"
        directory.mkdir()
        (directory / "out").mkdir()
        makefile = (ROOT / "Makefile").read_text()
        rule = "$(BUILD_DIR)/%.o: %.c\n" + makefile.split(
            "$(BUILD_DIR)/%.o: %.c\n", 1)[1].split("$(BUILD_DIR)/%.o: %.cpp", 1)[0]
        (directory / "Makefile").write_text(
            "CC := " + cc + "\nBUILD_DIR := out\nall: out/unit.o\n" + rule + "-include out/unit.d\n")
        header = directory / "retired.h"
        header.write_text("#define VALUE 1\n")
        source = directory / "unit.c"
        source.write_text('#include "retired.h"\nint value = VALUE;\n')
        result = subprocess.run(["make", "-s"], cwd=directory, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        header.unlink()
        source.write_text("int value = 2;\n")
        result = subprocess.run(["make", "-s"], cwd=directory, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertNotIn("retired.h", (directory / "out/unit.d").read_text())
        source.write_text('#include "retired.h"\nint value = VALUE;\n')
        result = subprocess.run(["make", "-s"], cwd=directory, capture_output=True, text=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("retired.h", result.stderr)

    def test_project_make_metadata_does_not_prepare_build_outputs(self):
        directory = self.directory / "make-settings"
        directory.mkdir()
        files = [ROOT / name for name in ("Makefile", "Makefile.split", "util.mk")]
        files += list((ROOT / "compression").glob("*.mk"))
        for source in files:
            destination = directory / source.relative_to(ROOT)
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(source, destination)
        before = {str(path.relative_to(directory)): path.read_bytes()
                  for path in directory.rglob("*") if path.is_file()}
        result = subprocess.run(["make", "-s", "COMPILER=clang", "quality-flags"],
                                cwd=directory, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("-DTARGET_N64", result.stdout)
        self.assertNotIn("Building", result.stdout)
        after = {str(path.relative_to(directory)): path.read_bytes()
                 for path in directory.rglob("*") if path.is_file()}
        self.assertEqual(before, after)
        self.assertFalse((directory / "build").exists())


if __name__ == "__main__":
    unittest.main()
