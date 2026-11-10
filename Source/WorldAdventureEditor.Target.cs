using UnrealBuildTool;
using System.Collections.Generic;

public class WorldAdventureEditorTarget : TargetRules
{
    public WorldAdventureEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.AddRange(new[] { "WorldAdventure", "WACore", "WAAbilities", "WAWorld", "WAPlayer", "WAGame", "WAUI", "WAEditor" });
    }
}
