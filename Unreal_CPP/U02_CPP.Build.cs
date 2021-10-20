// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class U02_CPP : ModuleRules
{
	public U02_CPP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"AIModule",
			"GameplayTasks",
			"CableComponent",
			"ApexDestruction",
			"Niagara",
			"UMG",
			"ProceduralMeshComponent"
		});

		PublicIncludePaths.Add(ModuleDirectory);

	}
}
