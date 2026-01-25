#include "Character/Components/AbilityComponent.h"
#include "AbilitySystemComponent.h"
#include "Character/BasePXCharacter.h"
#include "Character/PXCharacterDataAsset.h"
#include "Character/Components/BuffComponent.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Enemy/BaseEnemy.h"
#include "Fight/Components/StateComponent.h"
#include "Fight/Skills/BaseDefenseSkill.h"
#include "Fight/Skills/BaseSkill.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "Utilitys/PXCustomStruct.h"



#define LOCTEXT_NAMESPACE "PX"




// Sets default values for this component's properties
UAbilityComponent::UAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}



FGameplayTagContainer UAbilityComponent::CreateGameplayTagContainer(FName TagName, bool WithChildren)
{
	if (WithChildren)
	{
		FGameplayTagContainer AbilityTagsChildren = UGameplayTagsManager::Get().
			RequestGameplayTagChildren(TAG(TagName));

		AbilityTagsChildren.AddTag(TAG(TagName));
		return AbilityTagsChildren;
	}

	return FGameplayTagContainer(TAG(TagName));
}

// Called when the game starts
void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner);

	PXCharacter = Cast<ABasePXCharacter>(Owner);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);
	OwnerPreLocation = PXCharacter->GetActorLocation();
	
	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)

	UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem)

	UPXMainSaveGame* MainSave = SaveGameSubsystem->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSave)

	MainSave->TakeEffectAbilities.Append(AbilitiesDefaultOwn);
	
	if (UPXASComponent* PXASComponent = Cast<UPXASComponent>(PXCharacter->GetAbilitySystemComponent()))
	{
		CachedASC = PXASComponent;
	}
	
	// 对 OnPlayerAttack 的绑定，在玩家发起攻击时的事件
	PXCharacter->OnPlayerAttackStart.AddDynamic(this, &ThisClass::OnAttackStart);
}

void UAbilityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);

	UTimerSubsystemFuncLib::CancelDelay(GetWorld(), "Ability.WarmUP");
	
	PXCharacter->OnPlayerAttackStart.RemoveDynamic(this, &ThisClass::OnAttackStart);
}


// Called every frame
void UAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAbilityComponent::RefreshAbilitiesCanChoice()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->DataAsset)

	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem);
	
	AbilitiesCanChoice.Empty();

	for (auto& Table: PXCharacter->DataAsset->AbilityDataTables)
	{
		UDataTable* AbilityTable = Table.LoadSynchronous();
		if (!AbilityTable) continue;

		TArray<FAbility> Abilities = DataTableSubsystem->GetRowMap<FAbility>(AbilityTable);

		for (auto& Ability: Abilities)
		{
			if (CanLearnAbility(Ability))
			{
				AbilitiesCanChoice.Add(Ability.AbilityTag, Ability);
			}
		}
	}
}

void UAbilityComponent::LoadTalents()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->BuffComponent)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->StateComponent)
	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)
	UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem)
	UPXBasicBuildSaveGame* BasicBuildSaveGame = SaveGameSubsystem->GetBasicBuildData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BasicBuildSaveGame)

	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)
	
	if (TalentLoaded) return;
	TalentLoaded = true;
	
	// 热身
	FGameplayTag Tag = TAG("Ability.Warmup.Set.AttackDamagePlusPercent");
	if (AbilityExtendData.Contains(Tag))
	{
		UTimerSubsystemFuncLib::SetDelayLoopSafe(GetWorld(), "Ability.WarmUP",
			this, &ThisClass::MoveWarmingUP, 0.2);
	}
	
	// 奇迹行者
	MakeMiracleWalker();
	// 不灭
	MakeImmortalPower(true);
}

