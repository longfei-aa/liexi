using UnrealBuildTool;
using System.Collections.Generic;

public class RiftSquadTarget : TargetRules
{
    public RiftSquadTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("RiftSquad");
    }
}
