"""Use the asset-authoring reader in standalone and package test runs."""
from pathlib import Path
import runpy

read_text = runpy.run_path(str(Path(__file__).resolve().parents[1] / "tools/source_asset.py"))["read_text"]
