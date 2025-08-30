// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BasePXCharacter.h"
#include "GameFramework/GameState.h"
#include "PXGameState.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API APXGameState : public AGameState
{
	GENERATED_BODY()


public:

	// UPROPERTY(BlueprintReadWrite)
	// FName CurWeatherIndex;
	
	UPROPERTY(BlueprintReadOnly)
	FWeatherEffectData WeatherEffectData;

	UFUNCTION()
	void LoadWeatherEffect();

	UFUNCTION(BlueprintCallable)
	void SetWeatherEffect(const FWeatherEffectData& data);
	
};
