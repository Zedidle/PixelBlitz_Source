// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PXCustomSettings.generated.h"

class UPXWidgetsDataAsset;
class UPXSkillsDataAsset;
class UPXResourceDataAsset;
class UPXDataTablesDataAsset;
class UPXCameraShakeDataAsset;
class UPXGameDataAsset;
class UDataAsset;

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "PX Custom Settings"))
class PIXEL2DKIT_API UPXCustomSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<UPXGameDataAsset> GameDataAsset;
	
	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<UPXCameraShakeDataAsset> CameraShakeDataAsset;

	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<UPXDataTablesDataAsset> DataTablesDataAsset;

	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<UPXResourceDataAsset> ResourceDataAsset;

	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<UPXSkillsDataAsset> SkillsDataAsset;

	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<UPXWidgetsDataAsset> WidgetsDataAsset;

	
};
