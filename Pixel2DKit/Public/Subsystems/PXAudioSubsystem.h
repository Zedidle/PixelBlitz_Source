// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PXAudioSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	UAudioComponent* CurBGM;
	
public:

	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Begin")
	void ReplayCurBGM();

	UFUNCTION(BlueprintCallable, Category = "Begin")
	void PlayBGM(USoundBase* Sound, float StartTime = 0);

	UFUNCTION(BlueprintCallable, Category = "Begin")
	void CloseBGM();

	UFUNCTION(BlueprintCallable, Category = "Begin")
	void SetBGMVolume(float VolumeMultiplier);



protected:

	UFUNCTION()
	void OnBGM_Finished();
};

inline void UPXAudioSubsystem::OnBGM_Finished()
{
	if (CurBGM)
	{
		CurBGM->Play();
	}
}
