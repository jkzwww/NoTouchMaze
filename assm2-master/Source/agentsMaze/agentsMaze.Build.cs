// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class agentsMaze : ModuleRules
{
	public agentsMaze(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
