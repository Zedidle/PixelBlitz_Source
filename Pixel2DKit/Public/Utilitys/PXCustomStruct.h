// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "Fight/Skills/BaseSkill.h"
#include "PXCustomStruct.generated.h"

class USoundCue;
class ABasePXCharacter;

UCLASS()
class PIXEL2DKIT_API UPXCustomStruct : public UObject
{
	GENERATED_BODY()
	
};


#pragma region UConfigDataAsset

UCLASS(Blueprintable)
class PIXEL2DKIT_API UDevelopConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Version = "v0.5.2.6.1";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDemo = true;

	// 是否处于测试状态
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsTesting = true;

	// 是否加载测试地图 L_Test
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLoadTestMap = false;

	// 勾选 bLoadTestMap 后，进行测试，如果不选择，则默认测试 L_Test
	UPROPERTY(EditAnywhere, Category=DefaultTestMap, meta=(AllowedClasses="/Script/Engine.World"))
	FSoftObjectPath TestMap;

};

#pragma endregion


#pragma region EWeather
UENUM(BlueprintType)
enum class EWeather : uint8
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
enum class EStyleType : uint8
{
	Normal UMETA(DisplayName = "正常"),
	Cartoon UMETA(DisplayName = "卡通"),
	Hero UMETA(DisplayName = "英雄"),
	Old UMETA(DisplayName = "黑白")
};
#pragma endregion

#pragma region EAbilityQuality
UENUM(BlueprintType)
enum class EAbilityQuality : uint8
{
	Level1 UMETA(DisplayName = "白银"),
	Level2 UMETA(DisplayName = "黄金"),
	Level3 UMETA(DisplayName = "传说")
};
#pragma endregion 



#pragma region FDrop
USTRUCT(BlueprintType)
struct FDrop : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	FName DropID;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	FText DropDesc;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	float SpawnRandRotate = 20.0f;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	float SpawnSpeed = 200.0f;
	
	// 物品生成的间隔
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	float SpawnFrequency = 0.2;
	
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
	bool Enable = true;

	// 是否默认未解锁
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	bool DefaultLock = false;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	FGameplayTag AbilityTag;

	// 技能名称本地化
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	FText AbilityName;

	// 技能描述本地化
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	FText AbilityDesc;
	
	// 费用，暂时都是1
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	int32 Price = 1;

	// 技能品阶
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	EAbilityQuality AbilityQuality = EAbilityQuality::Level1;

	// 唯一的技能的前一级，学习后会替换
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	FGameplayTag PreLevelAbility;
	
	// 前置所需学习的技能 GameplayTag
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	TArray<FGameplayTag> RequiredAbilities;

	// 前置选择的天赋 GameplayTag
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	TArray<FGameplayTag> RequiredTalents;
	
	// 技能的特殊附加值，以GameplayTag为前缀
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	TMap<FGameplayTag, float> Effect_GameplayTag;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	TArray<TSoftClassPtr<UGameplayAbility>> AbilityClass;
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
	float EffectedPercent = 0.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Buff")
	float EffectedValue = 0.0f;

};

#pragma endregion 


#pragma region Character Data

// 不能写默认参数，因为和关卡继承角色属性通用
USTRUCT(BlueprintType)
struct FCharacterAttribute
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float SpringArmLengthSight = 0.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	int MaxHP = 0;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	int MaxEP = 0;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	int BasicAttackValue = 0;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	int BasicRepelValue = 0;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float MaxWalkSpeed = 0.0f;

	// 技能的快速移动速度
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float BasicDashSpeed = 0.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float MaxAcceleration = 0.0f;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float JumpZVelocity = 0.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float GravityScale = 0.0f;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float BasicAirControl = 0.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float RepelResistPercent = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="CharacterAttribute")
	int32 BasicMaxJumpCount = 0;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="CharacterAttribute")
	float BasicJumpMaxHoldTime = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="CharacterAttribute")
	float BasicAttackInterval = 0.0f;
};

USTRUCT(BlueprintType)
struct FCharacterEstimate
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterEstimate")
	int Speed;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterEstimate")
	int Survival;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterEstimate")
	int Damage;
};

USTRUCT(BlueprintType)
struct FCharacterData : public FTableRowBase
{
	GENERATED_BODY()

	// 是否可用
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterData")
	bool Enable = true;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterData")
	FText CharacterName;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterData")
	FText CharacterDesc;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterData")
	TSubclassOf<ABasePXCharacter> Character_Class;

	// 战斗方式示意图
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterData")
	TSoftObjectPtr<UTexture2D> Image_FightWay;
	
	// 头像
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterData")
	TSoftObjectPtr<UTexture2D> Profile;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterData")
	FCharacterEstimate CharacterEstimate;
	
};
#pragma endregion


#pragma region FBuffOnWidget
USTRUCT(BlueprintType)
struct FBuffOnWidget : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="BuffOnWidget")
	FText BuffName;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="BuffOnWidget")
	FGameplayTag Tag;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="BuffOnWidget")
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="BuffOnWidget")
	bool Permanent = false;
	
};

#pragma endregion


#pragma region FTalent
USTRUCT(BlueprintType)
struct FTalent: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	bool DefaultLock = false;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	FGameplayTag TalentTag;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	FText TalentName;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	int Price = 3;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	FText TalentDesc;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	TMap<FGameplayTag, float> Effect_GameplayTag;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	TArray<TSoftClassPtr<UGameplayAbility>> AbilityClass;

	// 如果 CommonInit 为 true，则会初始化生成SkillClass和显示BuffTagOnWidget
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	bool CommonInit = true;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	TSubclassOf<ABaseSkill> SkillClass;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	FGameplayTag BuffTagOnWidget;
};
#pragma endregion

#pragma region FUIStruct
USTRUCT(BlueprintType)
struct FUIStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIStruct")
	FName  UIName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIStruct", meta = (Categories = "UI.Layer"))
	FGameplayTag LayerTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UIStruct")
	TSubclassOf<UUserWidget>  WidgetClass;

};
#pragma endregion


#pragma region FWeaponData

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	FGameplayTag WeaponTag;
		
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	TSubclassOf<class ABaseWeapon> SpawnItemClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	TSoftObjectPtr<USoundCue> UseSound;

	UPROPERTY(Config, EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> UseCameraShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	int Damage = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	float RepelPower = 50.f;
};
#pragma endregion



#pragma region FLevelData
USTRUCT(BlueprintType)
struct FLevelData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	TSoftObjectPtr<USoundWave> BGM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	FName LevelInstanceName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	FText LevelDisplayName;
};
#pragma endregion


#pragma region FNPCDialogues
USTRUCT(BlueprintType)
struct FNPCDialogues : public FTableRowBase
{
	GENERATED_BODY()

	// 一秒10个字符
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	float TalkSpeed = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	FText FirstTalk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	TArray<FText> RandomTalks;
};
#pragma endregion


USTRUCT(BlueprintType)
struct FLocalizedTableData: public FTableRowBase
{
	GENERATED_BODY()

};
