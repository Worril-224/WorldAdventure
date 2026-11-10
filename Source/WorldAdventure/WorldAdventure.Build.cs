using UnrealBuildTool;

public class WorldAdventure : ModuleRules
{
    public WorldAdventure(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.Add("Core");
    }
}
