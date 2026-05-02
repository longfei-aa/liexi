# Rift Squad Demo Playbook

This file describes how to present the current local prototype.

## Demo Goal

Show a short, understandable combat loop:

1. Spawn into the greybox arena.
2. Move, aim, and shoot enemies.
3. Clear a room.
4. Choose one of three rewards with `1 / 2 / 3`.
5. Continue through five rooms.
6. Win after the boss room or lose if the squad is wiped.

## How To Run

```bash
open RiftSquad.uproject
```

In Unreal Editor:

1. Open `/Game/RiftSquad/Maps/M_Greybox_CombatRoom`.
2. Click `Play`.
3. Use `WASD` to move.
4. Aim with the mouse.
5. Fire with left mouse button.
6. Choose rewards with `1`, `2`, or `3`.

Use `Selected Viewport` or `New Editor Window`. Do not use `Simulate` for the demo.

## What Is Ready To Show

- Top-down combat camera.
- Runtime arena fallback with floor, walls, and lighting.
- Player cylinder, enemy cones, projectile spheres.
- Five-room combat route with final boss room.
- Combat HUD with status, room progress, enemy count, HP, weapon stats, controls, objectives, rewards, victory, and defeat.
- Server-authoritative reward selection path ready for later multiplayer testing.

## Current Placeholder Art

The demo intentionally uses Unreal Engine built-in basic shapes. This keeps the prototype reproducible without downloading external binary assets.

Do not treat the current visuals as final art direction. The target direction remains: readable low-poly sci-fi rift arena.

## Approved Asset Direction

Use these sources only after recording every imported asset in `Content/RiftSquad/ThirdPartyAssets/SOURCE.md`:

- Kenney assets for CC0 UI prompts, icons, low-poly props.
- OpenGameArt for CC0/attributed SFX and icons.
- Unreal Engine Marketplace free assets only when license and redistribution constraints are clear.

## Next Visual Pass

Recommended next tasks:

1. Replace cylinder/cone/sphere placeholders with low-poly character, enemy, and projectile assets.
2. Add hit feedback: flash, scale pulse, or short screen shake.
3. Add simple SFX for fire, hit, reward, victory, defeat.
4. Convert the Canvas HUD into UMG once the display information is stable.
5. Add a title/menu screen before entering the combat map.
