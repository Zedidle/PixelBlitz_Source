// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PXCustomSettings.generated.h"

class UDevelopConfigDataAsset;
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

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<UDevelopConfigDataAsset> DevelopConfig;
	
	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<UPXGameDataAsset> GameDataAsset;
	
	UPROPERTY(Config, BlueprintReadOnly,EditAnywhere)
	TSoftObjectPtr<UPXCameraShakeDataAsset> CameraShakeDataAsset;

	UPROPERTY(Config, BlueprintReadOnly,EditAnywhere)
	TSoftObjectPtr<UPXDataTablesDataAsset> DataTablesDataAsset;

	UPROPERTY(Config, BlueprintReadOnly,EditAnywhere)
	TSoftObjectPtr<UPXResourceDataAsset> ResourceDataAsset;

	UPROPERTY(Config, BlueprintReadOnly,EditAnywhere)
	TSoftObjectPtr<UPXSkillsDataAsset> SkillsDataAsset;

	UPROPERTY(Config, BlueprintReadOnly,EditAnywhere)
	TSoftObjectPtr<UPXWidgetsDataAsset> WidgetsDataAsset;

	
};
