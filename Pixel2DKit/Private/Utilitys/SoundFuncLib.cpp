// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/SoundFuncLib.h"
#include "Core/PXGameMode.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/PXSettingsLocal.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXResourceDataAsset.h"

void USoundFuncLib::PlaySoundAtLocation(USoundBase* Sound, FVector Location)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	const UPXSettingsLocal* SettingsLocal = GetDefault<UPXSettingsLocal>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SettingsLocal)
	
	if (SettingsLocal->GetOverallVolume() <= 0) return;

	float Volume = SettingsLocal->GetOverallVolume() * GetDefault<UPXSettingsLocal>()->GetSoundBattleVolume();
	
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

	const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceDataAsset)

	if (ResourceDataAsset->Attenuation_Default)
	{
		UGameplayStatics::PlaySoundAtLocation(World, Sound, Location, Volume, 1.0f, 0.0f, ResourceDataAsset->Attenuation_Default.Get());
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(World, Sound, Location, Volume);
	}
}

void USoundFuncLib::PlaySound2D(USoundBase* Sound)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	const UPXSettingsLocal* SettingsLocal = GetDefault<UPXSettingsLocal>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SettingsLocal)
	
	if (SettingsLocal->GetOverallVolume() <= 0) return;

	float Volume = SettingsLocal->GetOverallVolume() * GetDefault<UPXSettingsLocal>()->GetSoundBattleVolume();

	UGameplayStatics::PlaySound2D(World, Sound, Volume);
}

void USoundFuncLib::PlayUISound(USoundBase* Sound)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	const UPXSettingsLocal* SettingsLocal = GetDefault<UPXSettingsLocal>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SettingsLocal)
	
	if (SettingsLocal->GetOverallVolume() <= 0) return;

	float Volume = SettingsLocal->GetOverallVolume() * GetDefault<UPXSettingsLocal>()->GetSoundUIVolume();
	
	UGameplayStatics::PlaySound2D(World, Sound, Volume);
}

float USoundFuncLib::GetCurVolumeValue()
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(World, 0.0f)

	const UPXSettingsLocal* SettingsLocal = GetDefault<UPXSettingsLocal>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(SettingsLocal, 0.0f)

	return SettingsLocal->GetOverallVolume();
}
