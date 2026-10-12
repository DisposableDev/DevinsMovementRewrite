#!/usr/bin/env python3
"""Append inverse-stick poses without changing the original dab animation tracks."""

import argparse
from pathlib import Path
import re

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source

ROOT = Path(__file__).resolve().parents[1]
ASSET = ROOT / "assets/anims/anim_D1.inc.c"
ORIGINAL_FRAMES = 52
ANALOG_FRAMES = 64
FULL_POSE = 7
RELAXED_POSE = 2


def read_array(text, name):
    match = re.search(r"static const \w+ " + name + r"\[\] = \{(.*?)\};", text, re.S)
    if match is None:
        raise ValueError(f"Missing animation array: {name}")
    body = re.sub(r"//[^\n]*", "", match[1])
    values = [int(token, 0) for token in re.findall(r"-?(?:0x[0-9a-fA-F]+|\d+)", body)]
    return match, values


def inverse_track(track):
    samples = []
    for step in range(ANALOG_FRAMES):
        position = FULL_POSE - (FULL_POSE - RELAXED_POSE) * step / (ANALOG_FRAMES - 1)
        frame = int(position)
        fraction = position - frame
        first = track[frame]
        second = track[min(frame + 1, FULL_POSE)]
        # Joint rotations are signed 16-bit angles; cross their seam by the short arc.
        delta = (second - first + 32768) % 65536 - 32768
        value = round(first + delta * fraction)
        samples.append((value + 32768) % 65536 - 32768)
    return samples


def render_asset(text):
    index_match, indices = read_array(text, "anim_D1_indices")
    _, values = read_array(text, "anim_D1_values")
    if len(indices) != 126:
        raise ValueError("Expected the shared 20-bone Mario skeleton")
    labels = re.findall(r"// ([^\n]+)", index_match[1])
    if len(labels) != 21:
        raise ValueError("Expected one label for each animation node")
    tracks = []
    updated_indices = []
    offset = 0
    for channel in range(63):
        count, start = indices[channel * 2:channel * 2 + 2]
        if count not in (1, ORIGINAL_FRAMES, ORIGINAL_FRAMES + ANALOG_FRAMES):
            raise ValueError(f"Unexpected frame count in channel {channel}: {count}")
        if start + count > len(values):
            raise ValueError(f"Channel {channel} exceeds the animation data")
        track = values[start:start + min(count, ORIGINAL_FRAMES)]
        if count != 1:
            track += inverse_track(track)
        updated_indices.extend((len(track), offset))
        tracks.append(track)
        offset += len(track)

    header = text[:index_match.start()]
    header = re.sub(r"\n    (?:52|116),\n    ANIMINDEX_NUMPARTS", "\n    116,\n    ANIMINDEX_NUMPARTS", header)
    comment = "// 52..115: inverse-stick poses, full dab (frame 7) through wind-up (frame 2).\n"
    if comment not in header:
        header = header.replace("// Frames 7, 8 and 20", comment + "// Frames 7, 8 and 20")
    output = [header, "static const u16 anim_D1_indices[] = {\n"]
    for node, label in enumerate(labels):
        entries = updated_indices[node * 6:node * 6 + 6]
        output.append("    " + ", ".join(map(str, entries)) + ", // " + label + "\n")
    output.append("};\n\nstatic const s16 anim_D1_values[] = {\n")
    for node, label in enumerate(labels):
        output.append("    // " + label + "\n")
        for track in tracks[node * 3:node * 3 + 3]:
            output.extend(f"    {value},\n" for value in track)
    output.append("};\n")
    return "".join(output)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="verify generated poses without writing")
    arguments = parser.parse_args()
    current = read_source(ASSET)
    expected = render_asset(current)
    if arguments.check:
        if current != expected:
            print("FAIL: inverse dab poses need regeneration")
            return 1
        print("PASS: inverse dab poses match the original wind-up tracks")
    else:
        ASSET.write_text(expected)
        print("Generated 64 inverse dab poses; original frames 0..51 preserved")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
