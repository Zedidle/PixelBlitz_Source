// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/SoundFuncLib.h"
#include "Core/PXGameMode.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/Config/CustomResourceSettings.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXResourceDataAsset.h"

void USoundFuncLib::PlaySoundAtLocation(USoundBase* Sound, FVector Location, float Volume)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GEngine)

	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	UGameInstance* GameInstance = World->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)

	UPXSaveGameSubsystem* SaveGameSubsystem =  GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem)
	
	UPXSettingSaveGame* SaveGame = SaveGameSubsystem->GetSettingData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGame)

	if (!SaveGame->SoundSetting_ToggleAll) return;

	APXGameMode* GameMode = Cast<APXGameMode>(UGameplayStatics::GetGameMode(World));
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameMode)

	float _Volume = GameMode->SoundSetting_Arg_MusicBasicMulti * SaveGame->SoundSetting_VolumeValue * Volume;

	
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

	const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceDataAsset)

	if (ResourceDataAsset->Attenuation_Default)
	{
		UGameplayStatics::PlaySoundAtLocation(World, Sound, Location, _Volume, 1.0f, 0.0f, ResourceDataAsset->Attenuation_Default.Get());
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(World, Sound, Location, _Volume);
	}
}

void USoundFuncLib::PlaySound2D(USoundBase* Sound, float Volume)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GEngine)

	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	UGameInstance* GameInstance = World->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)

	UPXSaveGameSubsystem* SaveGameSubsystem =  GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem)
	
	UPXSettingSaveGame* SaveGame = SaveGameSubsystem->GetSettingData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGame)

	if (!SaveGame->SoundSetting_ToggleAll) return;

	APXGameMode* GameMode = Cast<APXGameMode>(UGameplayStatics::GetGameMode(World));
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameMode)

	float _Volume = GameMode->SoundSetting_Arg_VolumeBasicMulti * SaveGame->SoundSetting_VolumeValue * Volume;
	UGameplayStatics::PlaySound2D(World, Sound, _Volume);
}

void USoundFuncLib::PlayUISound(USoundBase* Sound, float Volume)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GEngine)

	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	UGameInstance* GameInstance = World->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)

	UPXSaveGameSubsystem* SaveGameSubsystem =  GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem)
	
	UPXSettingSaveGame* SaveGame = SaveGameSubsystem->GetSettingData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGame)

	if (!SaveGame->SoundSetting_ToggleAll) return;

	float _Volume = SaveGame->SoundSetting_UIValue * SaveGame->SoundSetting_UIValue_BasicArg * Volume;
	UGameplayStatics::PlaySound2D(World, Sound, _Volume);
}

float USoundFuncLib::GetCurVolumeValue()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GEngine, 0.0f)

	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(World, 0.0f)

	UGameInstance* GameInstance = World->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GameInstance, 0.0f)

	UPXSaveGameSubsystem* SaveGameSubsystem =  GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(SaveGameSubsystem, 0.0f)
	
	UPXSettingSaveGame* SaveGame = SaveGameSubsystem->GetSettingData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(SaveGame, 0.0f)

	if (!SaveGame->SoundSetting_ToggleAll) return 0.0f;

	APXGameMode* GameMode = Cast<APXGameMode>(UGameplayStatics::GetGameMode(World));
	if (!GameMode) return 1.0f;

	return GameMode->SoundSetting_Arg_VolumeBasicMulti * SaveGame->SoundSetting_VolumeValue;
}
