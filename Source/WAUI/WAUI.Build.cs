using UnrealBuildTool;

public class WAUI : ModuleRules
{
    public WAUI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "GameplayTags", "Slate", "SlateCore", "UMG", "WACore", "WAGame" });
    }
}
