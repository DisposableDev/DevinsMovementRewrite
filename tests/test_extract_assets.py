#!/usr/bin/env python3
"""Verify missing-asset repair and preservation of compatible project assets."""

from contextlib import redirect_stdout
from io import StringIO
import json
import os
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
import extract_assets


class AssetExtractionTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory(prefix="sm64-extract-assets-")
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        previous = Path.cwd()
        self.addCleanup(os.chdir, previous)
        os.chdir(self.root)
        (self.root / "assets").mkdir()
        (self.root / "assets/existing.bin").write_bytes(b"ZZ")
        (self.root / "baserom.test.z64").write_bytes(b"abcdef")
        self.assets = {
            "assets/existing.bin": [2, {"us": [0]}],
            "assets/missing.bin": [2, {"us": [2]}],
        }

    def extract(self, languages=(), version=8):
        (self.root / "assets.json").write_text(json.dumps(self.assets))
        (self.root / ".assets-local.txt").write_text(
            "# Fixture\n" + str(version) + "\n" + "\n".join(self.assets) + "\n")
        with patch.object(sys, "argv", ["extract_assets.py", *languages]), \
                patch.object(extract_assets, "get_rom_candidates",
                             return_value={"us": "baserom.test.z64"}) as scan, \
                patch.object(extract_assets.subprocess, "check_call") as tools, \
                redirect_stdout(StringIO()):
            extract_assets.main()
        return scan, tools

    def assert_repaired_and_preserved(self):
        self.assertEqual((self.root / "assets/missing.bin").read_bytes(), b"cd")
        self.assertEqual((self.root / "assets/existing.bin").read_bytes(), b"ZZ")

    def test_implicit_regions_repair_missing_assets_in_a_current_cache(self):
        self.extract()
        self.assert_repaired_and_preserved()

    def test_explicit_region_repair_preserves_existing_custom_assets(self):
        self.extract(["us"])
        self.assert_repaired_and_preserved()

    def test_complete_current_cache_does_not_scan_roms_or_build_tools(self):
        (self.root / "assets/missing.bin").write_bytes(b"cd")
        scan, tools = self.extract()
        scan.assert_not_called()
        tools.assert_not_called()

    def test_missing_asset_outside_requested_region_does_not_trigger_extraction(self):
        self.assets["assets/missing.bin"] = [2, {"sh": [2]}]
        scan, tools = self.extract(["us"])
        scan.assert_not_called()
        tools.assert_not_called()
        self.assertFalse((self.root / "assets/missing.bin").exists())

    def test_old_cache_updates_only_assets_with_a_required_migration(self):
        self.assets["assets/kept.bin"] = [2, {"us": [4]}]
        (self.root / "assets/kept.bin").write_bytes(b"KK")
        with patch.object(extract_assets, "envmap_table", {"assets/existing.bin"}):
            self.extract(version=7)
        self.assertEqual((self.root / "assets/existing.bin").read_bytes(), b"ab")
        self.assertEqual((self.root / "assets/kept.bin").read_bytes(), b"KK")
        self.assertEqual((self.root / "assets/missing.bin").read_bytes(), b"cd")


if __name__ == "__main__":
    unittest.main()
