// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/PXGameSettingRegistry.h"
#include "GameSettingCollection.h"
#include "Settings/PXSettingsLocal.h"
#include "Settings/PXSettingsShared.h"
#include "Player/PXLocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXGameSettingRegistry)

DEFINE_LOG_CATEGORY(LogPXGameSettingRegistry);

#define LOCTEXT_NAMESPACE "PX"

//--------------------------------------
// UPXGameSettingRegistry
//--------------------------------------

UPXGameSettingRegistry::UPXGameSettingRegistry()
{
}

UPXGameSettingRegistry* UPXGameSettingRegistry::Get(UPXLocalPlayer* InLocalPlayer)
{
	UPXGameSettingRegistry* Registry = FindObject<UPXGameSettingRegistry>(InLocalPlayer, TEXT("PXGameSettingRegistry"), true);
	if (Registry == nullptr)
	{
		Registry = NewObject<UPXGameSettingRegistry>(InLocalPlayer, TEXT("PXGameSettingRegistry"));
		Registry->Initialize(InLocalPlayer);
	}

	return Registry;
}

bool UPXGameSettingRegistry::IsFinishedInitializing() const
{
	if (Super::IsFinishedInitializing())
	{
		if (UPXLocalPlayer* LocalPlayer = Cast<UPXLocalPlayer>(OwningLocalPlayer))
		{
			if (LocalPlayer->GetSharedSettings() == nullptr)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void UPXGameSettingRegistry::OnInitialize(ULocalPlayer* InLocalPlayer)
{
	UPXLocalPlayer* PXLocalPlayer = Cast<UPXLocalPlayer>(InLocalPlayer);

	VideoSettings = InitializeVideoSettings(PXLocalPlayer);
	InitializeVideoSettings_FrameRates(VideoSettings, PXLocalPlayer);
	RegisterSetting(VideoSettings);

	AudioSettings = InitializeAudioSettings(PXLocalPlayer);
	RegisterSetting(AudioSettings);

	GameplaySettings = InitializeGameplaySettings(PXLocalPlayer);
	RegisterSetting(GameplaySettings);

	MouseAndKeyboardSettings = InitializeMouseAndKeyboardSettings(PXLocalPlayer);
	RegisterSetting(MouseAndKeyboardSettings);

	GamepadSettings = InitializeGamepadSettings(PXLocalPlayer);
	RegisterSetting(GamepadSettings);
}

void UPXGameSettingRegistry::SaveChanges()
{
	Super::SaveChanges();
	
	if (UPXLocalPlayer* LocalPlayer = Cast<UPXLocalPlayer>(OwningLocalPlayer))
	{
		// Game user settings need to be applied to handle things like resolution, this saves indirectly
		LocalPlayer->GetLocalSettings()->ApplySettings(false);
		
		LocalPlayer->GetSharedSettings()->ApplySettings();
		LocalPlayer->GetSharedSettings()->SaveSettings();
	}
}

#undef LOCTEXT_NAMESPACE

