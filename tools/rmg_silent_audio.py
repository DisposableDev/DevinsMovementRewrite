"""Build the process-local SDL3 device selector used by silent RMG trials."""

import hashlib
from pathlib import Path
import subprocess
import uuid


def silent_audio_library():
    root = Path(__file__).resolve().parents[1]
    source = root / "tools/rmg_silent_audio.c"
    digest = hashlib.sha256(source.read_bytes()).hexdigest()[:16]
    directory = root / "build/launcher"
    library = directory / f"rmg-silent-audio-{digest}.so"
    if not library.is_file():
        directory.mkdir(parents=True, exist_ok=True)
        temporary = directory / f".{library.name}-{uuid.uuid4().hex}"
        try:
            subprocess.run(["cc", "-shared", "-fPIC", "-O2", "-Wall", "-Wextra", "-Werror",
                            str(source), "-o", str(temporary), "-ldl"], check=True)
            temporary.replace(library)
        finally:
            temporary.unlink(missing_ok=True)
    return library
