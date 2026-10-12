#!/usr/bin/env python3
"""Install the optional quality hook without replacing an existing hook or hooksPath."""

from pathlib import Path
import subprocess
import sys


def git(root, *arguments):
    result = subprocess.run(["git", *arguments], cwd=root, capture_output=True, text=True, check=True)
    return result.stdout.strip()


def main():
    try:
        root = Path(git(Path.cwd(), "rev-parse", "--show-toplevel"))
        configured = subprocess.run(["git", "config", "--get", "core.hooksPath"], cwd=root,
                                    capture_output=True, text=True, check=False)
        if configured.returncode == 0:
            print("Existing core.hooksPath preserved; "
                  "integrate tools/hooks/pre-commit there explicitly.",
                  file=sys.stderr)
            return 2
        if configured.returncode != 1:
            raise OSError("could not read hook configuration")
        source = root / "tools/hooks/pre-commit"
        target = Path(git(root, "rev-parse", "--git-path", "hooks/pre-commit"))
        if not target.is_absolute():
            target = root / target
        content = source.read_bytes()
        if target.exists() or target.is_symlink():
            if (not target.is_symlink() and target.read_bytes() == content
                    and target.stat().st_mode & 0o111):
                print("Quality hook already installed.")
                return 0
            print("Existing pre-commit hook preserved; integrate the quality command explicitly.",
                  file=sys.stderr)
            return 2
        target.parent.mkdir(parents=True, exist_ok=True)
        with target.open("xb") as output:
            output.write(content)
        target.chmod(0o755)
        print("Installed staged quality checks in", target)
        return 0
    except (OSError, subprocess.CalledProcessError) as error:
        print("Hook installation unavailable:", error, file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
