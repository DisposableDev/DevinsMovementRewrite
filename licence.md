# Licensing

This licence offer, declared on 2026-10-12, covers eligible project-controlled
code and custom assets in the development tree and historical revisions of this
fork from upstream base `8953c07aa4592d65ffc974b596c97ae42778a221` onward. It includes
eligible contributions in commits made before the declaration date; that date
is not a cutoff based on when the material was created or committed.

The project copyright holders grant only rights they own or are authorized to
license, where allowed by the material's existing terms. The historical coverage
does not relicense the inherited upstream base or another creator's portions.
Existing copyright, licence and required notices remain in effect. This records
the scope of the current offer; it does not assert that the offer was published
at the time an earlier commit was made, or rewrite earlier commits.

## Code: GPLv3 or later

Eligible project code and project-controlled code contributions are licensed
under the **GNU General Public License, version 3 or, at your option, any later
version** (`GPL-3.0-or-later`). This includes eligible game changes, build and
asset-generation scripts, development tools and tests.

This covered code is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

This covered code is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

A complete copy is in [LICENSES/GPL-3.0-or-later.txt](LICENSES/GPL-3.0-or-later.txt).
The [GNU licence page](https://www.gnu.org/licenses/gpl-3.0.html) is the canonical
reference. Pre-existing third-party portions retain their own terms; the project
grant covers only the rights the project holders can grant, consistently with
those terms. A mixed or linked program is not declared GPL-compliant merely by
including this notice.

## Custom assets: CC BY-SA 4.0

Eligible original custom assets and project-controlled artistic contributions
are licensed under **Creative Commons Attribution-ShareAlike 4.0 International**
(`CC-BY-SA-4.0`). This covers eligible textures, models, level geometry,
animation data, music and recordings, including their editable source forms.

Attribute the creators identified in the asset's notices and source records,
retain supplied credits and source links, indicate your changes, and follow the
licence's ShareAlike terms. Project contributions without a more specific
recorded attribution may be credited to the **Devin's Movement Rewrite project**.
This does not replace named third-party creator credits.

A complete copy is in [LICENSES/CC-BY-SA-4.0.txt](LICENSES/CC-BY-SA-4.0.txt), with
the [canonical legal code](https://creativecommons.org/licenses/by-sa/4.0/legalcode.en)
available from Creative Commons. The licence applies only to rights the project
holders can grant; it grants no Nintendo or other third-party rights by implication.

## Scope and retained third-party terms

Classify material by its purpose and provenance, not just its filename extension.
For example, an original mesh stored in a C initializer is asset data, while
the script that generates it is code. An asset does not acquire the generator's
licence simply because that generator produced it.

| Material | Licensing scope |
| --- | --- |
| Original project code and authorized local code contributions | GPL-3.0-or-later, to the extent allowed by existing terms. |
| Original project custom assets and authorized artistic contributions | CC-BY-SA-4.0, to the extent allowed by underlying rights and existing terms. |
| Pre-existing SM64/HackerSM64 code and assets | Existing rights and notices remain; no new blanket grant is made for inherited portions. |
| Nintendo ROM assets and reconstructed or adapted Nintendo models, textures, animations and audio | Underlying Nintendo material is outside these grants. Only independently licensable project contributions can receive the applicable project licence. |
| SDK headers, libraries, microcode, runtime archives and SDK audio-tool sources | Retain their existing notices and terms. Inclusion as a HackerSM64 dependency does not relicense them. See [dependency provenance](docs/build-dependencies.md). |
| Imported Motos content, YouTube audio clips, Mii Channel/Evangelion music, Paper Mario artwork and Render96-derived artwork | Retain source-specific terms and credits. They are not automatically licensed CC BY-SA 4.0. See [asset sources](docs/asset-sources.md). |
| Third-party tools and fonts with MIT, LGPL, GPL or other notices | Preserve those licences and their requirements, including any version restrictions. The ProFont-derived crash font retains its [MIT notice](textures/crash_custom/LICENSE). |
| Material with unresolved ownership or licensing | No additional permission is asserted for the unresolved third-party rights. |

Existing copyright and author notices remain authoritative for their holders.
Keep [credits.md](credits.md), [asset-source records](docs/asset-sources.md),
file-level notices and any associated licence files with redistributed material.
These declarations do not create copyright in material that is not protected.
