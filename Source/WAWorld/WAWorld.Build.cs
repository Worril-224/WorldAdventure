using UnrealBuildTool;

public class WAWorld : ModuleRules
{
    public WAWorld(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "NetCore", "WACore" });
    }
}
