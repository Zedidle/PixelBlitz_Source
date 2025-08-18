// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/AbilityComponent.h"

#include "Character/BasePXCharacter.h"
#include "Character/PXCharacterDataAsset.h"
#include "Character/Components/BuffComponent.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Fight/Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Utilitys/PXCustomStruct.h"

// Sets default values for this component's properties
UAbilityComponent::UAbilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
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

	if (!PXCharacter->DataAsset->AbilityDataTable.IsValid()) {
		PXCharacter->DataAsset->AbilityDataTable.LoadSynchronous();
		AbilityDataTable = PXCharacter->DataAsset->AbilityDataTable.Get();
	}
}


// Called every frame
void UAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAbilityComponent::LearnAbility(FName AbilityIndex)
{
	AActor* Owner = GetOwner();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilityDataTable)

	if (!UDataTableFunctionLibrary::DoesDataTableRowExist(AbilityDataTable, AbilityIndex)) return;
	
	UDataTableSubsystem* DataTableManager = Owner->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableManager)

	ChoicedAbilityIndexes.Add(AbilityIndex);
	
	FAbility AbilityData = DataTableManager->FindRow<FAbility>(AbilityDataTable, AbilityIndex);

	if (AbilityData.PreLevelIndex == EName::None)
	{
		TakeEffectAbilityIndexes.AddUnique(AbilityIndex);
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
		if (AbilityData.CharacterName == EName::None)
		{
			TArray<FGameplayTag> Keys;
			AbilityData.Effect_GameplayTag.GetKeys(Keys);

			for (auto Key : Keys)
			{
				float v = AbilityData.Effect_GameplayTag[Key];

				if (EffectGameplayTag.Contains(Key))
				{
					EffectGameplayTag[Key] = EffectGameplayTag[Key] + v;
				}
				else
				{
					EffectGameplayTag.Add(Key, v);
				}
			}
		}
	}


# pragma region 通用技能加载部分，专属技能在 PXCharacter 子类中的LoadAbility自定义

	// 空中移动的控制（暂且无用）
	FGameplayTag Tag;

	Tag = FGameplayTag::RequestGameplayTag("AbilitySet.AirMoveEffectAddPercent");
	if (EffectGameplayTag.Contains(Tag))
	{
		PXCharacter->GetCharacterMovement()->AirControl = PXCharacter->BasicAirControl * (1 + EffectGameplayTag[Tag]);
	}
	else
	{
		PXCharacter->GetCharacterMovement()->AirControl = PXCharacter->BasicAirControl;
	}


	// 跳跃上升时间
	Tag = FGameplayTag::RequestGameplayTag("AbilitySet.JumpMaxHoldTimePlus");
	if (EffectGameplayTag.Contains(Tag))
	{
		PXCharacter->JumpMaxHoldTime = PXCharacter->BasicJumpMaxHoldTime + EffectGameplayTag[Tag];
	}
	else
	{
		PXCharacter->JumpMaxHoldTime = PXCharacter->BasicJumpMaxHoldTime;
	}


	// 附加跳跃次数
	Tag = FGameplayTag::RequestGameplayTag("AbilitySet.MaxJumpCountPlus");
	if (EffectGameplayTag.Contains(Tag))
	{
		PXCharacter->CurMaxJumpCount = PXCharacter->BasicMaxJumpCount + EffectGameplayTag[Tag];
	}
	else
	{
		PXCharacter->CurMaxJumpCount = PXCharacter->BasicMaxJumpCount;
	}


	// 霸体
	if (UHealthComponent* HealthComponent = PXCharacter->GetComponentByClass<UHealthComponent>())
	{
		Tag = FGameplayTag::RequestGameplayTag("AbilitySet.Stoic");
		HealthComponent->bInRock = EffectGameplayTag.Contains(Tag);
		if (HealthComponent->bInRock)
		{
			FString BuffName = ULocalizationFuncLib::GetBuffText(FName("Buff_Rock"));
			if (PXCharacter->BuffComponent)
			{
				PXCharacter->BuffComponent->AddBuff(Tag, BuffName, FLinearColor(FColor::Purple), false);
			}
		}
	}
	
	
#pragma endregion 
	
	
	if (PXCharacter)
	{
		PXCharacter->LoadAbility();
	}

	// 通过GameInstance存档
	UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem->MainSaveGame)
	SaveGameSubsystem->MainSaveGame->ChoicedAbilityIndexes = ChoicedAbilityIndexes;
	SaveGameSubsystem->MainSaveGame->TakeEffectAbilityIndexes = TakeEffectAbilityIndexes;

	SaveGameSubsystem->SaveMainData();
}

bool UAbilityComponent::HasChoiced(FName AbilityIndex)
{
	return ChoicedAbilityIndexes.Contains(AbilityIndex);
}

bool UAbilityComponent::CanLearnAbiliy(const FName& RowNameIndex, const FAbility& Ability)
{
	if (!Ability.Enable) return false;

	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GameInstance, false)

	UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(SaveGameSubsystem, false)

	UPXMainSaveGame* MainSaveGame = SaveGameSubsystem->MainSaveGame;
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(MainSaveGame, false)

	if (MainSaveGame->CurCharacterName != Ability.CharacterName && Ability.CharacterName != FName()) return false;

	if (ChoicedAbilityIndexes.Contains(RowNameIndex)) return false;

	UPXBasicBuildSaveGame* BasicBuildSaveGame = SaveGameSubsystem->BasicBuildSaveGame;
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(BasicBuildSaveGame, false)

	if (Ability.DefaultLock && !BasicBuildSaveGame->UnlockAbilityIndexes.Contains(RowNameIndex)) return false;

	if (Ability.PreLevelIndex == FName()) return true;

	return TakeEffectAbilityIndexes.Contains(Ability.PreLevelIndex);
}

