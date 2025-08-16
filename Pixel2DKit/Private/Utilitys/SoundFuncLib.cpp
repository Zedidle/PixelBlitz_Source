// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/SoundFuncLib.h"
#include "Basic/PXGameInstance.h"
#include "Basic/PXGameMode.h"
#include "CompGeom/FitOrientedBox3.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/CustomResourceSettings.h"

void USoundFuncLib::PlaySoundAtLocation(USoundBase* Sound, FVector Location, float Volume)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)
	
	UPXGameInstance* GI = Cast< UPXGameInstance >(UGameplayStatics::GetGameInstance(World));
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GI)
	if (!GI->SoundSetting_ToggleAll) return;

	APXGameMode* GameMode = Cast<APXGameMode>(UGameplayStatics::GetGameMode(World));
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameMode)

	float _Volume = GameMode->SoundSetting_Arg_MusicBasicMulti * GI->SoundSetting_VolumeValue * Volume;

	
	const UCustomResourceSettings* ResourceSetting = GetDefault<UCustomResourceSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceSetting)

	if (ResourceSetting->Attenuation_Default)
	{
		UGameplayStatics::PlaySoundAtLocation(World, Sound, Location, _Volume, 1.0f, 0.0f, ResourceSetting->Attenuation_Default.Get());
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(World, Sound, Location, _Volume);
	}
}

void USoundFuncLib::PlaySound2D(USoundBase* Sound, float Volume)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)
	
	UPXGameInstance* GI = Cast< UPXGameInstance >(UGameplayStatics::GetGameInstance(World));
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GI)
	if (!GI->SoundSetting_ToggleAll) return;

	APXGameMode* GameMode = Cast<APXGameMode>(UGameplayStatics::GetGameMode(World));
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameMode)

	float _Volume = GameMode->SoundSetting_Arg_VolumeBasicMulti * GI->SoundSetting_VolumeValue * Volume;
	UGameplayStatics::PlaySound2D(World, Sound, _Volume);
}

void USoundFuncLib::PlayUISound(USoundBase* Sound, float Volume)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)
	
	UPXGameInstance* GI = Cast< UPXGameInstance >(UGameplayStatics::GetGameInstance(World));
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GI)
	if (!GI->SoundSetting_ToggleAll) return;

	float _Volume = GI->SoundSetting_UIValue * GI->SoundSetting_UIValue_BasicArg * Volume;
	UGameplayStatics::PlaySound2D(World, Sound, _Volume);
}

float USoundFuncLib::GetCurVolumeValue()
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!World) return 0.0f;
	
	UPXGameInstance* GI = Cast< UPXGameInstance >(UGameplayStatics::GetGameInstance(World));
	if (!GI) return 0.0f;
	if (!GI->SoundSetting_ToggleAll) return 0.0f;

	APXGameMode* GameMode = Cast<APXGameMode>(UGameplayStatics::GetGameMode(World));
	if (!GameMode) return 1.0f;

	return GameMode->SoundSetting_Arg_VolumeBasicMulti * GI->SoundSetting_VolumeValue;
}
