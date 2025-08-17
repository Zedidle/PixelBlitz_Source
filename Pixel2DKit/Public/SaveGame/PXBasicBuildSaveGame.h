// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PXBasicBuildSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FPXBasicBuildSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicBuild")
	int RemainGoldNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicBuild")
	TArray<FName> ChoicedTalentIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicBuild")
	int TotalTalentPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicBuild")
	int MaxTalentPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicBuild")
	TArray<FName> UnlockAbilityIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicBuild")
	TArray<FName> UnlockTalentIndexes;

	
};


UCLASS()
class PIXEL2DKIT_API UPXBasicBuildSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	FPXBasicBuildSaveData Data;
};
