// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PXAchievementsSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXAchievementsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievements")
	TArray<FName> CompletedAchievements;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievements")
	float TotalUseTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievements")
	int KilledBossNum;
};
