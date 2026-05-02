# Milestone 0/1 Validation

## Current Toolchain Status

This repository currently contains a hand-authored UE5 C++ project skeleton.
The local machine did not expose `UnrealEditor.app`, and `xcodebuild` reported Command Line Tools instead of full Xcode, so UE compilation and PIE validation were not run in this pass.

## Milestone 0: Project Skeleton

File-level checks without Unreal Editor:

- `RiftSquad.uproject` exists.
- `Source/RiftSquad.Target.cs` exists.
- `Source/RiftSquadEditor.Target.cs` exists.
- `Source/RiftSquad/RiftSquad.Build.cs` exists.
- `Source/RiftSquad/RiftSquad.cpp` implements the primary game module.
- `Config/DefaultEngine.ini` points to `RiftGameMode`.
- `Content/RiftSquad` contains the planned asset directories.

Editor checks when UE5.5/5.6 is installed:

- Open `RiftSquad.uproject`.
- Let Unreal generate project files if prompted.
- Build `RiftSquadEditor`.
- Create `Content/RiftSquad/Maps/M_Greybox_CombatRoom.umap`.
- Create Enhanced Input assets `IMC_Player`, `IA_Move`, and `IA_Fire`.
- Create `BP_RiftPlayerCharacter` from `ARiftPlayerCharacter`.
- Launch PIE as a single player.

## Milestone 1: Single Room Combat

File-level checks without Unreal Editor:

- `URiftHealthComponent` exists and replicates health/death state.
- `URiftWeaponComponent` exists and routes firing through a server RPC.
- `ARiftProjectile` exists and applies damage only on authority.
- `ARiftEnemyBase` exists and applies chase/attack logic only on authority.
- `ARiftRoomManager` exists and replicates room phase and alive enemy count.
- `ARiftGameState` mirrors room phase and alive enemy count.

Editor checks when UE5.5/5.6 is installed:

- Place `BP_RiftRoomManager` in `M_Greybox_CombatRoom`.
- Set `EnemyClass` on the room manager to `BP_ChaserEnemy`.
- Set `ProjectileClass` on the player's `RiftWeaponComponent` if using a Blueprint projectile subclass.
- Confirm player can move and aim.
- Confirm player can fire a projectile.
- Confirm projectile damages and kills enemies.
- Confirm enemies chase and damage the player.
- Confirm room phase becomes `Completed` after all enemies die.

## PIE Network Smoke Test For Later

- `Number of Players = 2`.
- `Net Mode = Play As Listen Server`.
- Confirm host and client both see enemy deaths.
- Confirm host and client both see alive enemy count reach 0.
- Confirm room phase reaches `Completed` on both machines.
