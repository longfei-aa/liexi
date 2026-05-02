using UnrealBuildTool;
using System.Collections.Generic;

public class RiftSquadEditorTarget : TargetRules
{
    public RiftSquadEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("RiftSquad");
    }
}
