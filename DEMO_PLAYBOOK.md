# Rift Squad Demo Playbook

This file describes how to present the current local prototype.

## Demo Goal

Show a short, understandable combat loop:

1. Spawn into the greybox arena.
2. Use the title menu to choose `Start Demo`.
3. Move, aim, and shoot enemies.
4. Clear a room.
5. Choose one of three rewards with `1 / 2 / 3`.
6. After the elite room, choose one supply upgrade before entering the boss.
7. Continue through five combat rooms.
8. Win after the boss room or lose if the squad is wiped.
9. Press `Enter` on victory/defeat to replay the demo loop.

## How To Run

```bash
open RiftSquad.uproject
```

In Unreal Editor:

1. Open `/Game/RiftSquad/Maps/M_Greybox_CombatRoom`.
2. Click `Play`.
3. Use `Up / Down` on the title menu.
4. Press `Enter` on `Start Demo`.
5. Use `WASD` to move.
6. Aim with the mouse.
7. Fire with left mouse button.
8. Use `Q` or right mouse button for shockwave.
9. Use `Space` to dash.
10. Choose rewards with `1`, `2`, or `3`.
11. Choose the supply option after the elite room with `1`, `2`, or `3`.
12. Press `Enter` on victory/defeat to restart the run.

Use `Selected Viewport` or `New Editor Window`. Do not use `Simulate` for the demo.

## What Is Ready To Show

- Top-down combat camera.
- Runtime arena fallback with floor, walls, and lighting.
- Keyboard title menu before the run starts: Start Demo, Host Game placeholder, Join Game placeholder, Settings placeholder, Quit.
- Player cylinder, enemy cones, projectile spheres.
- Five-room combat route with final boss room.
- Supply node after the elite room, using recovery/mobility/cooldown options before the boss.
- Enemy variety: chaser, shooter, burster, and elite placeholder variants.
- Placeholder combat feedback: enemies pulse on hit and flatten briefly on death.
- Combat HUD with status, room progress, enemy count, HP, weapon stats, controls, objectives, rewards, victory, and defeat.
- Replay loop from victory/defeat using `Enter`, resetting player health, movement, weapon, ability, and item state.
- Assault-style active kit: basic fire, shockwave, and dash.
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
2. Add player-side feedback: short screen shake, damage vignette, or controller rumble.
3. Add simple SFX for fire, hit, reward, victory, defeat.
4. Convert the Canvas HUD into UMG once the display information is stable.
5. Convert the keyboard title menu into a UMG main menu with real host/join/settings buttons.
