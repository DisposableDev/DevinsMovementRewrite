import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import tarfile
import tempfile
import unittest
from unittest.mock import patch
from types import SimpleNamespace

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import export_source
import quality_tidy
import check_quality
import source_asset
import check_first_import

try:
    CLANG = check_quality.clang_tool(ROOT, "clang", "18.1.3")
except check_quality.SetupError:
    CLANG = None


class PublicSourceTests(unittest.TestCase):
    def test_reconstructed_asset_policy_preserves_gameplay_code_checks(self):
        config = check_quality.load_config(ROOT)
        for name in ("actors/bully/anims/anim_05004598.inc.c", "actors/amp/anims/animation.inc.c",
                     "levels/bbh/areas/1/1/model.inc.c", "levels/wf/small_bomp/1.inc.c"):
            self.assertIsNotNone(check_quality.exclusion(config, name))
        for name in ("src/game/mario_actions_airborne.c", "src/game/camera.c",
                     "levels/bowser_3/script.c", "actors/bully/geo.inc.c"):
            self.assertIsNone(check_quality.exclusion(config, name))

    def test_first_import_freezes_legacy_source_and_rejects_new_or_changed_files(self):
        with tempfile.TemporaryDirectory() as name:
            root = Path(name)
            subprocess.run(["git", "init", "-q", str(root)], check=True)
            (root / "assets.json").write_text("{}\n")
            (root / "tools").mkdir()
            (root / "tools/source-release-policy.json").write_text('{"exclude": {}, "allow_media": {}}\n')
            (root / "legacy.c").write_text("int legacy(){return 0;}\n")
            (root / check_first_import.SCOPE).write_text(json.dumps({"policy": "Fixture import", "files": {}}))
            subprocess.run(["git", "add", "."], cwd=root, check=True)
            subprocess.run(["git", "-c", "user.name=Fixture", "-c", "user.email=fixture@example.invalid", "commit", "-qm", "fixture"], cwd=root, check=True)
            check_first_import.refresh(root)
            scope = json.loads((root / check_first_import.SCOPE).read_text())
            self.assertEqual(check_first_import.verify(root, scope), 1)
            (root / "legacy.c").write_text("int legacy(){return 1;}\n")
            with self.assertRaisesRegex(ValueError, "source changed"):
                check_first_import.verify(root, scope)
            (root / "legacy.c").write_text("int legacy(){return 0;}\n")
            (root / "new.c").write_text("int added;\n")
            with self.assertRaisesRegex(ValueError, "inventory changed"):
                check_first_import.verify(root, scope)

    def test_source_reader_requires_explicit_private_frontend_directory(self):
        with tempfile.TemporaryDirectory() as name:
            root = Path(name)
            source = root / "source.c"
            identity = "0123456789abcdef0123"
            source.write_text('const short values[] = {\n#include "assets/rom/generated/' + identity + '.inc.c"\n};\n')
            include = root / "frontend/assets/rom/generated"
            include.mkdir(parents=True)
            (include / (identity + ".inc.c")).write_text("0, 0,")
            with patch.dict(os.environ):
                os.environ.pop("SM64_FRONTEND_ASSET_ROOT", None)
                with self.assertRaises(FileNotFoundError):
                    source_asset.read_text(source)
                os.environ["SM64_FRONTEND_ASSET_ROOT"] = str(root / "frontend")
                text = source_asset.read_text(source)
                self.assertIn("0, 0,", text)
                self.assertNotIn("#include", text)
                os.environ["SM64_FRONTEND_ASSET_ROOT"] = "relative"
                with self.assertRaisesRegex(ValueError, "absolute private directory"):
                    source_asset.read_text(source)

    def test_frontend_refuses_asset_validation_and_source_rewrites(self):
        for option in ("--validate", "--fix"):
            with self.subTest(option=option):
                result = subprocess.run(
                    [sys.executable, str(ROOT / "tools/check_quality.py"), "--frontend-only", option],
                    cwd=ROOT, capture_output=True, text=True)
                self.assertEqual(result.returncode, 2)
                self.assertIn("cannot validate assets or rewrite source", result.stderr)

    @unittest.skipUnless(CLANG, "LLVM 18 Clang unavailable")
    def test_frontend_texture_shape_and_strict_default(self):
        with tempfile.TemporaryDirectory() as name:
            root = Path(name)
            (root / "source.c").write_text('unsigned char demo[] = {\n#include "textures/demo.rgba16.inc.c"\n};\n')
            (root / "assets.json").write_text(json.dumps({"textures/demo.rgba16.png": [4, 4, 32, {"us": [0]}]}))
            change = SimpleNamespace(path="source.c")
            for frontend in (False, True):
                temp = root / str(frontend)
                (temp / "include").mkdir(parents=True)
                arguments = (root, root, temp, ["-DVERSION_US=1"], str(CLANG), [change])
                if not frontend:
                    with self.assertRaisesRegex(quality_tidy.TidySetupError, "baserom.*required"):
                        quality_tidy.generated_textures(*arguments)
                else:
                    quality_tidy.generated_textures(*arguments, frontend=True)
                    payload = (temp / "include/textures/demo.rgba16.inc.c").read_text()
                    self.assertEqual(payload.count("0"), 32)
                    subprocess.run([str(CLANG), "-I", str(temp / "include"), "-fsyntax-only", str(root / "source.c")], check=True)

    def test_initializer_templates_use_zero_values(self):
        with tempfile.TemporaryDirectory() as name:
            root = Path(name)
            (root / "assets/rom").mkdir(parents=True)
            (root / "assets/rom/arrays.json").write_text(json.dumps({"0123456789abcdef0123": {"type": "s16", "template": "@0x4X@, @d@, @-0x4x@,"}}))
            temp = root / "temp"
            quality_tidy.generated_frontend_arrays(root, temp)
            self.assertEqual((temp / "include/assets/rom/generated/0123456789abcdef0123.inc.c").read_text(), "0, 0, 0,")

    def test_package_inventory_comparison(self):
        previous = {"schema": 1, "include": {"same.c": {"sha256": "same", "mode": "0644"},
                    "edited.c": {"sha256": "before"}, "removed.c": {"sha256": "old"},
                    "tool": {"sha256": "tool", "mode": "0644"}},
                    "exclude": {"local.log": "generated"}, "gitlinks": {"lib/libpl": "old"}}
        current = {"schema": 1, "include": {"same.c": {"sha256": "same", "mode": "0644"},
                   "edited.c": {"sha256": "after"}, "new.c": {"sha256": "new"},
                   "tool": {"sha256": "tool", "mode": "0755"}},
                   "exclude": {"local.log": "local"}, "gitlinks": {"lib/libpl": "new"}}
        diff = export_source.compare_inventories(previous, current, "baseline digest")
        self.assertEqual(diff["added"], ["new.c"])
        self.assertEqual(diff["changed"], ["edited.c", "tool"])
        self.assertEqual(diff["removed"], ["removed.c"])
        self.assertEqual(diff["unchanged"], 1)
        self.assertEqual(diff["exclusion_changes"], ["local.log"])
        self.assertEqual(diff["metadata_changes"], ["gitlinks"])
        self.assertEqual(diff["baseline_sha256"], "baseline digest")
        with self.assertRaises(ValueError):
            export_source.compare_inventories({"schema": 99, "include": {}}, current, "bad")

    def test_export_exclusions_safe_links_and_repeatability(self):
        with tempfile.TemporaryDirectory() as name:
            root = Path(name) / "repo"
            root.mkdir()
            subprocess.run(["git", "init", "-q", str(root)], check=True)
            (root / "assets.json").write_text('{"original.png": [1,1,1,{"us":[0]}]}')
            (root / ".gitattributes").write_text("private.txt export-ignore\n")
            for filename in ("original.png", "source.c", "private.txt", "baserom.us.z64", "custom.png", "tool.o"):
                (root / filename).write_text("int value;\n")
            (root / "renamed-host-tool").write_bytes(b"\x7fELF" + bytes(20))
            (root / "source-link").symlink_to("source.c")
            (root / "escape-link").symlink_to("../outside")
            subprocess.run(["git", "add", "."], cwd=root, check=True)
            subprocess.run(["git", "-c", "user.name=Fixture", "-c", "user.email=fixture@example.invalid", "commit", "-qm", "fixture"], cwd=root, check=True)
            policy = {"allow_media": {"custom.png": "user-authorized custom edit"}, "exclude": {"escape-link": "private fixture"}}
            report = export_source.inventory(root, policy)
            report["changes_from_previous"] = export_source.compare_inventories(
                {"schema": 1, "include": {}, "exclude": {}}, report, "previous inventory hash")
            self.assertEqual(set(report["include"]), {"assets.json", ".gitattributes", "source.c", "source-link", "custom.png"})
            self.assertFalse(report["block"])
            for output in (Path(name) / "first.tar.gz", Path(name) / "second.tar.gz"):
                export_source.archive(root, report, output)
            self.assertEqual((Path(name) / "first.tar.gz").read_bytes(), (Path(name) / "second.tar.gz").read_bytes())
            with tarfile.open(Path(name) / "first.tar.gz") as archive:
                manifest = json.load(archive.extractfile("my-first-hack/SOURCE-MANIFEST.json"))
                self.assertEqual(manifest["files"], report["include"])
                self.assertEqual(manifest["gitlinks"], report["gitlinks"])
                self.assertEqual(manifest["changes_from_previous"], report["changes_from_previous"])
            self.assertIn("escape-link", export_source.inventory(root, {"allow_media": policy["allow_media"]})["block"])
            tree = check_quality.revision(root, "EMPTY_TREE")
            changed = check_quality.changed_files(root, tree, False)
            self.assertIn("source.c", changed)
            self.assertTrue(all(change.status == "A" for change in changed.values()))
            sdk = root / "tools/sdk-tools"
            sdk.mkdir(parents=True)
            (sdk / "README.md").write_text("SDK source provenance fixture\n")
            blocked = export_source.inventory(root, policy)
            self.assertTrue(blocked["publication_blockers"])
            output = Path(name) / "blocked.tar.gz"
            with self.assertRaisesRegex(ValueError, "private-checkpoint"):
                export_source.archive(root, blocked, output)
            self.assertFalse(output.exists())
            export_source.archive(root, blocked, output, private_checkpoint=True)
            with tarfile.open(output) as archive:
                manifest = json.load(archive.extractfile("my-first-hack/SOURCE-MANIFEST.json"))
                self.assertTrue(manifest["private_checkpoint"])
                self.assertTrue(manifest["publication_blockers"])

            library = root / "lib/sdk.a"
            library.parent.mkdir()
            library.write_bytes(b"!<arch>\nSDK fixture")
            (root / "lib/unused.a").write_bytes(b"!<arch>\nunused fixture")
            rom = root / "lib/renamed-rom.bin"
            rom.write_bytes(b"\x80\x37\x12\x40" + bytes(20))
            lock = root / "tools/build-dependencies.json"
            lock.write_text(json.dumps({"files": [{"path": "lib/sdk.a",
                "category": "inherited SDK archive",
                "sha256": hashlib.sha256(library.read_bytes()).hexdigest()}]}))
            policy["inherited_dependencies"] = {"manifest": "tools/build-dependencies.json",
                "origin": "HackerSM64 fixture", "decision": "Owner requests inherited SDK inclusion"}
            accepted = export_source.inventory(root, policy)
            self.assertFalse(accepted["block"])
            self.assertFalse(accepted["publication_blockers"])
            self.assertTrue(accepted["provenance_notes"])
            self.assertIn("lib/sdk.a", accepted["include"])
            self.assertIn("lib/unused.a", accepted["exclude"])
            self.assertIn("lib/renamed-rom.bin", accepted["exclude"])
            output = Path(name) / "inherited-sdk.tar.gz"
            export_source.archive(root, accepted, output)
            with tarfile.open(output) as archive:
                manifest = json.load(archive.extractfile("my-first-hack/SOURCE-MANIFEST.json"))
                self.assertEqual(manifest["inherited_dependencies"], policy["inherited_dependencies"])
                self.assertTrue(manifest["provenance_notes"])
                self.assertFalse(manifest["private_checkpoint"])
            library.write_bytes(b"changed dependency")
            self.assertIn("lib/sdk.a", export_source.inventory(root, policy)["block"])


if __name__ == "__main__":
    unittest.main()
