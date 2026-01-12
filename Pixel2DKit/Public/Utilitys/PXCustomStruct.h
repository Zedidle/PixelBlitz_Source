// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "PXCustomStruct.generated.h"

class USoundCue;
class ABasePXCharacter;
class ABaseSkill;

UCLASS()
class PIXEL2DKIT_API UPXCustomStruct : public UObject
{
	GENERATED_BODY()
	
};

USTRUCT(BlueprintType)
struct FNameArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Names;
};

USTRUCT(BlueprintType)
struct FGameplayTagArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> Tags;
};

USTRUCT(BlueprintType)
struct FDefaultEmptyMessage
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


#pragma region EAttackType
UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None UMETA(DisplayName = "无"),
	Melee UMETA(DisplayName = "近战 挥砍/撞击"),
	Projectile UMETA(DisplayName = "远战 投射物"),
	Spell  UMETA(DisplayName = "法术"),
};
#pragma endregion

#pragma region ETalentType 天赋分类
UENUM(BlueprintType)
enum class ETalentType : uint8
{
	None UMETA(DisplayName = "无"),
	Attack UMETA(DisplayName = "进攻类"),
	Survive UMETA(DisplayName = "生存类"),
	Moving UMETA(DisplayName = "移动类"),
	Agility UMETA(DisplayName = "技巧类"),
	Special UMETA(DisplayName = "特殊"),
};
#pragma endregion

#pragma region ETalentType
UENUM(BlueprintType)
enum class EAbilityBelongTo : uint8
{
	None UMETA(DisplayName = "无"),
	Common UMETA(DisplayName = "公用"),
	Saber UMETA(DisplayName = "剑客"),
	Archer UMETA(DisplayName = "弓箭手"),
};
#pragma endregion

#pragma region EAbilityTiming
UENUM(BlueprintType)
enum class EAbilityTiming : uint8
{
	None UMETA(DisplayName = "直接触发/特殊处理"),
	AttackStart UMETA(DisplayName = "攻击开始"),
	AttackEffect UMETA(DisplayName = "攻击生效"),
	AttackHit UMETA(DisplayName = "攻击命中"),
	AttackFinish UMETA(DisplayName = "攻击结束"),
	AttackSkill UMETA(DisplayName = "攻击预备/蓄力 阶段发起技能"),
	AttackWeakPoint UMETA(DisplayName = "攻击/技能命中弱点"),
	SkillStart UMETA(DisplayName = "技能发起"),
	SkillHit UMETA(DisplayName = "技能命中"),
	SkillFinish UMETA(DisplayName = "技能结束"),
	KillEnemy UMETA(DisplayName = "击杀小怪"),
	BeAttacked UMETA(DisplayName = "被攻击"),
	BeAttackedInvulnerable UMETA(DisplayName = "躲过攻击/伤害"),
	BeDamaged UMETA(DisplayName = "受到伤害"),
	JumpStart UMETA(DisplayName = "起跳"),
	Landing UMETA(DisplayName = "落地"),
	DefenseStart  UMETA(DisplayName = "防御开始"),
	DefenseSuccess  UMETA(DisplayName = "防御成功抵挡伤害"),
	DefenseFinish  UMETA(DisplayName = "防御结束"),
	Dying  UMETA(DisplayName = "死亡时"),
	PickGold  UMETA(DisplayName = "拾取金币时"),
	Loop  UMETA(DisplayName = "周期性持续（可不配置）"),
};
#pragma endregion


#pragma region EWeather
UENUM(BlueprintType, meta = (ScriptName = "GameStatusWeather"))
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
	Old UMETA(DisplayName = "黑白"),
	MAX UMETA(Hidden),
};
#pragma endregion


#pragma region EStyleType
UENUM(BlueprintType)
enum class ECameraColorFilter : uint8
{
	None UMETA(DisplayName = "无"),
	SkyBlue UMETA(DisplayName = "天青"),
	Retro UMETA(DisplayName = "怀旧"),
	Pink UMETA(DisplayName = "粉"),
	MoonlitNight UMETA(DisplayName = "清辉夜凝"),
	GrayWhite UMETA(DisplayName = "灰白"),
	Ocean UMETA(DisplayName = "海水"),
	Bright UMETA(DisplayName = "明亮"),
	Twilight UMETA(DisplayName = "黄昏"),
	Dawn UMETA(DisplayName = "清晨"),
	FairyTale UMETA(DisplayName = "童话"),
	Soft UMETA(DisplayName = "柔和"),
	MAX UMETA(Hidden),
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

	// 技能归属
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	EAbilityBelongTo AbilityBelongTo = EAbilityBelongTo::Common;
	
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

	// 实际所执行的技能
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	TArray<TSoftClassPtr<UGameplayAbility>> AbilityClass;

	// 技能触发时机，如果有 AbilityClass 的话，需要配置
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	EAbilityTiming Timing = EAbilityTiming::None;
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
	int Speed = 6;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterEstimate")
	int Survival = 6;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterEstimate")
	int Damage = 6;
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

	// 是否默认未解锁
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	bool DefaultLock = false;

	// 天赋Tag，用于显示BUFF与触发
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	FGameplayTag TalentTag;

	// 选择时所需天赋点
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	int Price = 3;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	ETalentType TalentType = ETalentType::None;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	FText TalentName;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	FText TalentDesc;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	TSoftObjectPtr<UTexture2D> Icon;

	// 相关属性，运行时记录在角色中统一处理
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Talent")
	TMap<FGameplayTag, float> Effect_GameplayTag;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	TArray<TSoftClassPtr<UGameplayAbility>> AbilityClass;

	// 如果 CommonInit 为 true，则会初始化生成SkillClass和显示BuffTagOnWidget
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	bool CommonInit = true;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	TSubclassOf<ABaseSkill> SkillClass;

	// 技能触发时机，同时触发 SkillActor 和 Ability
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Ability")
	EAbilityTiming Timing = EAbilityTiming::None;
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
