# Rift Squad CLI Scripts

Run this from the project root after C++ compilation to regenerate the current greybox map and starter input assets:

```zsh
'/Users/Shared/Epic Games/UE_5.6/Engine/Binaries/Mac/UnrealEditor.app/Contents/MacOS/UnrealEditor' \
  '/Users/shelongfei/Documents/game/RiftSquad.uproject' \
  -ExecutePythonScript='/Users/shelongfei/Documents/game/Scripts/ue_init_riftsquad_content.py' \
  -unattended -nop4 -nosplash -nullrhi -stdout -FullStdOutLogOutput
```

The map stays intentionally light: static greybox geometry plus `PlayerStart`. Runtime combat actors are spawned from C++ by `ARiftGameMode` to keep command-line asset generation reliable on macOS `-nullrhi`.