void UAbilityComponent::MoveWarmingUP()
{
	if (WarmUP_Power >= 4) return;
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->BuffComponent);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC);
	
	WarmUP_MoveDistance += FVector::Distance(PXCharacter->GetActorLocation(), OwnerPreLocation);
	OwnerPreLocation = PXCharacter->GetActorLocation();

	FGameplayTag MoveDistancePerLevelTag = TAG("Ability.Warmup.Set.MoveDistancePerLevel");
	FGameplayTag AttackDamagePlusPercentTag = TAG("Ability.Warmup.Set.AttackDamagePlusPercent");
	
	
	if (!AbilityExtendData.Contains(MoveDistancePerLevelTag) ||
		!AbilityExtendData.Contains(AttackDamagePlusPercentTag)
	) return;

	if (WarmUP_MoveDistance < AbilityExtendData[MoveDistancePerLevelTag]) return;

	WarmUP_MoveDistance -= AbilityExtendData[MoveDistancePerLevelTag];
	WarmUP_Power ++;

	float PlusPowerPercent = WarmUP_Power * AbilityExtendData[AttackDamagePlusPercentTag];
	FGameplayTag WarmUP_Tag = TAG("Ability.Warmup");
	FText BuffNameFormat = LOCTEXT("Buff_Warmup", "热身{0}");

	PXCharacter->BuffComponent->AddAttributeEffect(WarmUP_Tag,
		FAttributeEffect(EPXAttribute::CurAttackValue, PlusPowerPercent));

	PXCharacter->BuffComponent->AddBuffOnWidget(WarmUP_Tag, FText::Format(BuffNameFormat, WarmUP_Power).ToString(),
	FLinearColor( 0.25 * WarmUP_Power, 0.1, 0.1, 1.0), false);
}

void UAbilityComponent::MakeMiracleWalker()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	
	FGameplayTag MiracleWalkerTag = TAG("Ability.MiracleWalker");
	// IBuff_Interface::Execute_RemoveBuff(PXCharacter, MiracleWalkerTag, true);
	// RemoveAttributeEffectsByTag
	
	FGameplayTag DamagePlusTag = TAG("Ability.MiracleWalker.Set.DamagePlus");
	FGameplayTag IntervalTag = TAG("Ability.MiracleWalker.Set.Interval");

	if (!AbilityExtendData.Contains(DamagePlusTag) || !AbilityExtendData.Contains(IntervalTag)) return;

	FName TimerName = FName(GetReadableName() + "_MakeMiracleWalker");
	UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), TimerName,
		[WeakThis = TWeakObjectPtr(this), DamagePlusTag, MiracleWalkerTag]
		{
			if (!WeakThis.IsValid()) return;
			if (!WeakThis->PXCharacter) return;

			FEffectGameplayTags& EffectGameplayTags = WeakThis->AbilityExtendData;
			if (EffectGameplayTags.Contains(DamagePlusTag))
			{
				if (WeakThis->PXCharacter->BuffComponent)
				{
					WeakThis->PXCharacter->BuffComponent->AddAttributeEffect( MiracleWalkerTag,
						FAttributeEffect(EPXAttribute::CurAttackValue, 0.0, EffectGameplayTags[DamagePlusTag])
					);
					WeakThis->PXCharacter->BuffComponent->AddBuffByTag(MiracleWalkerTag);
				}
			}
		}, AbilityExtendData[IntervalTag]);
}

