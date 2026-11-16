# GC Core Dev

Shared systems, presentation and mod-pack behavior for Global Conflicts.
Core is not the full mod collection. It loads without GC Units, GC Missions,
GC Lobby, terrain packs, vehicle packs, or Realistic Combat Drones.

## Direct requirements

Alongside Arma Reforger, Core requires only the systems its own changes use:

- ACE Core Dev: settings infrastructure and the metal-clanging command extension.
- ACE Captives Dev: captive/surrender options in the entity modifier.
- ACE Finger Dev: pointing, ping and editor settings.
- ACE Medical Core Dev: shared medical settings.
- ACE Overheating Dev: the clear-jam input binding.
- CSI: the compass presentation extension.

Equipment, weapon, mortar, vehicle and unit overrides now belong to GC Units.
Worlds, mission headers and terrain-specific data belong to GC Missions.
Moved resources retain their GUIDs, so saved references continue to identify them
when the owning addon is loaded. Update all three Dev checkouts together.

Use GC Workshop to load/download the full collection. Workshop dependencies are
updated through the game's native dependency actions; Core runs no updater.
This branch uses the Dev identity `623555110E2B2CA1`; published release packages
are separate and are not changed by editing this checkout.
