// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PXCameraShakeDataAsset.generated.h"

class UCameraShakeBase;

UCLASS()
class PIXEL2DKIT_API UPXCameraShakeDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY( EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> BowAttackLongShake;

	UPROPERTY( EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> BowAttackShortShake;

	UPROPERTY( EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> MonsterHitedShake;

	UPROPERTY( EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> OnBossLandedShake;

	UPROPERTY( EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> PlayerAttackShake;

	UPROPERTY( EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> PlayerHitedShake;

	UPROPERTY( EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> PlayerHitedShake_Powerful;
	
	UPROPERTY( EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> PlayerLandedShake;
	
	UPROPERTY( EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> SwordAttackShortShake;
	
};
