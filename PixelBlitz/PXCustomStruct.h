#pragma once

#include "CoreMinimal.h"
#include "Utilitys/CommonFuncLib.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "GAS/AttributeSet/PXAttributeSet.h"
#include "PXCustomStruct.generated.h"

class ABasePXCharacter;
class USoundCue;
class ABaseSkill;
class ABaseEnemy;

UCLASS()
class PIXELBLITZ_API UPXCustomStruct : public UObject
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

/** 弱点连击连音配置：达到 TriggerCombo 连击时，按顺序播放 NoteIndices 指定的音符 */
USTRUCT(BlueprintType)
struct FComboWeakPointRun
{
	GENERATED_BODY()

	/** 触发连击数（从 1 起算） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TriggerCombo = 0;

	/** 播放 SW_ComboWeakPoints 的下标序列，如 2,5,7 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> NoteIndices;

	/** 音量倍率 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float Volume = 1.0f;

	/** 连音间隔（秒），<=0 时使用 DA 的 ComboWeakPoint_RunNoteInterval */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "0.3"))
	float NoteInterval = 0.f;
};

USTRUCT(BlueprintType)
struct FDefaultEmptyMessage
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FEnemyMessage
{
	GENERATED_BODY()
	
	UPROPERTY()
	ABaseEnemy* Enemy = nullptr;
};


#pragma region UConfigDataAsset

UCLASS(Blueprintable)
class PIXELBLITZ_API UDevelopConfigDataAsset : public UDataAsset
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
	AttackSkillStart UMETA(DisplayName = "攻击预备/蓄力 阶段发起技能 开始"),
	AttackSkillEnd UMETA(DisplayName = "攻击预备/蓄力 阶段发起技能 结束"),
	AttackWeakPoint UMETA(DisplayName = "攻击/技能命中弱点"),
	SkillStart UMETA(DisplayName = "技能发起"),
	SkillHit UMETA(DisplayName = "技能命中"),
	SkillEnd UMETA(DisplayName = "技能结束"),
	SkillEndTrigger UMETA(DisplayName = "技能结束后短期内再次按下技能"),
	KillEnemy UMETA(DisplayName = "击杀小怪"),
	BeAttacked UMETA(DisplayName = "被攻击"),
	BeAttackedInvulnerable UMETA(DisplayName = "躲过攻击/伤害"),
	BeDamaged UMETA(DisplayName = "受到伤害"),
	JumpStart UMETA(DisplayName = "起跳"),
	Landing UMETA(DisplayName = "落地"),
	CrossPlatformLanding UMETA(DisplayName = "跨平台跳跃落地"),
	DefenseStart  UMETA(DisplayName = "防御开始"),
	DefenseSuccess  UMETA(DisplayName = "防御成功抵挡伤害"),
	DefenseFinish  UMETA(DisplayName = "防御结束"),
	Dying  UMETA(DisplayName = "死亡时"),
	PickGold  UMETA(DisplayName = "拾取金币时"),
	Interact  UMETA(DisplayName = "交互"),
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

	FAttributeEffect() = default;

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
	EPXAttribute EffectedAttribute = EPXAttribute::HP;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Buff")
	float EffectedPercent = 0.0f;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Buff")
	float EffectedValue = 1.0f;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Buff")
	float EffectedDuration = 99999.9;
	
	float EffectedEndTime = 1.0f;
	
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
	
	// 对角色属性的影响列表，含 目标属性、百分比、直接值、持续时间
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TArray<FAttributeEffect> AttributeEffectsOnActivated;
	
	// 实际所执行的技能
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TArray<TSoftClassPtr<UGameplayAbility>> AbilityClass;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TSubclassOf<ABaseSkill> SkillClass;
	
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


USTRUCT(BlueprintType)
struct FNPCData : public FTableRowBase
{
	GENERATED_BODY()
	
	// 对话参与者标签，同时也是NPC的ID，必须与 NPC 身上 Mountea Dialogue Participant 组件的 ParticipantTag 一致。
	// ❗ 用 HasTagExact 匹配对话图节点 tag，父子标签不算命中。
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="NPCData", meta=(Categories="Mountea_Dialogue"))
	FGameplayTag DialogueParticipantTag;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="NPCData")
	FText NPCName;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="NPCData")
	FText NPCDesc;
	
	// 对话立绘
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="NPCData")
	TObjectPtr<UTexture2D> Image_Dialogue;  
	
};


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
	
	// 是否初始化显示BuffTagOnWidget
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool InitBuffOnWidget = true;
	
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


