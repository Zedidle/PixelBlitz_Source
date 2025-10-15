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
	FName CurLevelToChoiceAbility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RemLife = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxLife = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RemSkillPoints = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RemRefreshPoints = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> ChosenAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> TakeEffectAbilities;

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

	// 当前小关累计金币
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int JustPickedGolds;

	// 当局累计金币
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RoundGoldNum;

	// 是否拾取了所有金币
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PickupAllGolds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SkyTimeOfDay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TotalPerfectDodgeTimes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurCharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterAttribute CharacterInheritAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurLevel = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RemLevels;
};
