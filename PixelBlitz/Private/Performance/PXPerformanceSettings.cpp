// Fill out your copyright notice in the Description page of Project Settings.


#include "Performance/PXPerformanceSettings.h"
#include "Engine/PlatformSettingsManager.h"
#include "Misc/EnumRange.h"
#include "Performance/PXPerformanceStatTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXPerformanceSettings)

//////////////////////////////////////////////////////////////////////

UPXPlatformSpecificRenderingSettings::UPXPlatformSpecificRenderingSettings()
{
	MobileFrameRateLimits.Append({ 20, 30, 45, 60, 90, 120 });
}

const UPXPlatformSpecificRenderingSettings* UPXPlatformSpecificRenderingSettings::Get()
{
	UPXPlatformSpecificRenderingSettings* Result = UPlatformSettingsManager::Get().GetSettingsForPlatform<ThisClass>();
	check(Result);
	return Result;
}

//////////////////////////////////////////////////////////////////////

UPXPerformanceSettings::UPXPerformanceSettings()
{
	PerPlatformSettings.Initialize(UPXPlatformSpecificRenderingSettings::StaticClass());

	CategoryName = TEXT("Game");

	DesktopFrameRateLimits.Append({ 30, 60, 120, 144, 160, 165, 180, 200, 240, 360 });

	// Default to all stats are allowed
	FPXPerformanceStatGroup& StatGroup = UserFacingPerformanceStats.AddDefaulted_GetRef();
	for (EPXDisplayablePerformanceStat PerfStat : TEnumRange<EPXDisplayablePerformanceStat>())
	{
		StatGroup.AllowedStats.Add(PerfStat);
	}
}

