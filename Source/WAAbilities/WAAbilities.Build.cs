using UnrealBuildTool;

public class WAAbilities : ModuleRules
{
    public WAAbilities(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "GameplayAbilities", "GameplayTags", "GameplayTasks", "NetCore", "WACore" });
    }
}
