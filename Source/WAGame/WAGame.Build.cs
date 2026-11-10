using UnrealBuildTool;

public class WAGame : ModuleRules
{
    public WAGame(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "GameplayAbilities", "GameplayTags", "GameplayTasks", "NetCore", "WACore", "WAAbilities", "WAWorld", "WAPlayer" });
    }
}
