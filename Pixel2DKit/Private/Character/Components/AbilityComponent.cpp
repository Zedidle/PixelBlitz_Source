// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/AbilityComponent.h"

#include <opensubdiv/vtr/level.h>

#include "AbilitySystemComponent.h"
#include "ActorMode.h"
#include "CurveEditorTypes.h"
#include "InputAction.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/BasePXCharacter.h"
#include "Character/PXCharacterDataAsset.h"
#include "Character/Components/BuffComponent.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Enemy/BaseEnemy.h"
#include "Fight/Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/CustomResourceSettings.h"
#include "Settings/SkillSettings.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Util/ColorConstants.h"
#include "Utilitys/PXCustomStruct.h"
#include "WorldPartition/Cook/WorldPartitionCookPackage.h"

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
			RequestGameplayTagChildren(FGameplayTag::RequestGameplayTag(TagName));

		AbilityTagsChildren.AddTag(FGameplayTag::RequestGameplayTag(TagName));
		return AbilityTagsChildren;
	}

	
	FGameplayTagContainer GameplayTagContainer;
	GameplayTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TagName));
	return GameplayTagContainer;
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

	if (!AbilityDataTable && PXCharacter->DataAsset->AbilityDataTable) {
		AbilityDataTable = PXCharacter->DataAsset->AbilityDataTable.LoadSynchronous();
	}

	CachedASC = PXCharacter->GetAbilitySystemComponent();
}


// Called every frame
void UAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAbilityComponent::LearnAbility(FName AbilityIndex)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilityDataTable)
	
	if (!UDataTableFunctionLibrary::DoesDataTableRowExist(AbilityDataTable, AbilityIndex)) return;

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UDataTableSubsystem* DataTableManager = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableManager)

	ChoicedAbilityIndexes.Add(AbilityIndex);
	
	FAbility AbilityData = DataTableManager->FindRow<FAbility>(AbilityDataTable, AbilityIndex);

	if (AbilityData.PreLevelIndex == EName::None)
	{
		TakeEffectAbilityIndexes.AddUnique(AbilityIndex);
		// 由于同一技能的不同等级都使用相同的GA，只是数值不同，就只用在学习第一个技能是GiveAbility
		UClass* AbilityClass = AbilityData.AbilityClass.LoadSynchronous();
		if (!AbilityClass) return;
		
		FGameplayAbilitySpec Spec(AbilityClass);
		CachedASC->GiveAbility(Spec);
	}
	else if (TakeEffectAbilityIndexes.Contains(AbilityData.PreLevelIndex))
	{
		TakeEffectAbilityIndexes.Remove(AbilityData.PreLevelIndex);
		TakeEffectAbilityIndexes.AddUnique(AbilityIndex);
	}
	
}

void UAbilityComponent::LoadAbility()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilityDataTable)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC);

	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	
	for (auto Index : TempTestAbilityIndexes)
	{
		TakeEffectAbilityIndexes.AddUnique(Index);
	}

	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)
	
	UDataTableSubsystem* DataTableManager = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableManager)

	
	for (auto Index : TakeEffectAbilityIndexes)
	{
		FAbility AbilityData = DataTableManager->FindRow<FAbility>(AbilityDataTable, Index);
		if (UClass* LoadedClass = AbilityData.AbilityClass.LoadSynchronous())
		{
			FGameplayAbilitySpec Spec(LoadedClass);
			CachedASC->GiveAbility(Spec);
		}
		
		TArray<FGameplayTag> Keys;
		AbilityData.Effect_GameplayTag.GetKeys(Keys);

		for (auto Key : Keys)
		{
			EffectGameplayTags.AddData(Key, AbilityData.Effect_GameplayTag[Key]);
		}
	}

	UHealthComponent* HealthComponent = PXCharacter->GetComponentByClass<UHealthComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(HealthComponent)
	UBuffComponent* BuffComponent = PXCharacter->GetComponentByClass<UBuffComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffComponent)


