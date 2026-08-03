// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE58GroomExporter : ModuleRules
{
	public UE58GroomExporter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);


        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",

				"Slate",
				"SlateCore",

				// Editor
				"ToolMenus",
				"ContentBrowser",
				"ContentBrowserData",
				"DesktopPlatform",

				// Hair
				"HairStrandsCore",
				"HairStrandsEditor",

				// Misc
				"Projects",

				// Export
				"AlembicLibrary",
				"AlembicImporter"
			}
		);


        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