void UAbilityComponent::MakeImmortalPower(bool First)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->BuffComponent)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->StateComponent)
	
	FGameplayTag AttackDamagePlusOnMaxHPPercentTag = TAG("Ability.Immortal.Set.AttackDamagePlusOnMaxHPPercent");
	FGameplayTag IntervalTag = TAG("Ability.Immortal.Set.Interval");
	FGameplayTag MaxHPPlusAfterAttackTag = TAG("Ability.Immortal.Set.MaxHPPlusAfterAttack");
	
	if (!AbilityExtendData.Contains(AttackDamagePlusOnMaxHPPercentTag) || 
		!AbilityExtendData.Contains(IntervalTag) ||
		!AbilityExtendData.Contains(MaxHPPlusAfterAttackTag)
	) return;
	FGameplayTag ImmortalPowerTag = TAG("Ability.Immortal");

	if (First || PXCharacter->BuffComponent->BuffExist(ImmortalPowerTag))
	{
		UTimerSubsystemFuncLib::SetDelay(GetWorld(),
	[WeakThis = TWeakObjectPtr(this), AttackDamagePlusOnMaxHPPercentTag, ImmortalPowerTag]
		{
			if (!WeakThis.IsValid()) return;
			if (!WeakThis->PXCharacter) return;
			if (!WeakThis->PXCharacter->StateComponent) return;
			if (!WeakThis->PXCharacter->BuffComponent) return;
		
			FEffectGameplayTags& EffectGameplayTags = WeakThis->AbilityExtendData;
			if (!EffectGameplayTags.Contains(AttackDamagePlusOnMaxHPPercentTag)) return;
		
			WeakThis->PXCharacter->BuffComponent->AddAttributeEffect( ImmortalPowerTag,
				FAttributeEffect(EPXAttribute::CurAttackValue, 0.0f,
					FMath::RoundToInt(WeakThis->PXCharacter->StateComponent->GetMaxHP() * EffectGameplayTags[AttackDamagePlusOnMaxHPPercentTag]))
			);

			if (WeakThis->PXCharacter->BuffComponent)
			{
				WeakThis->PXCharacter->BuffComponent->AddBuffByTag(ImmortalPowerTag);
			}

			// 不灭中，恢复生命值的部分
			// UGameInstance* GameInstance = WeakThis->PXCharacter->GetGameInstance();
			// CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);
			// UPXMainSaveGame* MainSaveGame= UPXSaveGameSubSystemFuncLib::GetMainData(GameInstance->GetWorld());
			// MainSaveGame->CharacterInheritAttribute.BasicMaxHP ++;
		}, AbilityExtendData[IntervalTag]);
	}

	if (!First && PXCharacter->BuffComponent->BuffExist(ImmortalPowerTag))
	{
		// IBuff_Interface::Execute_RemoveBuff(PXCharacter, ImmortalPowerTag, true);
		PXCharacter->StateComponent->ModifyMaxHP(1, EStatChange::Increase, true);
	}
}

void UAbilityComponent::InitTalents()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->BuffComponent)
	UPXBasicBuildSaveGame* BasicBuildSaveGame = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BasicBuildSaveGame)

	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem);
	
	for (const FGameplayTag& TalentTag: BasicBuildSaveGame->ChosenTalents)
	{
		// 获取对应的Talent结构
		const FTalent& TalentData = DataTableSubsystem->GetTalentDataByTag(TalentTag);
		if (!TalentData.TalentTag.IsValid()) continue;

		for (auto& D: TalentData.ExtendData)
		{
			AbilityExtendData.AddData(D.Key, D.Value);
		}

		FAttributeEffectArray& AttributeEffects = AbilityAttributeEffects.FindOrAdd(TalentData.TalentTag);
		for (auto& D: TalentData.AttributeEffectOnActivated)
		{
			AttributeEffects.AddEffect(D);
		}
		
		for (auto& AbilityClass: TalentData.AbilityClass)
		{
			if (UClass* LoadedClass = AbilityClass.LoadSynchronous())
			{
				FGameplayAbilitySpec Spec(LoadedClass);
				CachedASC->GiveAbility(Spec);
			}
		}
		
		if (TalentData.CommonInit)
		{
			if (ABaseSkill* Skill = SpawnSkill(TalentData.SkillClass))
			{
				Skill->SetActivateTiming(TalentData.Timing);
				Skill->AbilityTags.AddTag(TalentData.TalentTag);
				SkillsHolding.Add(Skill);
			}
			if (PXCharacter->BuffComponent)
			{
				PXCharacter->BuffComponent->AddBuffByTag(TalentData.TalentTag, true);
			}
		}

		
		if (TalentData.Timing != EAbilityTiming::None)
		{
			FGameplayTagArray& TagArray = AbilitiesTiming.FindOrAdd(TalentData.Timing);
			TagArray.Tags.Add(TalentData.TalentTag);
		}
		else
		{
			PXCharacter->BuffComponent->AddAttributeEffects(TalentData.TalentTag, TalentData.AttributeEffectOnActivated);
		}
	}
}

void UAbilityComponent::InitAbilities()
{
	InitTalents();
	
	// 天赋需要在此之前加载完成
	float GoldenBlessRadioPlus;
	if (FindExtendData(TAG("Ability.GoldenBless.Set.RatePlus"), GoldenBlessRadioPlus))
	{
		GoldenRadio += GoldenBlessRadioPlus;
	}

	float LegendBlessRadioPlus;
	if (FindExtendData(TAG("Ability.LegendBless.Set.RatePlus"), LegendBlessRadioPlus))
	{
		GoldenRadio += LegendBlessRadioPlus;
		LegendRadio += LegendBlessRadioPlus;
	}
}

