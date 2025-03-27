// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Industrial_AI_buddy : ModuleRules
{
	public Industrial_AI_buddy(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AudioCapture", "AudioMixer", "Json", "JsonUtilities" });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
