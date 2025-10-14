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
				"Pixel2DKit"
			}
		);

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject",
			"ApplicationCore",
			"Engine",  
			"PhysicsCore",
			"GameplayTags",
			"GameplayTasks", 
			"GameplayAbilities", 
			"AIModule",
			"ModularGameplay",
			"ModularGameplayActors", 
			"DataRegistry",
			"ReplicationGraph",
			"GameFeatures", 
			"SignificanceManager",
			"CommonLoadingScreen",
			"Niagara",
			"ControlFlows",
			"PropertyPath",
			"QuicMessagingTransport", 
			"OnlineSubsystemSteam", 
			"OnlineSubsystem", 
			"OnlineSubsystemUtils",
			"PaperZD", 
			"Paper2D",
			"WFCBaseOnActor",
			"NavigationSystem",
			"GameSubtitles", 
			"GameSettings",
			"AdvancedFadeObjects", 
			"CommonGame", 
			"GameplayMessageRuntime"
		});

		PrivateIncludePaths.AddRange(new string[]{
			// "OnlineSubsystemSteam/Private"
			
		});

		// DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"InputCore",
			"Slate", 
			"SlateCore",
			"RenderCore",
			"DeveloperSettings",
			"EnhancedInput",
			"NetCore",
			"RHI",
			"Projects",
			"Gauntlet",
			"UMG",
			"CommonUI",
			"CommonInput",
			"GameSettings",
			"CommonUser",
			"GameSubtitles",
			"AudioMixer",
			"AudioModulation",
			"UIExtension",
			"EngineSettings",
			"Json"
		});
		
		// Uncomment if you are using online features
		PrivateDependencyModuleNames.Add("OnlineSubsystemSteam");
		PrivateDependencyModuleNames.AddRange( new string[]
		{
			"OpenSubdiv"
		});

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
