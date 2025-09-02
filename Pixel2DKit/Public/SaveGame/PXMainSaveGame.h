// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Utilitys/PXCustomStruct.h"
#include "PXMainSaveGame.generated.h"


UCLASS(BlueprintType)
class PIXEL2DKIT_API UPXMainSaveGame : public USaveGame
{
	GENERATED_BODY()


public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurLevelChoiceAbility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SupLife = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxLife = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> ChoicedAbilityIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> TakeEffectAbilityIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Results;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTotalMonsterClear = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PreLevelClearAllMonster;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DieTimes = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HurtTimes = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RoundGoldNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PickupAllGolds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SkyTimeOfDay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TotalPerfectDodgeTimes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TotalAttackInDashTimes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EStyleType> StyleType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPrimaryDataAsset> CurWeatherType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurCharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterAttribute CharacterInheritAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> SurLevels;
};