ABaseSkill* UAbilityComponent::SpawnSkill(TSubclassOf<ABaseSkill> SkillClass, const FTransform& SpawnTransform)
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;
	if (!SkillClass) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PXCharacter;
	SpawnParams.Instigator = PXCharacter;
	
	if (ABaseSkill* Skill = World->SpawnActor<ABaseSkill>(SkillClass, SpawnTransform, SpawnParams))
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
		Skill->AttachToActor(PXCharacter, AttachmentRules);
		
		if (ABaseDefenseSkill* DefenseSkill = Cast<ABaseDefenseSkill>(Skill))
		{
			RegisterDefenseSkill(DefenseSkill);
		}
		
		return Skill;
	}
	return nullptr;
}

void UAbilityComponent::RegisterDefenseSkill(ABaseDefenseSkill* Skill)
{
	DefenseSkills.Add(Skill);
	int Index = DefenseSkills.Num() - 1;
	while (Index > 0)
	{
		if (IsValid(DefenseSkills[Index]) && IsValid(DefenseSkills[Index - 1]))
		{
			if (DefenseSkills[Index]->Priority > DefenseSkills[Index - 1]->Priority)
			{
				DefenseSkills.Swap(Index, Index - 1);
				Index --;
				continue;
			}
		}
		break;
	}
}

void UAbilityComponent::RemoveDefenseSkill(const FGameplayTag& Tag)
{
	DefenseSkills.RemoveAllSwap([&Tag](const ABaseDefenseSkill* Skill)
	{
		return Skill && Skill->AbilityTags.HasTag(Tag);
	}, EAllowShrinking::No);
}

FAbility UAbilityComponent::GetAbilityToLearn()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(PXCharacter, FAbility())

	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GameInstance, FAbility())
	
	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(DataTableSubsystem, FAbility())
	
	UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(SaveGameSubsystem, FAbility())
	
	UPXMainSaveGame* MainSave = SaveGameSubsystem->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(MainSave, FAbility())

	TArray<FGameplayTag> Tags;
	AbilitiesCanChoice.GetKeys(Tags);

	for (auto& TagShowing: AbilitiesShowing)
	{
		Tags.Remove(TagShowing);
	}
	
	if (Tags.Num() <= 0) return FAbility();

	int LoopTimes = 10;
	
	while (--LoopTimes > 0)
	{
		float RandRadio = FMath::RandRange(0.0f, 1.0f);
		EAbilityQuality Quality;
		if (RandRadio < LegendRadio)
		{
			Quality = EAbilityQuality::Level3;
		}
		else if (RandRadio < GoldenRadio)
		{
			Quality = EAbilityQuality::Level2;
		}
		else
		{
			Quality = EAbilityQuality::Level1;
		}

		const FGameplayTag& MaybeTag = Tags[FMath::RandRange(0, Tags.Num() - 1)];

		FAbility AbilityData = AbilitiesCanChoice.FindRef(MaybeTag);

		if (AbilityData.AbilityQuality == Quality)
		{
			return DataTableSubsystem->GetAbilityDataByTag(MaybeTag);
		}
	}

	FGameplayTag RandTag = Tags[FMath::RandRange(0, Tags.Num() - 1)];
	return DataTableSubsystem->GetAbilityDataByTag(RandTag);
}

void UAbilityComponent::LearnAbility(const FGameplayTag& AbilityTag)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem);

	UPXMainSaveGame* MainSave = GameInstance->GetSubsystem<UPXSaveGameSubsystem>()->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSave);

	AbilitiesShowing.Remove(AbilityTag);
	AbilitiesCanChoice.Remove(AbilityTag);
	MainSave->ChosenAbilities.Add(AbilityTag);
	
	const FAbility& AbilityData = DataTableSubsystem->GetAbilityDataByTag(AbilityTag);

	if (!AbilityData.PreLevelAbility.IsValid())
	{
		MainSave->TakeEffectAbilities.AddUnique(AbilityTag);
		// 由于同一技能的不同等级都使用相同的GA，只是数值不同，就只用在学习第一个技能是GiveAbility
		for (auto& AbilityClass: AbilityData.AbilityClass)
		{
			if (UClass* LoadedClass = AbilityClass.LoadSynchronous())
			{
				FGameplayAbilitySpec Spec(LoadedClass);
				CachedASC->GiveAbility(Spec);
			}
		}
	}
	else if (MainSave->TakeEffectAbilities.Contains(AbilityData.PreLevelAbility))
	{
		MainSave->TakeEffectAbilities.Remove(AbilityData.PreLevelAbility);
		MainSave->TakeEffectAbilities.AddUnique(AbilityTag);
	}
}

