// Author: Antonio Sidenko (Tonetfal), May 2026

using UnrealBuildTool;

public class AssetsSortingEditor : ModuleRules
{
	public AssetsSortingEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"AssetRegistry",
			"AssetsSorting",
			"Blutility",
			"DeveloperSettings",
			"EditorScriptingUtilities",
			"Slate",
			"SlateCore",
			"UnrealEd",
		});
	}
}
