using UnrealBuildTool;
using System.Collections.Generic;

public class WorldAdventureTarget : TargetRules
{
    public WorldAdventureTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.AddRange(new[] { "WACore", "WAAbilities", "WAWorld", "WAPlayer", "WAGame", "WAUI" });
    }
}
