// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/AbilityComponent.h"

#include "Character/BasePXCharacter.h"
#include "Character/PXCharacterDataAsset.h"
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
	AActor* Owner = GetOwner();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilityDataTable)

	for (auto Index : TempTestAbilityIndexes)
	{
		TakeEffectAbilityIndexes.AddUnique(Index);
	}

	UDataTableSubsystem* DataTableManager = Owner->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
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


	/**
	 *通用技能加载部分，专属技能在 PXCharacter 子类中的LoadAbility自定义
	 *
	 *
	 *
	 **/

	
	if (PXCharacter)
	{
		PXCharacter->LoadAbility();
	}

	// 通过GameInstance存档
	
	
}

