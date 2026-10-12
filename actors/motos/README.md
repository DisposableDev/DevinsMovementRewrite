# Motos assets

Imported from the user-supplied `motos_full.patch`, sourced from
https://www.youtube.com/watch?v=Aud0uFGDOIs .

Patch SHA-256: `9bcfa7425fe53ee07d94b59b3724c390ee2e071153d6f7f0e408dc14e99837b7`.

The mesh vertices, triangles, textures, animation samples and joint indices are
preserved. The animation pointer table is const-corrected. The geometry layout
uses native project macros, omits the enemy held-Mario callback and shadow, and
routes every part through the transparent layer. Material alpha now uses the
NPC's environment opacity while retaining texture cutouts. Lighting uses native
light-color commands with the patch's diffuse and ambient colors; a final display list
restores shared alpha and environment state.

These assets are included directly in JRB's level segment for its SimpleFlips
NPC. The patch's enemy behavior, Lethal Lava Land replacements, coin changes,
actor-group changes and memory configuration changes are not imported.
