// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class PixelBlitzTarget : TargetRules
{
	public PixelBlitzTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "PixelBlitz"} );

		ApplySharedTargetSettings(this);
	}
	
	
	internal static void ApplySharedTargetSettings(TargetRules Target)
	{
		Target.DefaultBuildSettings = BuildSettingsVersion.V5;
		Target.IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
	}
}