void UAbilityComponent::LoadAbilities()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->BuffComponent);
	
	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)
	
	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem);

	UPXMainSaveGame* MainSave = GameInstance->GetSubsystem<UPXSaveGameSubsystem>()->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSave);
	
	for (auto Tag : MainSave->TakeEffectAbilities)
	{
		FString Parent;
		FString Level;
		Tag.ToString().Split(".Level", &Parent, &Level);

		FGameplayTag AbilityTag = TAG(*Parent);
		
		if (AbilityExtendData.Contains(AbilityTag)) continue;
		AbilityExtendData.SetData(AbilityTag, 1);
		
		const FAbility& AbilityData = DataTableSubsystem->GetAbilityDataByTag(Tag);
		if (!AbilityData.AbilityTag.IsValid()) continue;

		for (auto& AbilityClass: AbilityData.AbilityClass)
		{
			if (UClass* LoadedClass = AbilityClass.LoadSynchronous())
			{
				FGameplayAbilitySpec Spec(LoadedClass);
				CachedASC->GiveAbility(Spec);
			}
		}
		
		for (auto& D : AbilityData.ExtendData)
		{
			AbilityExtendData.SetData(D.Key, D.Value);
		}

		FAttributeEffectArray& AttributeEffects = AbilityAttributeEffects.FindOrAdd(AbilityData.AbilityTag);
		for (auto& D: AbilityData.AttributeEffectsOnActivated)
		{
			AttributeEffects.AddEffect(D);
		}
		
		PXCharacter->BuffComponent->AddBuffByTag(TAG(*Parent));
		
		if (AbilityData.Timing != EAbilityTiming::None)
		{
			FGameplayTagArray& TagArray = AbilitiesTiming.FindOrAdd(AbilityData.Timing);
			TagArray.Tags.Add(TAG(*Parent));
		}
		else
		{
			PXCharacter->BuffComponent->AddAttributeEffects(AbilityData.AbilityTag, AbilityData.AttributeEffectsOnActivated);
		}
	}

	UStateComponent* StateComponent = PXCharacter->GetComponentByClass<UStateComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(StateComponent)
	UBuffComponent* BuffComponent = PXCharacter->GetComponentByClass<UBuffComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffComponent)


# pragma region 通用技能加载部分，专属技能在 PXCharacter 子类中的LoadAbility自定义

	const auto& EffectTags = AbilityExtendData.GetAllKeys();
	for (auto& T : EffectTags)
	{
		FString _, AttributeName;
		T.ToString().Split("BasicAttributeSet.", &_, &AttributeName);
	
		if (AttributeName.IsEmpty()) continue;
	
		CachedASC->ModifyAttributeValue(AttributeName, AbilityExtendData[T]);
	}
	
	
#pragma endregion 
	
	
	if (PXCharacter)
	{
		PXCharacter->LoadAbility();
	}
	
	UPXSaveGameSubSystemFuncLib::SaveMainData(this);
}

bool UAbilityComponent::HasChosenAbility(const FGameplayTag& AbilityTag)
{
	if (UPXMainSaveGame* MainSave = UPXSaveGameSubSystemFuncLib::GetMainData(this))
	{
		return MainSave->ChosenAbilities.Contains(AbilityTag);
	}
	
	return false;
}

