// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LocalizationFuncLib.h"
#include "UObject/NoExportTypes.h"
#include "Abilities/GameplayAbility.h"
#include "PXCustomStruct.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXCustomStruct : public UObject
{
	GENERATED_BODY()
	
};


#pragma region EWeather
UENUM(BlueprintType)
enum EWeather : uint8
{
	Overcast UMETA(DisplayName = "阴"),
	Fog UMETA(DisplayName = "雾"),
	Sand UMETA(DisplayName = "沙"),
	Snow UMETA(DisplayName = "雪"),
	Rain UMETA(DisplayName = "雨"),
	Sunny UMETA(DisplayName = "晴")
	
};

#pragma endregion

#pragma region EStyleType
UENUM(BlueprintType)
enum EStyleType : uint8
{
	Normal UMETA(DisplayName = "正常"),
	Cartoon UMETA(DisplayName = "卡通"),
	Hero UMETA(DisplayName = "英雄"),
	Old UMETA(DisplayName = "黑白")
};
#pragma endregion

#pragma region EAbilityQuality
UENUM(BlueprintType)
enum EAbilityQuality : uint8
{
	Level1 UMETA(DisplayName = "白银"),
	Level2 UMETA(DisplayName = "黄金"),
	Level3 UMETA(DisplayName = "紫金")
};
#pragma endregion 



#pragma region FDrop
USTRUCT(BlueprintType)
struct FDrop
{
	GENERATED_BODY()

	// 物品及对应概率比率
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	TMap<FName, int> Items; 

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	int DropTotalNum = 1;
};
#pragma endregion



#pragma region FAbility
USTRUCT(BlueprintType)
struct FAbility: public FTableRowBase
{
	GENERATED_BODY()

	// 是否已经禁用
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	bool Enable;

	// 是否默认未解锁
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	bool DefaultLock;

	// 可以学习的角色下标，对应角色表DT_CharacterData，如果是0则通用
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	FName CharacterName;

	// 费用，暂时都是1
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	int32 Price;

	// 技能名称本地化
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	FLocationTableData AbilityName_Localized;

	// 技能描述本地化
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	FLocationTableData AbilityDesc_Localized;

	// 技能品阶
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	TEnumAsByte<EAbilityQuality> AbilityQuality;

	// 当前技能等级
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	int32 CurLevel;

	// 技能的最大等级
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	int32 MaxLevel;

	// 前置学习的技能NameIndex，如果前技能没学习，则无法学习该技能
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	FName PreLevelIndex;

	// 技能的下一等级在技能列表的NameIndex
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	FName NextLevelIndex;

	// 技能的特殊附加值，以GameplayTag为前缀
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	TMap<FGameplayTag, float> Effect_GameplayTag;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	TSoftClassPtr<UGameplayAbility> AbilityClass;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	FGameplayTag AbilityTag;
};


#pragma endregion



#pragma region FBuffValueEffect

USTRUCT(BlueprintType)
struct FBuffValueEffect
{
	GENERATED_BODY()
	FBuffValueEffect(){}
	FBuffValueEffect(float Percent, float Value): EffectedPercent(Percent), EffectedValue(Value){}

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Buff")
	float EffectedPercent;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Buff")
	float EffectedValue;

};

#pragma endregion 



#pragma region FCharacterAttribute

USTRUCT(BlueprintType)
struct FCharacterAttribute
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float SpringArmLengthSight;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	int MaxHealth;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	int MaxEnergy;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	int BasicAttackValue;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float WalkMoveSpeed;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float WalkMoveAcceleration;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float JumpHeight;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float RepelResistPercent;
	
};
#pragma endregion


#pragma region FBuffOnWidget
USTRUCT(BlueprintType)
struct FBuffOnWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="BuffOnWidget")
	FName RowName;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="BuffOnWidget")
	FGameplayTag Tag;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="BuffOnWidget")
	FLinearColor Color;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="BuffOnWidget")
	bool Permanent;
	
};


#pragma endregion