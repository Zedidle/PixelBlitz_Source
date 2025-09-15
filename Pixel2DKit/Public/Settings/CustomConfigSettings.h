// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Engine/DeveloperSettings.h"
#include "CustomConfigSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Custom Config Settings"))
class PIXEL2DKIT_API UCustomConfigSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> Gamepad_IMC;

	UPROPERTY(Config, EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> Keyboard_IMC;

	UPROPERTY(Config, EditAnywhere, Category = "Weather")
	TSoftObjectPtr<class UDayTimeEffectSight> DayTimeEffect;

	UPROPERTY(Config, EditAnywhere, Category = "GAS | GameplayEffect")
	TSubclassOf<class UPXGameplayEffect> GE_WeatherEffect_HP;
};
