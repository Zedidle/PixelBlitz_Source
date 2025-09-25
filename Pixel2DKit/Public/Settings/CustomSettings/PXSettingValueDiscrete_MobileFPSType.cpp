// Copyright Epic Games, Inc. All Rights Reserved.

#include "PXSettingValueDiscrete_MobileFPSType.h"

#include "Performance/PXPerformanceSettings.h"
#include "Settings/PXSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXSettingValueDiscrete_MobileFPSType)

#define LOCTEXT_NAMESPACE "PXSettings"

UPXSettingValueDiscrete_MobileFPSType::UPXSettingValueDiscrete_MobileFPSType()
{
}

void UPXSettingValueDiscrete_MobileFPSType::OnInitialized()
{
	Super::OnInitialized();

	const UPXPlatformSpecificRenderingSettings* PlatformSettings = UPXPlatformSpecificRenderingSettings::Get();
	const UPXSettingsLocal* UserSettings = UPXSettingsLocal::Get();

	for (int32 TestLimit : PlatformSettings->MobileFrameRateLimits)
	{
		if (UPXSettingsLocal::IsSupportedMobileFramePace(TestLimit))
		{
			FPSOptions.Add(TestLimit, MakeLimitString(TestLimit));
		}
	}

	const int32 FirstFrameRateWithQualityLimit = UserSettings->GetFirstFrameRateWithQualityLimit();
	if (FirstFrameRateWithQualityLimit > 0)
	{
		SetWarningRichText(FText::Format(LOCTEXT("MobileFPSType_Note", "<strong>Note: Changing the framerate setting to {0} or higher might lower your Quality Presets.</>"), MakeLimitString(FirstFrameRateWithQualityLimit)));
	}
}

int32 UPXSettingValueDiscrete_MobileFPSType::GetDefaultFPS() const
{
	return UPXSettingsLocal::GetDefaultMobileFrameRate();
}

FText UPXSettingValueDiscrete_MobileFPSType::MakeLimitString(int32 Number)
{
	return FText::Format(LOCTEXT("MobileFrameRateOption", "{0} FPS"), FText::AsNumber(Number));
}

void UPXSettingValueDiscrete_MobileFPSType::StoreInitial()
{
	InitialValue = GetValue();
}

void UPXSettingValueDiscrete_MobileFPSType::ResetToDefault()
{
	SetValue(GetDefaultFPS(), EGameSettingChangeReason::ResetToDefault);
}

void UPXSettingValueDiscrete_MobileFPSType::RestoreToInitial()
{
	SetValue(InitialValue, EGameSettingChangeReason::RestoreToInitial);
}

void UPXSettingValueDiscrete_MobileFPSType::SetDiscreteOptionByIndex(int32 Index)
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);

	int32 NewMode = FPSOptionsModes.IsValidIndex(Index) ? FPSOptionsModes[Index] : GetDefaultFPS();

	SetValue(NewMode, EGameSettingChangeReason::Change);
}

int32 UPXSettingValueDiscrete_MobileFPSType::GetDiscreteOptionIndex() const
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);
	return FPSOptionsModes.IndexOfByKey(GetValue());
}

TArray<FText> UPXSettingValueDiscrete_MobileFPSType::GetDiscreteOptions() const
{
	TArray<FText> Options;
	FPSOptions.GenerateValueArray(Options);

	return Options;
}

int32 UPXSettingValueDiscrete_MobileFPSType::GetValue() const
{
	return UPXSettingsLocal::Get()->GetDesiredMobileFrameRateLimit();
}

void UPXSettingValueDiscrete_MobileFPSType::SetValue(int32 NewLimitFPS, EGameSettingChangeReason InReason)
{
	UPXSettingsLocal::Get()->SetDesiredMobileFrameRateLimit(NewLimitFPS);

	NotifySettingChanged(InReason);
}

#undef LOCTEXT_NAMESPACE

