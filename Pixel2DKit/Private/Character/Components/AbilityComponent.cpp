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
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->DataAsset)

	if (!PXCharacter->DataAsset->AbilityDataTables.IsEmpty()) {
		for (auto& AbilityDataTable: PXCharacter->DataAsset->AbilityDataTables)
		{
			AbilityDataTables.Add(AbilityDataTable.LoadSynchronous());
		}
	}

	if (UPXASComponent* PXASComponent = Cast<UPXASComponent>(PXCharacter->GetAbilitySystemComponent()))
	{
		CachedASC = PXASComponent;
	}

	InitAbilities();
}


// Called every frame
void UAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAbilityComponent::InitAbilities()
{
	UPXMainSaveGame* MainSave = UPXSaveGameSubSystemFuncLib::GetMainData(this);
	if (MainSave)
	{
		ChosenAbilities = MainSave->ChosenAbilities;
		TakeEffectAbilities = MainSave->TakeEffectAbilities;
	}
}

void UAbilityComponent::LearnAbility(const FGameplayTag& AbilityTag)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem);

	ChosenAbilities.Add(AbilityTag);
	
	const FAbility& AbilityData = DataTableSubsystem->GetAbilityDataByTag(AbilityTag);

	if (!AbilityData.PreLevelAbility.IsValid())
	{
		TakeEffectAbilities.AddUnique(AbilityTag);
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
	else if (TakeEffectAbilities.Contains(AbilityData.PreLevelAbility))
	{
		TakeEffectAbilities.Remove(AbilityData.PreLevelAbility);
		TakeEffectAbilities.AddUnique(AbilityTag);
	}
	
}

void UAbilityComponent::LoadAbility()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC);
	
	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)
	
	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem);
	
	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	
	for (auto& Tag : TempTestAbilities)
	{
		TakeEffectAbilities.AddUnique(Tag);
	}
	
	for (auto Tag : TakeEffectAbilities)
	{
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
			EffectGameplayTags.AddData(D.Key, D.Value);
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
		BuffComponent->AddBuffByTag(TAG("Buff.Ability.InRock"));
	}

	// 空击
	Tag = TAG("AbilitySet.InAir.DamagePlus");
	if (EffectGameplayTags.Contains(Tag))
	{
		BuffComponent->AddBuffByTag(TAG("Buff.Ability.AirFight"));
	}
	

	// 天手力
	Tag = TAG("AbilitySet.SkyHandPower.Cooldown");
	if (EffectGameplayTags.Contains(Tag))
	{
		BuffComponent->AddBuffByTag(TAG("Buff.Ability.SkyHandPower"));
	}
	
	// 移形换影
	Tag = TAG("AbilitySet.Mobiliarbus.Cooldown");
	if (EffectGameplayTags.Contains(Tag))
	{
		BuffComponent->AddBuffByTag(TAG("Buff.Ability.Mobiliarbus"));
	}


	
	
#pragma endregion 
	
	
	if (PXCharacter)
	{
		PXCharacter->LoadAbility();
	}

	// 通过GameInstance存档
	UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem)
	UPXMainSaveGame* MainSaveGame = SaveGameSubsystem->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSaveGame)
	MainSaveGame->ChosenAbilities = ChosenAbilities;
	MainSaveGame->TakeEffectAbilities = TakeEffectAbilities;

	SaveGameSubsystem->SaveMainData();
}

bool UAbilityComponent::HasChosenAbility(const FGameplayTag& AbilityTag)
{
	return ChosenAbilities.Contains(AbilityTag);
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

	UPXMainSaveGame* MainSaveGame = SaveGameSubsystem->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(MainSaveGame, false)

	// 通过分表，不再需要角色判断
	// if (MainSaveGame->CurCharacterName != Ability.CharacterName && Ability.CharacterName != FName()) return false;
	
	if (ChosenAbilities.Contains(AbilityTag)) return false;

	UPXBasicBuildSaveGame* BasicBuildSaveGame = SaveGameSubsystem->GetBasicBuildData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(BasicBuildSaveGame, false)

	if (Ability.DefaultLock && !BasicBuildSaveGame->UnlockedAbilities.Contains(AbilityTag)) return false;

	if (Ability.PreLevelAbility.IsValid())
	{
		if (!TakeEffectAbilities.Contains(Ability.PreLevelAbility)) return false;
	}
	
	if (!Ability.RequiredAbilities.IsEmpty())
	{
		for (auto& RequiredAbility : Ability.RequiredAbilities)
		{
			if (!TakeEffectAbilities.Contains(RequiredAbility)) return false;
		}
	}

	if (!Ability.RequiredTalents.IsEmpty())
	{
		for (auto& RequiredAbility : Ability.RequiredAbilities)
		{
			if (!TakeEffectAbilities.Contains(RequiredAbility)) return false;
		}
	}
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
	
	// 触发黑荆棘
	CachedASC->TryActivateAbilitiesByTag(CreateGameplayTagContainer(FName("Ability.Blackthorn")));

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

	if (!Enemy->OnEnemyDie.IsAlreadyBound(this, &UAbilityComponent::OnHurtInstigatorDead))
	{
		Enemy->OnEnemyDie.AddDynamic(this, &UAbilityComponent::OnHurtInstigatorDead);
	}
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