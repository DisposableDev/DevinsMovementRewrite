"""Read authoring source with reconstructed ROM initializer includes expanded."""
import os
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
INCLUDE = re.compile(r'\n#include "(assets/rom/generated/[0-9a-f]{20}\.inc\.c)"\n*', re.M)

def read_text(path):
    text = Path(path).read_text()
    # Only explicit syntax-analysis callers may redirect reconstructed includes.
    frontend = os.environ.get('SM64_FRONTEND_ASSET_ROOT')
    verified = os.environ.get('SM64_VERIFIED_ASSET_ROOT')
    assets = Path(frontend or verified) if frontend or verified else ROOT
    if (frontend or verified) and not assets.is_absolute():
        raise ValueError('Asset root must be an absolute private directory')
    return INCLUDE.sub(lambda match: (assets / match[1]).read_text() + '\n', text)