# pragma region 通用技能加载部分，专属技能在 PXCharacter 子类中的LoadAbility自定义

	FGameplayTag Tag;
	FGameplayTagContainer Tags;

	// 空中移动的控制（暂且无用）
	Tag = FGameplayTag::RequestGameplayTag("AbilitySet.AirMoveEffectAddPercent");
	if (EffectGameplayTags.Contains(Tag))
	{
		PXCharacter->GetCharacterMovement()->AirControl = PXCharacter->BasicAirControl * (1 + EffectGameplayTags[Tag]);
	}
	else
	{
		PXCharacter->GetCharacterMovement()->AirControl = PXCharacter->BasicAirControl;
	}


	// 跳跃上升时间
	Tag = FGameplayTag::RequestGameplayTag("AbilitySet.JumpMaxHoldTimePlus");
	if (EffectGameplayTags.Contains(Tag))
	{
		PXCharacter->JumpMaxHoldTime = PXCharacter->BasicJumpMaxHoldTime + EffectGameplayTags[Tag];
	}
	else
	{
		PXCharacter->JumpMaxHoldTime = PXCharacter->BasicJumpMaxHoldTime;
	}


	// 附加跳跃次数
	Tag = FGameplayTag::RequestGameplayTag("AbilitySet.MaxJumpCountPlus");
	if (EffectGameplayTags.Contains(Tag))
	{
		PXCharacter->CurMaxJumpCount = PXCharacter->BasicMaxJumpCount + EffectGameplayTags[Tag];
	}
	else
	{
		PXCharacter->CurMaxJumpCount = PXCharacter->BasicMaxJumpCount;
	}
	
	// EP恢复加快
	Tag = FGameplayTag::RequestGameplayTag("AbilitySet.EPRecoverLevel");
	if (EffectGameplayTags.Contains(Tag))
	{
		TArray<FActiveGameplayEffectHandle> Handles = CachedASC->GetActiveEffectsWithAllTags(
						FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Ability.RecoverEP")));
		for (auto& Handle : Handles)
		{
			CachedASC->SetActiveGameplayEffectLevel(Handle, EffectGameplayTags[Tag]);
		}
	}
	
	// 霸体
	Tag = FGameplayTag::RequestGameplayTag("AbilitySet.InRock");
	HealthComponent->bInRock = EffectGameplayTags.Contains(Tag);
	if (HealthComponent->bInRock)
	{
		BuffComponent->AddBuffByTag(FGameplayTag::RequestGameplayTag("Buff.Ability.InRock"));
	}

	// 空击
	Tag = FGameplayTag::RequestGameplayTag("AbilitySet.InAir.DamagePlus");
	if (EffectGameplayTags.Contains(Tag))
	{
		BuffComponent->AddBuffByTag(FGameplayTag::RequestGameplayTag("Buff.Ability.AirFight"));
	}
	

	// 天手力
	Tag = FGameplayTag::RequestGameplayTag("AbilitySet.SkyHandPower.Cooldown");
	if (EffectGameplayTags.Contains(Tag))
	{
		BuffComponent->AddBuffByTag(FGameplayTag::RequestGameplayTag("Buff.Ability.SkyHandPower"));
	}
	
	// 移形换影
	Tag = FGameplayTag::RequestGameplayTag("AbilitySet.Mobiliarbus.Cooldown");
	if (EffectGameplayTags.Contains(Tag))
	{
		BuffComponent->AddBuffByTag(FGameplayTag::RequestGameplayTag("Buff.Ability.Mobiliarbus"));
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
	MainSaveGame->ChoicedAbilityIndexes = ChoicedAbilityIndexes;
	MainSaveGame->TakeEffectAbilityIndexes = TakeEffectAbilityIndexes;

	SaveGameSubsystem->SaveMainData();
}

bool UAbilityComponent::HasChoiced(FName AbilityIndex)
{
	return ChoicedAbilityIndexes.Contains(AbilityIndex);
}

bool UAbilityComponent::CanLearnAbility(const FName& RowNameIndex, const FAbility& Ability)
{
	if (!Ability.Enable) return false;

	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GameInstance, false)

	UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(SaveGameSubsystem, false)

	UPXMainSaveGame* MainSaveGame = SaveGameSubsystem->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(MainSaveGame, false)

	if (MainSaveGame->CurCharacterName != Ability.CharacterName && Ability.CharacterName != FName()) return false;

	if (ChoicedAbilityIndexes.Contains(RowNameIndex)) return false;

	UPXBasicBuildSaveGame* BasicBuildSaveGame = SaveGameSubsystem->GetBasicBuildData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(BasicBuildSaveGame, false)

	if (Ability.DefaultLock && !BasicBuildSaveGame->UnlockAbilityIndexes.Contains(RowNameIndex)) return false;

	if (Ability.PreLevelIndex == FName()) return true;

	return TakeEffectAbilityIndexes.Contains(Ability.PreLevelIndex);
}

