// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Utilitys/PixelCustomStruct.h"
#include "CustomResourceSettings.generated.h"

class UNiagaraSystem;
class USoundCue;

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Custom Resource Settings"))
class PIXEL2DKIT_API UCustomResourceSettings : public UDeveloperSettings
{
	GENERATED_BODY()


public:
	UPROPERTY(Config, EditAnywhere, Category = "Custom Resource")
	TMap<TEnumAsByte<EWeather>, TSoftObjectPtr<UNiagaraSystem>> WeatherTypeToNiagara;

	UPROPERTY(Config, EditAnywhere, Category = "Custom Resource")
	TMap<TEnumAsByte<EWeather>, TSoftObjectPtr<USoundBase>> WeatherTypeToSound;

	UPROPERTY(Config, EditAnywhere, Category = "Custom Resource")
	TSoftObjectPtr<USoundAttenuation> Attenuation_Default;
};
