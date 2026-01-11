// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Utilitys/PXCustomStruct.h"
#include "PXResourceDataAsset.generated.h"

class APlayerRespawnPoint;
class UNiagaraSystem;

UCLASS(BlueprintType)
class PIXEL2DKIT_API UPXResourceDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY( EditAnywhere, Category = "Custom | FootStep")
	TMap<EWeather, TSoftObjectPtr<UNiagaraSystem>> WeatherTypeToNiagara;

	UPROPERTY( EditAnywhere, Category = "Custom | FootStep")
	TMap<EWeather, TSoftObjectPtr<USoundBase>> WeatherTypeToSound;

	UPROPERTY( EditAnywhere, Category = "Custom | Audio")
	TSoftObjectPtr<USoundAttenuation> Attenuation_Default;


	UPROPERTY( EditAnywhere, Category = "Ability | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_Mobiliarbus;

	UPROPERTY( EditAnywhere, Category = "Ability | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_Revive;

	UPROPERTY( EditAnywhere, Category = "Ability | SoundCue")
	TSoftObjectPtr<USoundCue> SC_Revive;


	
	UPROPERTY( EditAnywhere, Category = "Common | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_HitSmoke;
	UPROPERTY( EditAnywhere, Category = "Common | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_HitBlood;
	UPROPERTY( EditAnywhere, Category = "Common | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_HitBlood_Little;


	
	UPROPERTY( EditAnywhere, Category = "Buff | Niagara")
	TSoftObjectPtr<UNiagaraSystem> NS_SpeedUP;

	
	UPROPERTY( EditAnywhere, Category = "GameplayFlow | SoundCue")
	TSoftObjectPtr<USoundCue> SC_OnDie;
	
	UPROPERTY( EditAnywhere, Category = "GameplayFlow | Actor")
	TSubclassOf<APlayerRespawnPoint> PlayerRespawnPointClass;

	
	UPROPERTY( EditAnywhere, Category = "Buff | Niagara")
	TSoftObjectPtr<USoundWave> SW_BeginBGM;

	UPROPERTY( EditAnywhere, Category = "Combo | Sound")
	TArray<TSoftObjectPtr<USoundBase>> SW_ComboWeakPoints;
	
};
