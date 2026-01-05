// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/AbilityComponent.h"
#include "AbilitySystemComponent.h"
#include "Character/BasePXCharacter.h"
#include "Character/PXCharacterDataAsset.h"
#include "Character/Components/BuffComponent.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Enemy/BaseEnemy.h"
#include "Fight/Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Subsystems/DataTableSubsystem.h"
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

void UAbilityComponent::InitAbilities()
{
	// 天赋需要在此之前加载完成
	float GoldenBlessRadioPlus;
	if (IFight_Interface::Execute_FindEffectGameplayTag(PXCharacter, TAG("TalentSet.GoldenBless"), GoldenBlessRadioPlus))
	{
		GoldenRadio += GoldenBlessRadioPlus;
	}

	float LegendBlessRadioPlus;
	if (IFight_Interface::Execute_FindEffectGameplayTag(PXCharacter, TAG("TalentSet.LegendBless"), LegendBlessRadioPlus))
	{
		GoldenRadio += LegendBlessRadioPlus;
		LegendRadio += LegendBlessRadioPlus;
	}

	// 目前放在AbilityChoice界面调用
	// RefreshAbilitiesCanChoice();
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
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->BuffComponent);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC);
	
	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)
	
	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem);

	UPXMainSaveGame* MainSave = GameInstance->GetSubsystem<UPXSaveGameSubsystem>()->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSave);
	
	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	
	for (auto Tag : MainSave->TakeEffectAbilities)
	{
		FString Parent;
		FString Level;
		Tag.ToString().Split(".Level", &Parent, &Level);

		if (CachedASC->HasAbility(TAG(*Parent))) continue;
		
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
		
		for (auto& D : AbilityData.Effect_GameplayTag)
		{
			EffectGameplayTags.SetData(D.Key, D.Value);
		}

		PXCharacter->BuffComponent->AddBuffByTag(TAG(*Parent));

		if (AbilityData.Timing != EAbilityTiming::None)
		{
			FGameplayTagArray& TagArray = AbilitiesTiming.FindOrAdd(AbilityData.Timing);
			TagArray.Tags.Add(TAG(*Parent));
		}
	}

	UHealthComponent* HealthComponent = PXCharacter->GetComponentByClass<UHealthComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(HealthComponent)
	UBuffComponent* BuffComponent = PXCharacter->GetComponentByClass<UBuffComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffComponent)


# pragma region 通用技能加载部分，专属技能在 PXCharacter 子类中的LoadAbility自定义

	FGameplayTag Tag;
	FGameplayTagContainer Tags;

	// 空中移动的控制
	Tag = TAG("AbilitySet.AirMoveEffectAddPercent");
	if (EffectGameplayTags.Contains(Tag))
	{
		PXCharacter->GetCharacterMovement()->AirControl = PXCharacter->BasicAirControl * (1 + EffectGameplayTags[Tag]);
	}


	// 跳跃上升时间
	Tag = TAG("AbilitySet.JumpMaxHoldTimePlus");
	if (EffectGameplayTags.Contains(Tag))
	{
		PXCharacter->JumpMaxHoldTime = PXCharacter->BasicJumpMaxHoldTime + EffectGameplayTags[Tag];
	}
	
	// 附加跳跃次数
	Tag = TAG("AbilitySet.MaxJumpCountPlus");
	if (EffectGameplayTags.Contains(Tag))
	{
		PXCharacter->CurMaxJumpCount = PXCharacter->CurMaxJumpCount + EffectGameplayTags[Tag];
	}
	
	// EP恢复加快
	Tag = TAG("AbilitySet.EPRecoverLevel");
	if (EffectGameplayTags.Contains(Tag))
	{
		TArray<FActiveGameplayEffectHandle> Handles = CachedASC->GetActiveEffectsWithAllTags(
						FGameplayTagContainer(TAG("Ability.RecoverEP")));
		for (auto& Handle : Handles)
		{
			CachedASC->SetActiveGameplayEffectLevel(Handle, EffectGameplayTags[Tag]);
		}
	}
	
	// 霸体
	Tag = TAG("AbilitySet.InRock");
	if (EffectGameplayTags.Contains(Tag))
	{
		HealthComponent->InRockPercent = EffectGameplayTags[Tag];
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
		for (auto& RequiredAbility : Ability.RequiredAbilities)
		{
			if (!MainSave->ChosenAbilities.Contains(RequiredAbility)) return false;
		}
	}

	if (!Ability.RequiredTalents.IsEmpty())
	{
		for (auto& RequiredTalent : Ability.RequiredTalents)
		{
			if (!BasicBuildSave->ChosenTalents.Contains(RequiredTalent)) return false;
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


void UAbilityComponent::OnBeAttacked(AActor* Maker, int InDamage, int& OutDamage, bool bForce)
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

	if (CachedASC->TryActivateAbilityByTagName("Ability.SkyHandPower.QTE"))
	{
		ListenHurtInstigatorDead();
	}

	OutDamage = RemDamage; 
}

void UAbilityComponent::OnLanding()
{
	ActivateAbilityByTiming(EAbilityTiming::Landing);
}

void UAbilityComponent::OnSkillStart()
{
	ActivateAbilityByTiming(EAbilityTiming::SkillStart);
}

void UAbilityComponent::OnAttackSkill()
{
	ActivateAbilityByTiming(EAbilityTiming::AttackSkill);
}

void UAbilityComponent::OnKillEnemy()
{
	ActivateAbilityByTiming(EAbilityTiming::KillEnemy);
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
	KeyPressCountDownWidget->InitializeData(_SustainTime, _Scale, Action_Jump);
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

void UAbilityComponent::OnKeyPressed(const FName& TagName, bool& Keep)
{
	Keep = true;
	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->DataAsset)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->DataAsset->InputConfig)

	const auto& ActionMap = PXCharacter->DataAsset->InputConfig->ActionMap;
	FGameplayTag Tag = TAG(TagName);
	if (!ActionMap.Contains(Tag) || Action_Jump != ActionMap[Tag])
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


#undef LOCTEXT_NAMESPACE