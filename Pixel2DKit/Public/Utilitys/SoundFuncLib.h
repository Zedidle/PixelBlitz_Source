// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoundFuncLib.generated.h"

class USoundBase;
/**
 * 
 */
UCLASS(Abstract, MinimalAPI)
class USoundFuncLib : public UObject
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, Category="Sound FuncLib")
	static void PlaySoundAtLocation(USoundBase* Sound, FVector Location, float Volume = 1.0f);

	UFUNCTION(BlueprintCallable, Category="Sound FuncLib")
	static void PlaySound2D(USoundBase* Sound, float Volume = 1.0f);

	UFUNCTION(BlueprintCallable, Category="Sound FuncLib")
	static void PlayUISound(USoundBase* Sound, float Volume = 1.0f);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Sound FuncLib")
	static float GetCurVolumeValue();
	
};
