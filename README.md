# GC Core Dev

Shared systems, presentation and mod-pack behavior for Global Conflicts.
Core is not the full mod collection. It loads without GC Units, GC Missions,
GC Lobby, terrain packs, vehicle packs, or Realistic Combat Drones.

## Direct requirements

Alongside Arma Reforger, Core requires:

- **ACE All in One Dev** (`65AD7BE594439A52`), the official collection of ACE Dev
  modules. It includes ACE Core Dev and ACE Medical Core Dev
  (`6586079789278413`), along with pointing, captives, overheating and the other
  ACE systems. We use this single umbrella requirement rather than maintaining
  individual ACE dependency entries across GC projects.
- **CSI**, for the compass presentation extension.

ACE's collection owns the exact module list; GC inherits its Dev dependencies.
This intentionally loads the full ACE Dev suite, including modules beyond the
specific ACE features Core modifies.

Equipment, weapon, mortar, vehicle and unit overrides now belong to GC Units.
Worlds, mission headers and terrain-specific data belong to GC Missions.
Moved resources retain their GUIDs, so saved references continue to identify them
when the owning addon is loaded. Update all three Dev checkouts together.

Use GC Workshop to load/download the full collection. Workshop dependencies are
updated through the game's native dependency actions; Core runs no updater.
This branch uses the Dev identity `623555110E2B2CA1`; published release packages
are separate and are not changed by editing this checkout.
