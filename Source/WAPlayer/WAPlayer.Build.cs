using UnrealBuildTool;

public class WAPlayer : ModuleRules
{
    public WAPlayer(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "EnhancedInput", "GameplayAbilities", "GameplayTags", "GameplayTasks", "InputCore", "NetCore", "WACore", "WAAbilities" });
    }
}
