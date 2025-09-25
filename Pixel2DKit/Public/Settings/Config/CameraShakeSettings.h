// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CameraShakeSettings.generated.h"

class ULegacyCameraShake;
/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Camera Shake Settings"))
class PIXEL2DKIT_API UCameraShakeSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> BowAttackLongShake;

	UPROPERTY(Config, EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> BowAttackShortShake;

	UPROPERTY(Config, EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> MonsterHitedShake;

	UPROPERTY(Config, EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> OnBossLandedShake;

	UPROPERTY(Config, EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> PlayerAttackShake;

	UPROPERTY(Config, EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> PlayerHitedShake;

	UPROPERTY(Config, EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> PlayerHitedShake_Powerful;
	
	UPROPERTY(Config, EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> PlayerLandedShake;
	
	UPROPERTY(Config, EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> SwordAttackShortShake;
	
};
