// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PXBasicBuildSaveGame.generated.h"



UCLASS()
class PIXEL2DKIT_API UPXBasicBuildSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicBuild")
	int RemainGoldNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicBuild")
	TArray<FName> ChoicedTalentIndexes;


	// 当前累计可用天赋点
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicBuild")
	int TotalTalentPoint = 10;

	// 天赋点最高上限
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicBuild")
	int MaxTalentPoint = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicBuild")
	TArray<FName> UnlockAbilityIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicBuild")
	TArray<FName> UnlockTalentIndexes;
};
