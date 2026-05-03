# UI Module

Owns C++ UI view models, Canvas HUD presentation, and future widget-facing data structures.

## Current Implementation

- `ARiftCombatHUD` draws the current prototype HUD through Canvas.
- The HUD includes run state, room progress, combat stats, reward/supply cards, title menu, and victory/defeat panels.
- The title menu is a keyboard-driven scaffold with `Start Demo`, `Host Game`, `Join Game`, `Settings`, and `Quit`.

## Next Step

Move the Canvas title menu into UMG once the host/join/settings flows are stable enough to bind to real buttons.
