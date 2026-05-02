# Local Toolchain Setup

## Current Status

Checked on 2026-05-02:

- Homebrew exists at `/opt/homebrew/bin/brew`.
- Xcode is installed at `/Applications/Xcode.app`.
- `xcode-select -p` points to `/Applications/Xcode.app/Contents/Developer`.
- `xcodebuild -version` returns `Xcode 26.4.1`.
- `mas` is installed, but App Store CLI access is unreliable in the current CN store region.
- Epic Games Launcher was installed once through Homebrew, but it is not currently present at `/Applications/Epic Games Launcher.app`.
- `UnrealEditor.app` was not found under `/Applications` or `/Users/Shared/Epic Games`.
- Git global proxy points to `http://127.0.0.1:7897`, but that port was not listening during the last check.

## Required Installs

Install these before C++ compilation and PIE testing:

- Unreal Engine 5.6 from Epic Games Launcher.

The project currently targets UE 5.6 in `RiftSquad.uproject`.

## Unreal Install Options On macOS

### Option A: Epic Games Launcher

This is the easiest route for a prebuilt Unreal Editor.

Install or reinstall:

```bash
brew install --cask epic-games
open -a "Epic Games Launcher"
```

Then log in and install Unreal Engine 5.6 from the Library tab.

Expected editor path:

```text
/Users/Shared/Epic Games/UE_5.6/Engine/Binaries/Mac/UnrealEditor.app
```

### Option B: Build Unreal Engine From GitHub Source

Use this when Epic Games Launcher cannot be opened.

Requirements:

- Epic account linked to GitHub.
- GitHub account has accepted the EpicGames organization invite.
- Working network or proxy access to GitHub.
- At least 150 GB of free disk space is recommended for source, dependencies, intermediates, and build artifacts.

Commands:

```bash
git clone --branch 5.6 --single-branch https://github.com/EpicGames/UnrealEngine.git ~/UnrealEngine-5.6
cd ~/UnrealEngine-5.6
./Setup.command
./GenerateProjectFiles.command
open UE5.xcworkspace
```

In Xcode:

- Select the `UnrealEditor` scheme.
- Select `My Mac`.
- Build `Development Editor`.

After build, the editor binary should be under:

```text
~/UnrealEngine-5.6/Engine/Binaries/Mac/UnrealEditor.app
```

Then open this project:

```bash
open -a ~/UnrealEngine-5.6/Engine/Binaries/Mac/UnrealEditor.app RiftSquad.uproject
```

### Option C: Copy A Prebuilt Engine From Another Mac

If another Mac already has UE 5.6 installed through Epic Games Launcher, copying the whole engine folder can work for local development:

```text
/Users/Shared/Epic Games/UE_5.6
```

Copy it to the same path on this machine, then verify:

```bash
find /Users/Shared/Epic\ Games -maxdepth 4 -name UnrealEditor.app
```

This is less clean than installing through Launcher, but it avoids building the full engine from source.

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

If using a source build instead:

```bash
find ~/UnrealEngine-5.6 -maxdepth 5 -name UnrealEditor.app
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
