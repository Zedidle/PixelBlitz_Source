// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class Pixel2DKit : ModuleRules
{
	public Pixel2DKit(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
        	PublicIncludePaths.AddRange(
			new string[] {
                		Path.Combine(ModuleDirectory, "Public"),
                		Path.Combine(ModuleDirectory, "Public/Steam")
		                
			}
		);

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", "CoreUObject", "Engine", "InputCore",
			"Niagara","GameplayTags",
			"UMG",
			// "SteamLeaderboardSubsystem",
			"OnlineSubsystemSteam", "OnlineSubsystem", "OnlineSubsystemUtils",
			"EnhancedInput",
			"PaperZD", "Paper2D",
			"NavigationSystem",
			"UMGEditor"
		});

		PrivateIncludePaths.AddRange(new string[]{
			// "OnlineSubsystemSteam/Private"
		});

		// DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore",  });
		
		// Uncomment if you are using online features
		PrivateDependencyModuleNames.Add("OnlineSubsystemSteam");
		PrivateDependencyModuleNames.AddRange( new string[]
		{
			
		});

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
