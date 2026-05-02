# SoftUEBridge For Rift Squad

This plugin copy comes from `soft-ue-cli` Python package `1.26.0`, bundled plugin `SoftUEBridge` `1.3.3`.

Repository reference:

- https://github.com/softdaddy-o/soft-ue-cli

## Local Compatibility Target

- Project: `RiftSquad`
- Unreal Engine: `5.6.1`
- Platform verified: macOS, Xcode 16.4

## Rift Squad Compatibility Patches

The upstream plugin was copied into the project and patched locally so it can compile against UE `5.6.1` on macOS:

- `BridgeServer.cpp`
  - Replaced ambiguous empty `TEXT("")` HTTP response bodies with explicit `FString()`.
- `CompileMaterialTool.cpp`
  - Replaced `GMaxRHIShaderPlatform` material-resource lookup with `GMaxRHIFeatureLevel`.
  - Avoided a UE 5.6 format-string sanitizer issue by storing compile status before `UE_LOG`.
- `RewindHelper.cpp`
  - Replaced unavailable `IRewindDebugger::GetDebuggedObjects()` with UE 5.6's `GetDebugComponents()`.
  - Removed direct calls to unavailable `CanStartRecording()` / `StartRecording()` and uses trace console fallback.
- `SoftUEBridgeEditor.Build.cs`
  - Removed the `LiveCoding` module dependency. The tool already returns "Windows only" on macOS, and installed UE 5.6.1 for Mac does not provide a linkable `UnrealEditor-LiveCoding.dylib`.
- `SoftUEBridge.uplugin`
  - Declared `EnhancedInput` as a plugin dependency because `SoftUEBridgeEditor` depends on the `EnhancedInput` module.

## Verified Commands

Run from the project root while Unreal Editor is open with SoftUEBridge enabled:

```zsh
/Users/shelongfei/.local/bin/soft-ue-cli check-setup
/Users/shelongfei/.local/bin/soft-ue-cli status
/Users/shelongfei/.local/bin/soft-ue-cli project-info
/Users/shelongfei/.local/bin/soft-ue-cli query-level --limit 20
/Users/shelongfei/.local/bin/soft-ue-cli query-asset --asset-path /Game/RiftSquad/Input/IA_Move
/Users/shelongfei/.local/bin/soft-ue-cli validate-class-path /Script/RiftSquad.RiftRoomManager
/Users/shelongfei/.local/bin/soft-ue-cli run-python-script --script "import unreal; print(unreal.SystemLibrary.get_engine_version())"
```

## Known Limitation

`pie-session start` can launch PIE in a `-nullrhi` editor session, but `pie-tick` / `pie-session get-state` timed out afterward in the current macOS headless test. Treat PIE control as needing validation in a normal GUI editor session before relying on it in the workflow.

Do not commit `Binaries/`, `Intermediate/`, `.soft-ue-bridge/`, or other generated files.
