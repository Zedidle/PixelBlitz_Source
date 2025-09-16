// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PlayerState/PlayerRespawnPoint.h"
#include "Utilitys/PXCustomStruct.h"
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
	UPROPERTY(Config, EditAnywhere, Category = "Custom")
	TMap<TEnumAsByte<EWeather>, TSoftObjectPtr<UNiagaraSystem>> WeatherTypeToNiagara;

	UPROPERTY(Config, EditAnywhere, Category = "Custom")
	TMap<TEnumAsByte<EWeather>, TSoftObjectPtr<USoundBase>> WeatherTypeToSound;

	UPROPERTY(Config, EditAnywhere, Category = "Custom")
	TSoftObjectPtr<USoundAttenuation> Attenuation_Default;


	UPROPERTY(Config, EditAnywhere, Category = "Ability | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_Mobiliarbus;

	UPROPERTY(Config, EditAnywhere, Category = "Ability | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_Revive;

	UPROPERTY(Config, EditAnywhere, Category = "Ability | SoundCue")
	TSoftObjectPtr<USoundCue> SC_Revive;


	
	UPROPERTY(Config, EditAnywhere, Category = "Common | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_HitSmoke;
	UPROPERTY(Config, EditAnywhere, Category = "Common | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_HitBlood;
	UPROPERTY(Config, EditAnywhere, Category = "Common | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_HitBlood_Little;


	
	UPROPERTY(Config, EditAnywhere, Category = "Buff | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_SpeedUP;

	
	UPROPERTY(Config, EditAnywhere, Category = "GameplayFlow | SoundCue")
	TSoftObjectPtr<USoundCue> SC_OnDie;
	
	UPROPERTY(Config, EditAnywhere, Category = "GameplayFlow | Actor")
	TSubclassOf<APlayerRespawnPoint> PlayerRespawnPointClass;

	
	
};
