// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class PixelBlitzEditorTarget : TargetRules
{
	public PixelBlitzEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		ExtraModuleNames.AddRange( new string[] { "PixelBlitz" } );
	}
}