bool UAbilityComponent::CanLearnAbility(const FAbility& Ability)
{
	if (!Ability.Enable) return false;
	const FGameplayTag& AbilityTag = Ability.AbilityTag;
	if (!AbilityTag.IsValid()) return false;

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(PXCharacter, false)

	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GameInstance, false)

	UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(SaveGameSubsystem, false)

	UPXMainSaveGame* MainSave = SaveGameSubsystem->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(MainSave, false)
	
	if (MainSave->TakeEffectAbilities.Contains(AbilityTag)) return false;

	UPXBasicBuildSaveGame* BasicBuildSave = SaveGameSubsystem->GetBasicBuildData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(BasicBuildSave, false)

	if (Ability.DefaultLock && !BasicBuildSave->UnlockedAbilities.Contains(AbilityTag)) return false;

	if (Ability.PreLevelAbility.IsValid() && !MainSave->TakeEffectAbilities.Contains(Ability.PreLevelAbility)) return false;
	
	if (!Ability.RequiredAbilities.IsEmpty())
	{
		for (auto& RequiredTag : Ability.RequiredAbilities)
		{
			if (!MainSave->ChosenAbilities.Contains(RequiredTag) && !BasicBuildSave->ChosenTalents.Contains(RequiredTag)) return false;
		}
	}

	return true;
}

void UAbilityComponent::OnItemRefresh(const FGameplayTag& NewTag, const FGameplayTag& PreShowingAbility, int& RemRefreshPoints)
{
	if (!NewTag.IsValid()) return;

	AbilitiesShowing.Add(NewTag);
	
	UPXMainSaveGame* MainSave = UPXSaveGameSubSystemFuncLib::GetMainData(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSave)

	if (PreShowingAbility.IsValid())
	{
		MainSave->RemRefreshPoints--;
		AbilitiesShowing.Remove(PreShowingAbility);
	}

	RemRefreshPoints = MainSave->RemRefreshPoints;
}

bool UAbilityComponent::ChoiceAbility(const FGameplayTag& Tag, int& RemSkillPoints)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(PXCharacter, false)
	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GameInstance, false)

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(DataTableSubsystem, false)

	UPXMainSaveGame* MainSave = UPXSaveGameSubSystemFuncLib::GetMainData(this);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(MainSave, false)
	
	const FAbility& AbilityData = DataTableSubsystem->GetAbilityDataByTag(Tag);

	if (MainSave->RemSkillPoints < AbilityData.Price) return false;

	MainSave->RemSkillPoints -= AbilityData.Price;
	RemSkillPoints = MainSave->RemSkillPoints;

	LearnAbility(Tag);
		
	return true;
}



void UAbilityComponent::OnLanding()
{
	ActivateAbilityByTiming(EAbilityTiming::Landing);
}

void UAbilityComponent::OnSkillStart()
{
	ActivateAbilityByTiming(EAbilityTiming::SkillStart);
}

int UAbilityComponent::GetAttackDamagePlus()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(PXCharacter, 0)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(PXCharacter->StateComponent, 0)

	int LocalPlus = 0;
	float FoundR;
	if (FindExtendData(TAG("Ability.Wushu.Set.AttackDamagePlusOnCurHPPercent"), FoundR))
	{
		LocalPlus += FMath::RoundToInt(FoundR * PXCharacter->StateComponent->GetCurrentHP()) ;
	}

	// …… 其它技能
	return LocalPlus;
}

void UAbilityComponent::OnAttackStart(EAttackType Type, FVector Direction)
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnAttackStart();
		}
	}
	
	ActivateAbilityByTiming(EAbilityTiming::AttackStart);
}


void UAbilityComponent::OnAttackDash()
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnAttackDash();
		}
	}
	ActivateAbilityByTiming(EAbilityTiming::AttackSkill);
}

void UAbilityComponent::OnKillEnemy()
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnKillEnemy();
		}
	}
	ActivateAbilityByTiming(EAbilityTiming::KillEnemy);
}

void UAbilityComponent::OnBuffCalDamage()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	
	// 清除热身运动
	if (WarmUP_Power > 0)
	{
		// IBuff_Interface::Execute_RemoveBuff(PXCharacter, TAG("Ability.Warmup"), true);
		WarmUP_Power = 0 ;
		WarmUP_MoveDistance = 0;
		OwnerPreLocation = PXCharacter->GetActorLocation();
	}

	MakeMiracleWalker();
	MakeImmortalPower(false);

	// IBuff_Interface::Execute_RemoveBuff(PXCharacter, TAG("Ability.StaticPower"), true);
	// IBuff_Interface::Execute_RemoveBuff(PXCharacter, TAG("Ability.EvadeStrike"), true);
}

