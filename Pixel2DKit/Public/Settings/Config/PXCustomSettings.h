// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PXCustomSettings.generated.h"

class UEnemyActionMoveDataAsset;
class UPXForceFeedbackEffectDataAsset;
class UDevelopConfigDataAsset;
class UPXWidgetsDataAsset;
class UPXSkillsDataAsset;
class UPXResourceDataAsset;
class UPXDataTablesDataAsset;
class UPXCameraResourceDataAsset;
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
	TSoftObjectPtr<UPXCameraResourceDataAsset> CameraResourceDataAsset;

	UPROPERTY(Config, BlueprintReadOnly,EditAnywhere)
	TSoftObjectPtr<UEnemyActionMoveDataAsset> EnemyActionMoveDataAsset;
	
	UPROPERTY(Config, BlueprintReadOnly,EditAnywhere)
	TSoftObjectPtr<UPXForceFeedbackEffectDataAsset> ForceFeedbackEffectDataAsset;
	
	UPROPERTY(Config, BlueprintReadOnly,EditAnywhere)
	TSoftObjectPtr<UPXDataTablesDataAsset> DataTablesDataAsset;

	UPROPERTY(Config, BlueprintReadOnly,EditAnywhere)
	TSoftObjectPtr<UPXResourceDataAsset> ResourceDataAsset;

	UPROPERTY(Config, BlueprintReadOnly,EditAnywhere)
	TSoftObjectPtr<UPXWidgetsDataAsset> WidgetsDataAsset;

	
};
