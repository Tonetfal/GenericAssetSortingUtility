// Author: Antonio Sidenko (Tonetfal), May 2026

using UnrealBuildTool;

public class AssetsSorting : ModuleRules
{
	public AssetsSorting(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"AssetRegistry",
			"Blutility",
			"DeveloperSettings",
			"EditorScriptingUtilities",
			"UnrealEd",
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("Slate");
			PrivateDependencyModuleNames.Add("SlateCore");
		}
	}
}
