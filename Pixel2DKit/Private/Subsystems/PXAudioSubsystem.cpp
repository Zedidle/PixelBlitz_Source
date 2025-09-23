// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/PXAudioSubsystem.h"

#include "Components/AudioComponent.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"


void UPXAudioSubsystem::Deinitialize()
{
	Super::Deinitialize();

	CloseBGM();
	
}

void UPXAudioSubsystem::ReplayCurBGM()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CurBGM)
	
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)
	
	UPXSettingSaveGame* SG = UPXSaveGameSubSystemFuncLib::GetSettingData(World);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SG);
	
	if (SG->SoundSetting_ToggleAll)
	{
		CurBGM->Play();
	}
}

void UPXAudioSubsystem::PlayBGM(USoundBase* Sound, float StartTime)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Sound)

	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)
	
	UPXSettingSaveGame* SG = UPXSaveGameSubSystemFuncLib::GetSettingData(World);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SG);

	if (CurBGM)
	{
		if (CurBGM->GetSound() == Sound) return;
		
		if (CurBGM->IsPlaying())
		{
			CloseBGM();
		}
	}

	if (SG->SoundSetting_ToggleAll)
	{
		if (UAudioComponent* BGM = UGameplayStatics::SpawnSound2D(World, Sound, SG->SoundSetting_MusicValue, 1.0f, StartTime))
		{
			BGM->FadeIn(5,1, 0, EAudioFaderCurve::Linear);
			CurBGM = BGM;
			BGM->OnAudioFinished.AddDynamic(this, &ThisClass::OnBGM_Finished);
		}
	}
}

void UPXAudioSubsystem::CloseBGM()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CurBGM);

	CurBGM->Stop();
	CurBGM->DestroyComponent();
	CurBGM = nullptr;
}

void UPXAudioSubsystem::SetBGMVolume(float VolumeMultiplier)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CurBGM);
	CurBGM->SetVolumeMultiplier(VolumeMultiplier);
}

