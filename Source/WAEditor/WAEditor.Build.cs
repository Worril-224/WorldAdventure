using UnrealBuildTool;

public class WAEditor : ModuleRules
{
    public WAEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "UnrealEd", "WACore", "WAAbilities", "WAWorld", "WAPlayer", "WAGame", "WAUI" });
        PrivateDependencyModuleNames.Add("AssetTools");
    }
}
