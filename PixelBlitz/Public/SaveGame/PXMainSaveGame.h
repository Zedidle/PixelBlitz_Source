// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PXCustomStruct.h"
#include "PXMainSaveGame.generated.h"


// Main 存档的域归属：语义上 Main 数据只属于战斗竞速（R8 隔离裁决）。
// 教程/主城/竞技场激活时会清空 Main 并改写归属；Load 按钮(Main_Saved)只认 Race。
UENUM(BlueprintType)
enum class ESaveDomain : uint8
{
	None UMETA(DisplayName = "无"),
	Race UMETA(DisplayName = "战斗竞速"),
	Arena UMETA(DisplayName = "竞技场"),
	Guide UMETA(DisplayName = "教程"),
	Home UMETA(DisplayName = "主城")
};

UCLASS(BlueprintType)
class PIXELBLITZ_API UPXMainSaveGame : public USaveGame
{
	GENERATED_BODY()


public:
	// 当前存档归属域（竞速运行时为 Race，其余模式激活时被清空并改写）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESaveDomain OwnerDomain = ESaveDomain::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurLevelToChoiceAbility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RemLife = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxLife = 5;

	// 技能选择的剩余点数
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RemSkillPoints = 0;

	// 技能选择的剩余刷新点数
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

	// 角色读档某局生成时，叠加局内属性
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterAttribute CharacterInheritAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurLevel = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RemLevels;
};
