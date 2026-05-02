# Local Toolchain Setup

## Current Status

Checked on 2026-05-02:

- Homebrew exists at `/opt/homebrew/bin/brew`.
- Xcode 16.4 is installed at `/Applications/Xcode-16.4.0.app`.
- Xcode 26.4.1 is installed at `/Applications/Xcode.app`, but it is not selected for this project.
- `xcode-select -p` points to `/Applications/Xcode-16.4.0.app/Contents/Developer`.
- `xcodebuild -version` returns `Xcode 16.4`.
- `xcrun --sdk macosx --show-sdk-version` returns `15.5`.
- Unreal Engine 5.6 rejects Xcode 26.4.1 for Mac builds because UE 5.6 accepts Apple SDK versions from `15.2.0` to `16.9.0`.
- `mas` is installed, but App Store CLI access is unreliable in the current CN store region.
- Epic Games Launcher is installed at `/Applications/Epic Games Launcher.app`.
- Unreal Engine 5.6 is installed at `/Users/Shared/Epic Games/UE_5.6`.
- `UnrealEditor.app` exists at `/Users/Shared/Epic Games/UE_5.6/Engine/Binaries/Mac/UnrealEditor.app`.
- `RiftSquadEditor Mac Development` compiled successfully with Xcode 16.4.
- Git global proxy points to `http://127.0.0.1:7897`, but that port was not listening during the last check.

## Required Installs

Install these before C++ compilation and PIE testing:

- Unreal Engine 5.6 from Epic Games Launcher.
- Xcode 16.4 or another Xcode version in UE 5.6's accepted range.

The project currently targets UE 5.6 in `RiftSquad.uproject`.

## Xcode Compatibility For UE 5.6

UE 5.6's installed SDK configuration reports:

```text
MinRequired=15.2.0
MaxRequired=16.9.0
```

If `xcodebuild -version` reports Xcode `26.x`, UnrealBuildTool will fail with:

```text
Unable to find valid SDK(s) for Mac:
Found Sdk Version=26.4.1, MinRequired=15.2.0, MaxRequired=16.9.0.
Platform Mac is not a valid platform to build.
```

Install Xcode 16.4 alongside the current Xcode:

```bash
brew install xcodes
xcodes install 16.4 --select --experimental-unxip
```

The `xcodes install` command requires Apple authentication. If command-line authentication fails, download Xcode 16.4 manually from Apple Developer Downloads, then install from the local `.xip`:

```bash
xcodes install 16.4 --path /path/to/Xcode_16.4.xip --select --experimental-unxip
```

After installation, verify:

```bash
xcodebuild -version
xcrun --sdk macosx --show-sdk-version
```

Expected:

```text
Xcode 16.4
<macOS SDK version accepted by UE 5.6>
```

## Unreal Install Options On macOS

### Option A: Epic Games Launcher

This is the easiest route for a prebuilt Unreal Editor.

Install or reinstall:

```bash
brew install --cask epic-games
open -a "Epic Games Launcher"
```

Then log in and install Unreal Engine 5.6 from the Library tab.

If Homebrew says the cask is installed but `open -a "Epic Games Launcher"` fails, clean the broken cask state and reinstall:

```bash
brew uninstall --cask --force epic-games
brew install --cask epic-games
open -a "Epic Games Launcher"
```

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