void UAbilityComponent::OnBeAttacked(AActor* Instigator, int InDamage, int& OutDamage)
{
	if (!CachedASC || !Instigator->Implements<UFight_Interface>())
	{
		OutDamage = InDamage;
		return;
	}

	int SurDamage = InDamage;
	
	HurtInstigator = Instigator;
	
	// 触发黑荆棘
	CachedASC->TryActivateAbilitiesByTag(CreateGameplayTagContainer(FName("Ability.Blackthorn")));
	
	// 移形换影
	if (CachedASC->TryActivateAbilitiesByTag(CreateGameplayTagContainer(FName("Ability.Mobiliarbus"))))
	{
		SurDamage = 0;
	}
	
	// 检查天手力是否有效 且 冷却结束
	FGameplayAbilitySpecHandle AbilitySpecHandle = GetGameplayAbilityWithTag(FGameplayTag::RequestGameplayTag("AbilitySet.SkyHandPower"));
	if (const FGameplayAbilitySpec* Spec = CachedASC->FindAbilitySpecFromHandle(AbilitySpecHandle))
	{
		UGameplayAbility* AbilityInstance_SkyHandPower = Spec->GetPrimaryInstance();
		if (AbilityInstance_SkyHandPower->K2_CheckAbilityCooldown())
		{
			OnHurtInstigatorDead(nullptr);
			CreateQTE();
			ListHurtInstigatorDead();
		}
	}

	OutDamage = SurDamage; 
}

void UAbilityComponent::CreateQTE()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(KeyPressCountdownWidgetClass)
	KeyPressCountDownWidget = CreateWidget<UKeyPressCountDownWidget>(GetWorld(), KeyPressCountdownWidgetClass);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(KeyPressCountDownWidget)
	KeyPressCountDownWidget->InitializeData(1.5, 1.2);
	KeyPressCountDownWidget->AddToViewport(100);

	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ArrowLineWidgetClass)
	ArrowLineWidget = CreateWidget<UArrowLineWidget>(GetWorld(), ArrowLineWidgetClass);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ArrowLineWidget)
	ArrowLineWidget->InitializeData(PXCharacter, HurtInstigator, true, 10,
		600, FLinearColor::White, 1.5, FVector2D(36, 54),
		FVector2D(64, 64), FVector::Zero());
	ArrowLineWidget->AddToViewport(100);
}

void UAbilityComponent::OnKeyPressed(UInputAction* InputAction, bool& Keep)
{
	if (Action_Jump != InputAction)
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


	if (CachedASC)
	{
		CachedASC->TryActivateAbilitiesByTag(CreateGameplayTagContainer(FName("Ability.SkyHandPower")));
	}

	if (ArrowLineWidget)
	{
		ArrowLineWidget->RemoveFromParent();
	}

	Keep = false;
}

void UAbilityComponent::OnHurtInstigatorDead_Implementation(ABaseEnemy* DeadEnemy)
{
	if (KeyPressCountDownWidget)
	{
		KeyPressCountDownWidget->RemoveFromParent();	
	}
	if (ArrowLineWidget)
	{
		ArrowLineWidget->RemoveFromParent();
	}
	
}

void UAbilityComponent::ListHurtInstigatorDead()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(HurtInstigator)
	ABaseEnemy* Enemy = Cast<ABaseEnemy>(HurtInstigator);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Enemy)
	Enemy->OnEnemyDeath.AddDynamic(this, &UAbilityComponent::OnHurtInstigatorDead);

}

FGameplayAbilitySpecHandle UAbilityComponent::GetGameplayAbilityWithTag(const FGameplayTag& Tag)
{
	if (!CachedASC)
	{
		return FGameplayAbilitySpecHandle();
	}
	
	TArray<FGameplayAbilitySpecHandle> OutAbilityHandles;
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(Tag);
	CachedASC->FindAllAbilitiesWithTags(OutAbilityHandles, TagContainer);

	if (OutAbilityHandles.IsEmpty())
	{
		return FGameplayAbilitySpecHandle();
	}
	
	return OutAbilityHandles[0];
}
