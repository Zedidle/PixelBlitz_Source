// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PXDataTablesDataAsset.generated.h"

class UDataTable;


UCLASS()
class PIXEL2DKIT_API UPXDataTablesDataAsset : public UDataAsset
{
	GENERATED_BODY()

	
	UPROPERTY(EditAnywhere, Category = "Character")
	TSoftObjectPtr<UDataTable> UIDataPath;
	
	UPROPERTY(EditAnywhere, Category = "Character")
	TSoftObjectPtr<UDataTable> CharacterDataPath;
	
	UPROPERTY(EditAnywhere, Category = "ItemData")
	TSoftObjectPtr<UDataTable> ItemDataPath;
	
	UPROPERTY(EditAnywhere, Category = "Buff")
	TSoftObjectPtr<UDataTable> BuffOnWidgetDataPath;
	
	UPROPERTY(EditAnywhere, Category = "Ability")
	TArray<TSoftObjectPtr<UDataTable>> AbilityDataPaths;
	
	UPROPERTY(EditAnywhere, Category = "Talent")
	TSoftObjectPtr<UDataTable> TalentDataPath;
	
	UPROPERTY(EditAnywhere, Category = "Achievement")
	TSoftObjectPtr<UDataTable> AchievementDataPath;
	
	UPROPERTY(EditAnywhere, Category = "Weather")
	TSoftObjectPtr<UDataTable> LevelWeatherRateDataPath;
	
	UPROPERTY(EditAnywhere, Category = "Weather")
	TSoftObjectPtr<UDataTable> WeatherDataPath;
	
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSoftObjectPtr<UDataTable> WeaponDataPath;
	
};
