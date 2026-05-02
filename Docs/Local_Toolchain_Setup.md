# Local Toolchain Setup

## Current Status

Checked on 2026-05-02:

- Homebrew exists at `/opt/homebrew/bin/brew`.
- Epic Games Launcher is installed at `/Applications/Epic Games Launcher.app`.
- `mas` is installed, but App Store CLI access is unreliable in the current CN store region.
- Full Xcode is not installed at `/Applications/Xcode.app`.
- `xcode-select -p` currently points to `/Library/Developer/CommandLineTools`.
- `UnrealEditor.app` was not found under `/Applications` or `/Users/Shared/Epic Games`.

## Required Installs

Install these before C++ compilation and PIE testing:

- Xcode from the Mac App Store.
- Unreal Engine 5.6 from Epic Games Launcher.

The project currently targets UE 5.6 in `RiftSquad.uproject`.

## Xcode Setup After Install

Run:

```bash
sudo xcode-select -s /Applications/Xcode.app/Contents/Developer
sudo xcodebuild -license accept
xcodebuild -version
```

Expected:

```text
Xcode <version>
Build version <build>
```

## Unreal Setup After Install

Expected editor path:

```text
/Users/Shared/Epic Games/UE_5.6/Engine/Binaries/Mac/UnrealEditor.app
```

Verify:

```bash
find /Users/Shared/Epic\ Games -maxdepth 4 -name UnrealEditor.app
```

Then open:

```bash
open RiftSquad.uproject
```

## First Unreal Tasks

After the editor opens:

- Let Unreal generate project files if prompted.
- Build `RiftSquadEditor`.
- Create `Content/RiftSquad/Maps/M_Greybox_CombatRoom.umap`.
- Create Enhanced Input assets:
  - `Content/RiftSquad/Input/IA_Move`
  - `Content/RiftSquad/Input/IA_Fire`
  - `Content/RiftSquad/Input/IMC_Player`
- Create Blueprint subclasses:
  - `BP_RiftPlayerCharacter` from `ARiftPlayerCharacter`
  - `BP_ChaserEnemy` from `ARiftEnemyBase`
  - `BP_RiftRoomManager` from `ARiftRoomManager`
- Place `BP_RiftRoomManager` in the greybox map.
- Set its `EnemyClass` to `BP_ChaserEnemy`.

## Validation Commands

Run these after installation:

```bash
xcodebuild -version
find /Users/Shared/Epic\ Games -maxdepth 4 -name UnrealEditor.app
python3 -m json.tool RiftSquad.uproject
git status --short --branch
```
