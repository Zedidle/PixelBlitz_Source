// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Utilitys/LocalizationFuncLib.h"
#include "AchievementSubsystem.generated.h"

UENUM(BlueprintType)
enum class EAchievementType : uint8
{
	Usual UMETA(DisplayName = "通用"),
	Boss UMETA(DisplayName = "BOSS"),
	Level UMETA(DisplayName = "关卡"),
	Skill UMETA(DisplayName = "技巧"),
	Gold UMETA(DisplayName = "金币")
	
};

USTRUCT(BlueprintType)
struct FAchievement: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	FLocalizedTableData AchievementName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	FLocalizedTableData AchievementDesc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	TEnumAsByte<EAchievementType> Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Achievement")
	int CompleteNum; 
};

UCLASS()
class PIXEL2DKIT_API UAchievementSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AchievementSubsystem")
	void CompleteAchievement(FName AchievementRowName);

	
	/* 击杀BOSS时触发的相关成就
	 * InjuredNum: Boss受伤次数
	 */
	UFUNCTION(BlueprintCallable, Category = "AchievementSubsystem")
	void Achievement_KillBoss(int InjuredNum);

	/* 通关触发的相关成就
	* UseTime: 总用时
	* bClearAllMonsters: 是否清除了所有怪物
	* bPickupAllGolds: 是否拾取了所有金币
	* LostLife: 死亡次数
	* HurtTimes: 受伤次数 
	*/
	UFUNCTION(BlueprintCallable, Category = "AchievementSubsystem")
	void Achievement_Completion(float UseTime, bool bClearAllEnemy, bool bPickupAllGolds, int DieTimes, int HurtTimes);

	UFUNCTION(BlueprintCallable, Category = "AchievementSubsystem")
	void Achievement_LevelPass(int PerfectDodgeTimes, int AttackHitInDashTimes);
};