void UAbilityComponent::OnDying(int& RemReviveTimes)
{
	int MaxReviveTimes = -1;
	for (auto& Skill : SkillsHolding)
	{
		int TmpReviveTimes = -1;
		Skill->OnDying(TmpReviveTimes);
		if (MaxReviveTimes < TmpReviveTimes)
		{
			MaxReviveTimes = TmpReviveTimes;
		}
	}
	RemReviveTimes = MaxReviveTimes;

	ActivateAbilityByTiming(EAbilityTiming::Dying);
}

void UAbilityComponent::ActivateAbilityByTiming(EAbilityTiming Timing)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	
	FGameplayTagArray TagArray = AbilitiesTiming.FindRef(Timing);
	for (auto& Tag : TagArray.Tags)
	{
		CachedASC->TryActivateAbilityByTag(Tag);
	}
}

void UAbilityComponent::CreateQTE(float _SustainTime, float _Scale)
{
	OnHurtInstigatorDead(nullptr);
	if (!HurtMaker || !HurtMaker->Implements<UFight_Interface>()
		|| !IFight_Interface::Execute_IsAlive(HurtMaker)) return;
	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(KeyPressCountdownWidgetClass)
	KeyPressCountDownWidget = CreateWidget<UKeyPressCountDownWidget>(GetWorld(), KeyPressCountdownWidgetClass);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(KeyPressCountDownWidget)
	KeyPressCountDownWidget->InitializeData(_SustainTime, _Scale);
	KeyPressCountDownWidget->AddToViewport(100);

	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ArrowLineWidgetClass)
	ArrowLineWidget = CreateWidget<UArrowLineWidget>(GetWorld(), ArrowLineWidgetClass);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ArrowLineWidget)
	ArrowLineWidget->InitializeData(PXCharacter, HurtMaker, true, 10,
		600, FLinearColor::White, _SustainTime, FVector2D(36, 54),
		FVector2D(64, 64), FVector::Zero());
	ArrowLineWidget->SetRenderScale(FVector2D(_Scale));
	ArrowLineWidget->AddToViewport(100);
}

void UAbilityComponent::OnInteract(bool& Keep)
{
	Keep = true;
	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->DataAsset)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->DataAsset->InputConfig)

	const auto& ActionMap = PXCharacter->DataAsset->InputConfig->ActionMap;
	FGameplayTag Tag = TAG("InputAction.Interact");
	if (!ActionMap.Contains(Tag))
	{
		Keep = true;
		return;
	}
	
	if (!IsValid(KeyPressCountDownWidget))
	{
		Keep = true;
		return;
	}

	bool Effect;
	KeyPressCountDownWidget->OnPressed(Effect);

	if (!Effect)
	{
		Keep = true;
		return;
	}

	if (ArrowLineWidget)
	{
		ArrowLineWidget->RemoveArrowLine();
	}
	
	if (CachedASC)
	{
		if (CachedASC->TryActivateAbilityByTagName("Ability.SkyHandPower"))
		{
			Keep = false;
		}
	}
}

void UAbilityComponent::OnHurtInstigatorDead(ABaseEnemy* DeadEnemy)
{
	if (KeyPressCountDownWidget)
	{
		KeyPressCountDownWidget->RemoveFromParent();	
	}
	if (ArrowLineWidget)
	{
		ArrowLineWidget->RemoveArrowLine();
	}
}


void UAbilityComponent::ListenHurtInstigatorDead()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(HurtMaker)
	ABaseEnemy* Enemy = Cast<ABaseEnemy>(HurtMaker);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Enemy)

	Enemy->OnEnemyDie.AddUniqueDynamic(this, &UAbilityComponent::OnHurtInstigatorDead);
}

