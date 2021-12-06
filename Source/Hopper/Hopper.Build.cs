// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Hopper : ModuleRules
{
	public Hopper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Core
		PublicDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "InputCore"});
		PrivateDependencyModuleNames.AddRange(new string[] {"AIModule", "Slate", "SlateCore"});

		// Gameplay Ability System
		PublicDependencyModuleNames.AddRange(new string[] {"GameplayAbilities", "GameplayTags", "GameplayTasks"});

		// Automation Dependencies
		PublicDependencyModuleNames.AddRange(new string[] {"UnrealEd"});

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
