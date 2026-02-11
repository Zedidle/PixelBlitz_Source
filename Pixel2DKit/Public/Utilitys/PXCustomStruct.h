#pragma once

#include "CoreMinimal.h"
#include "CommonFuncLib.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "GAS/AttributeSet/PXAttributeSet.h"
#include "PXCustomStruct.generated.h"

class ABasePXCharacter;
class USoundCue;
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

#pragma region FBuffValueEffect

USTRUCT(BlueprintType)
struct FAttributeEffect
{
	GENERATED_BODY()
	FAttributeEffect()
	{
		
	}
	FAttributeEffect(EPXAttribute Attribute, float Percent): EffectedAttribute(Attribute),
			EffectedPercent(Percent), EffectedValue(1.0f), EffectedDuration(9999)
	{
		EffectedEndTime = UCommonFuncLib::CalEndTime(EffectedDuration);
	}
	FAttributeEffect(EPXAttribute Attribute, float Percent, float Value): EffectedAttribute(Attribute),
				EffectedPercent(Percent), EffectedValue(Value), EffectedDuration(9999)
	{
		EffectedEndTime = UCommonFuncLib::CalEndTime(EffectedDuration);
	}
	FAttributeEffect(EPXAttribute Attribute, float Percent, float Value, float Duration): EffectedAttribute(Attribute),
					EffectedPercent(Percent), EffectedValue(Value), EffectedDuration(Duration)
	{
		EffectedEndTime = UCommonFuncLib::CalEndTime(EffectedDuration);
	}

	void CalculateEndTime()
	{
		EffectedEndTime = UCommonFuncLib::CalEndTime(EffectedDuration);
	}
	
	float GetRemainingTime() const
	{
		UWorld* World = GEngine->GetCurrentPlayWorld();
		if (!World) return -1;
	
		return EffectedEndTime - World->GetTimeSeconds();
	}
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Buff")
	EPXAttribute EffectedAttribute;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Buff")
	float EffectedPercent = 0.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Buff")
	float EffectedValue = 1.0f;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Buff")
	float EffectedDuration = 9999;
	
	float EffectedEndTime = 1;
	
	FAttributeEffect& operator+=(const FAttributeEffect& Other)
	{
		// 检查属性是否匹配（重要：通常只合并相同属性的效果）
		if (this->EffectedAttribute == Other.EffectedAttribute)
		{
			this->EffectedPercent += Other.EffectedPercent;
			this->EffectedValue += Other.EffectedValue;
            
			// 持续时间处理：取最大值或相加，根据你的游戏逻辑决定
			this->EffectedDuration = FMath::Max(this->EffectedDuration, Other.EffectedDuration);
            
			// 结束时间重新计算
			this->EffectedEndTime = UCommonFuncLib::CalEndTime(this->EffectedDuration);
		}
		else
		{
			// 如果属性不匹配，可以选择抛出错误或采取其他处理方式
			UE_LOG(LogTemp, Warning, TEXT("尝试合并不同属性的FAttributeEffect"));
		}
        
		return *this;
	}
};

USTRUCT(BlueprintType)
struct FAttributeEffectArray
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Buff")
	TArray<FAttributeEffect> Data;
	
	FAttributeEffectArray() {}
    
	// 可以添加一些便捷方法
	void AddEffect(const FAttributeEffect& Effect)
	{
		Data.Add(Effect);
	}
    
	void RemoveEffectByAttribute(EPXAttribute Attribute)
	{
		Data.RemoveAll([Attribute](const FAttributeEffect& Effect) -> bool {
			return Effect.EffectedAttribute == Attribute;
		});
	}

	void CalculateEndTime()
	{
		for (auto& Effect : Data)
		{
			Effect.CalculateEndTime();
		}
	}
};

#pragma endregion 

#pragma region FAbility
USTRUCT(BlueprintType)
struct FAbility: public FTableRowBase
{
	GENERATED_BODY()

	// 是否已经禁用
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool Enable = true;

	// 是否默认未解锁
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool DefaultLock = false;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FGameplayTag AbilityTag;

	// 技能归属
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	EAbilityBelongTo AbilityBelongTo = EAbilityBelongTo::Common;
	
	// 技能名称本地化
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FText AbilityName;

	// 技能描述本地化
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FText AbilityDesc;
	
	// 费用，暂时都是1
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 Price = 1;

	// 技能品阶
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	EAbilityQuality AbilityQuality = EAbilityQuality::Level1;

	// 唯一的技能的前一级，学习后会替换
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FGameplayTag PreLevelAbility;
	
	// 前置所需学习的技能 与 前置选择的天赋 GameplayTag
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TArray<FGameplayTag> RequiredAbilities;
	
	// 技能的特殊附加值，以GameplayTag为前缀
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TMap<FGameplayTag, float> ExtendData;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TArray<FAttributeEffect> AttributeEffectsOnActivated;
	
	// 实际所执行的技能
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TArray<TSoftClassPtr<UGameplayAbility>> AbilityClass;

	// 技能触发时机，如果有 AbilityClass 的话，需要配置
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	EAbilityTiming Timing = EAbilityTiming::None;
};


#pragma endregion






#pragma region Character Data

// 不能写默认参数，因为和关卡继承角色属性通用
USTRUCT(BlueprintType)
struct FCharacterAttribute
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	int BasicMaxHP = 0;
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	int BasicHPRecoverValue = 0;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	int BasicMaxEP = 0;
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	int BasicEPRecoverValue = 0;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float BasicSpeed = 0.0f;
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float BasicAcceleration = 0.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float BasicDashSpeed = 0.0f;
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float BasicDashCD = 0.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float BasicSight = 0.0f;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	int BasicAttackValue = 0;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	int BasicRepelValue = 0;
	

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float BasicJumpSpeed = 0.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float BasicGravityScale = 0.0f;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float BasicAirControl = 0.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="CharacterAttribute")
	float BasicRepelResist = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="CharacterAttribute")
	int32 BasicMaxJumpCount = 0;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="CharacterAttribute")
	float BasicJumpMaxHoldTime = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="CharacterAttribute")
	float BasicAttackCD = 0.0f;
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
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool DefaultLock = false;

	// 天赋Tag，用于显示BUFF与触发
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FGameplayTag TalentTag;

	// 选择时所需天赋点
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int Price = 3;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	ETalentType TalentType = ETalentType::None;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FText TalentName;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FText TalentDesc;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TMap<FGameplayTag, float> ExtendData;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TArray<FAttributeEffect> AttributeEffectOnActivated;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TArray<TSoftClassPtr<UGameplayAbility>> AbilityClass;

	// 是否初始化生成SkillClass和显示BuffTagOnWidget
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool CommonInit = true;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TSubclassOf<ABaseSkill> SkillClass;

	// 技能触发时机，同时触发 SkillActor 和 Ability
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
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