FGameplayAbilitySpecHandle UAbilityComponent::GetGameplayAbilityWithTag(const FGameplayTag& Tag)
{
	if (!CachedASC)
	{
		return FGameplayAbilitySpecHandle();
	}
	
	TArray<FGameplayAbilitySpecHandle> OutAbilityHandles;
	FGameplayTagContainer TagContainer(Tag);
	CachedASC->FindAllAbilitiesWithTags(OutAbilityHandles, TagContainer);

	if (OutAbilityHandles.IsEmpty())
	{
		return FGameplayAbilitySpecHandle();
	}
	
	return OutAbilityHandles[0];
}

bool UAbilityComponent::FindExtendData(FGameplayTag Tag, float& Result)
{
	if (AbilityExtendData.Contains(Tag))
	{
		Result = AbilityExtendData[Tag];
		return true;
	}
	Result = 0;
	return false;
}

bool UAbilityComponent::FindAttributeEffects(FGameplayTag Tag, FAttributeEffectArray& Result)
{
	if (AbilityAttributeEffects.Contains(Tag))
	{
		Result = AbilityAttributeEffects[Tag];
		return true;
	}
	Result = FAttributeEffectArray();
	return false;
}

void UAbilityComponent::ApplyAttributeEffects(FGameplayTag Tag)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->BuffComponent);
	
	FAttributeEffectArray Effects;
	if (FindAttributeEffects(Tag, Effects))
	{
		PXCharacter->BuffComponent->AddAttributeEffects(Tag, Effects.Data);
	}
}

bool UAbilityComponent::GetIsAttacking()
{
	return false;
}

bool UAbilityComponent::GetIsDefending()
{
	return false;
}

void UAbilityComponent::OnAttackHit_Implementation(AActor* Receiver)
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnAttackHit(Receiver);
		}
	}
	ActivateAbilityByTiming(EAbilityTiming::AttackHit);
}

void UAbilityComponent::OnSkillHit_Implementation()
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnSkillHit();
		}
	}
	ActivateAbilityByTiming(EAbilityTiming::SkillHit);
}

void UAbilityComponent::OnBeAttacked_Implementation(AActor* Maker, int InDamage, int& OutDamage, bool bForce)
{
	if (InDamage <= 0)
	{
		OutDamage = 0;
		return;
	}
	
	if (!CachedASC || !Maker || !Maker->Implements<UFight_Interface>())
	{
		OutDamage = InDamage;
		return;
	}

	AcceptDamage = InDamage;
	int RemDamage = InDamage;
	
	HurtMaker = Maker;
	
	ActivateAbilityByTiming(EAbilityTiming::BeAttacked);

	// 所有涉及到 RemDamage 的都需要判断 bForce
	if (!bForce)
	{
		// 移形换影
		if (CachedASC->TryActivateAbilityByTagName("Ability.Mobiliarbus"))
		{
			RemDamage = 0;
		}
	}
	
	if (RemDamage > 0)
	{
		for (auto& Skill : DefenseSkills)
		{
			if (!Skill) continue;
	
			bool stop;
			int _OutDamage;
			Skill->OnBeAttacked(Maker, RemDamage, _OutDamage, stop);

			// 某些防御性技能不只是削减伤害
			if (!bForce)
			{
				RemDamage = _OutDamage;
			}
			if (stop) break;
		}
	}
	
	if (!Execute_GetOwnCamp(Maker).HasTag(TAG("Enemy.BOSS")))
	{	
		if (CachedASC->TryActivateAbilityByTagName("Ability.SkyHandPower.QTE"))
		{
			ListenHurtInstigatorDead();
		}
	}
	
	OutDamage = RemDamage; 
}

void UAbilityComponent::OnAttackEffect_Implementation()
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnAttackEffect();
		}
	}
	
	ActivateAbilityByTiming(EAbilityTiming::AttackEffect);
}

void UAbilityComponent::OnAttackWeakPoint_Implementation(AActor* Receiver)
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnHitWeakPoint(Receiver);
		}
	}
	ActivateAbilityByTiming(EAbilityTiming::AttackWeakPoint);
}

void UAbilityComponent::OnPickGold_Implementation()
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnPickGold();
		}
	}

	ActivateAbilityByTiming(EAbilityTiming::PickGold);
}

void UAbilityComponent::OnDashEffectEnd_Implementation()
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnSkillFinish();
		}
	}
	
	ActivateAbilityByTiming(EAbilityTiming::SkillFinish);
}


#undef LOCTEXT_NAMESPACE
