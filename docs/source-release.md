# Preparing a source release

Use `tools/export_source.py` to package the current source. It reads tracked and
non-ignored untracked files without changing staging or Git history. Use its
filtered archive rather than exporting the full development history, which
contains original ROM assets.

## Package contents

Include game/engine source, required asset inputs and generators, build tools,
tests, human documentation, `credits.md`, `licence.md` and `LICENSES/`.
[Asset sources](asset-sources.md) and [dependency provenance](build-dependencies.md)
are part of the release, including inherited notices and documented exceptions.

The package retains the hash-pinned inherited HackerSM64 SDK/runtime prerequisites
and GNU 2.30 linker. `tools/build-dependencies.json` identifies the bundled files;
`tools/source-release-policy.json` records the inclusion decision. Do not replace
those bytes or discard their notices during packaging.

ROMs, saves, builds, unchanged extracted assets, reconstructed numeric/audio
outputs, captures and local settings are excluded. Agent procedures and history
under `llm/` are excluded by default; `--optional-llm` adds that optional material.
The ordinary human guides do not depend on it.

## Reconstructing the game

Supply the three owned baseroms and compiler described in [building](building.md).
`extract_assets.py` uses the ROM recipe registries in `assets/rom/` to regenerate
numeric initializers and edited audio. Initializer output lives in
`assets/rom/generated/`; model/animation source retains surrounding code and
references those generated includes.

The inherited prerequisites are already bundled. `tools/import_build_dependencies.py`
can repair missing files from a matching local tree after checking their pinned
hashes; it is not a downloader or a required step for an intact package.

## Standard re-packaging workflow

1. Compare against the inventory from the last successfully created package.
   Include added/changed/deleted files, modes, symlinks, dependency pins and policy.
2. Review the changes and run the affected checks. Reuse earlier results only
   for unchanged inputs. Packaging by itself does not require a ROM build.
3. Keep previous archives and inventories. Write a new report and archive outside
   the checkout, using a new version or date.
4. Confirm the final inventory matches what was reviewed. Record the baseline
   inventory hash, relevant checks, archive checksum and applicable build evidence.
5. Review and publish the resulting package separately. The exporter does not
   stage, commit, tag or publish it.

Create an inventory comparison without writing an archive:

```sh
python3 tools/export_source.py --root . \
  --policy tools/source-release-policy.json \
  --previous-inventory /path/to/previous-inventory.json \
  --report /path/to/review-inventory.json
```

After review, create the package:

```sh
python3 tools/export_source.py --root . \
  --policy tools/source-release-policy.json \
  --previous-inventory /path/to/previous-inventory.json \
  --report /path/to/source-inventory.json \
  --output /path/to/Devin-Movement-Rewrite-source.tar.gz
```

For the first package only, use `--initial-package` instead of a previous
inventory. The exporter refuses to overwrite that baseline inventory. Its report
and embedded manifest identify the selected source, exclusions and changes.

## Verification boundaries

Verify the archive's hashes and exclusions. When a clean build is performed,
record its source/package identity, compiler/settings and ROM checksum. An older
ROM checksum does not verify later game-source changes. Syntax checks using
placeholder ROM inputs do not establish asset correctness; host tests do not
establish emulator or console behavior.

Eligible project code and assets use the grants in [licence.md](../licence.md).
Inherited/imported material retains its original notices and exceptions.
The exporter reports unresolved inputs/provenance according to the release
policy; credits and source attribution must remain in the package.