// 分别包括 战斗竞速、竞技场
#pragma region FLevelData
USTRUCT(BlueprintType)
struct FLevelSeriesData : public FTableRowBase
{
	GENERATED_BODY()
	
	// 命名方式为：Race_01、Race_02、Arena_01、Arena_02
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelSeriesData)
	FName LevelSeriesID;
	
	// 直接加载表数据
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category=LevelSeriesData)
	FString LevelTableName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelSeriesData)
	FText LevelSeriesName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelSeriesData)
	FText LevelSeriesDesc;
	
	// 关卡预览图
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category= LevelSeriesData)
	TObjectPtr<UTexture2D> PreviewImage;
	
};
#pragma endregion


#pragma region FLevelData
USTRUCT(BlueprintType)
struct FLevelData : public FTableRowBase
{
	GENERATED_BODY()

	// Optional explicit sort key for run order. Falls back to level name when unset.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	int32 LevelOrder = INDEX_NONE;
	
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


#pragma region FGuidesWord
USTRUCT(BlueprintType)
struct FGuidesWord : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	FName GuideID = "";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	FString GuideTitle = TEXT("引导标题");
	
	//引导具体内容
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	FText GuideDesc;
};
#pragma endregion







USTRUCT(BlueprintType)
struct FLocalizedTableData: public FTableRowBase
{
	GENERATED_BODY()

};

#pragma region FUDSLevelLighting
/**
 * UDS 关卡光照参数表行（DataTable: LevelLighting）
 * 裁决：UDS 为项目唯一全局光照系统；曝光由关卡实例接管为 Manual；
 *       本表是每关 UDS 参数的唯一真相源，关卡实例上的 UDS 保持默认值。
 * 行名 = 关卡标识（与 LevelWeatherRate 等环境表对齐），如 L1_HomingMoon。
 * 应用方式：关卡加载后（UDS 初始化完成）由 BP 读取本行并逐项 Set 到 UDS actor。
 * 注意：UDS 部分属性为 Static Properties，运行时修改后需调用对应的
 *       Static Properties 函数才会生效（见 UltraDynamicSky/Blueprints/README.txt）。
 */
USTRUCT(BlueprintType)
struct FUDSLevelArgs : public FTableRowBase
{
	GENERATED_BODY()

	// 如 L1_HomingMoon 直接对照关卡文件名
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDSLevelLighting")
	FName LevelName;
	
	// 是否应用本行参数（false 时跳过该关卡，使用 UDS 默认）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDSLevelLighting")
	bool bApply = true;

	// 曝光 Metering Mode 是否切为 Manual（项目裁决：true；=false 保持 UDS 自动曝光）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDSLevelLighting")
	bool bManualExposure = true;

	// ---- Lighting Brightness（直射光/月光亮度） ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDSLevelLighting", meta = (DisplayName = "Lighting Brightness Day"))
	float LightingBrightnessDay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDSLevelLighting", meta = (DisplayName = "Lighting Brightness DawnDusk"))
	float LightingBrightnessDawnDusk = 20.f;

	// L1 校准值：18.0（Manual EV 下试出 15~20 区间定稿）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDSLevelLighting", meta = (DisplayName = "Lighting Brightness Night"))
	float LightingBrightnessNight = 18.f;

	// ---- 夜空环境亮度 ----
	// L1 校准值：1.2
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDSLevelLighting")
	float NightBrightness = 1.2f;

	// 多云时夜间额外亮度（L1 少云下为 0；若有乌云夜演出，该档需给值）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDSLevelLighting")
	float ExtraNightBrightnessWhenCloudy = 0.0f;

	// 阴天夜间亮度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDSLevelLighting")
	float OvercastBrightnessNight = 0.2f;

	// 太空层（星空）夜间亮度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDSLevelLighting")
	float SpaceLayerBrightnessNight = 1.0f;

	// 备注：校准说明/日期，不参与逻辑
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDSLevelLighting")
	FText Note;
};
#pragma endregion
