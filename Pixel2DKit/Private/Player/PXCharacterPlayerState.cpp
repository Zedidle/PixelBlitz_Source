// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PXCharacterPlayerState.h"

#include "Pixel2DKit.h"
#include "GAS/PXCharacterASComponent.h"
#include "Character/BasePXCharacter.h"
#include "Enemy/BaseEnemy.h"
#include "GAS/AttributeSet/PXCharacterAttributeSet.h"


APXCharacterPlayerState::APXCharacterPlayerState(const FObjectInitializer& ObjectInitializer)
{
	
	AbilitySystem = CreateDefaultSubobject<UPXCharacterASComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
	
	CharacterAttributeSet = CreateDefaultSubobject<UPXCharacterAttributeSet>(TEXT("AttributeSetBase"));
}

UAbilitySystemComponent* APXCharacterPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

UPXAttributeSet* APXCharacterPlayerState::GetCharacterAttributeSet() const
{
	return CharacterAttributeSet;
}


APawn* APXCharacterPlayerState::GetPawn_Implementation()
{
	return GetPawn<ABasePXCharacter>();
}

void APXCharacterPlayerState::RecoverPlayerStateOnEnemyDie()
{
	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(Super::GetPawn());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);

	UAbilitySystemComponent* ASC = PXCharacter->GetAbilitySystemComponent();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ASC);

	ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(TAG("Ability.GOWRecover")));
}

void APXCharacterPlayerState::OnEnemyDie_Implementation(ABaseEnemy* Enemy)
{
	if (ABasePXCharacter* PXCharacter = GetPawn<ABasePXCharacter>())
	{
		PXCharacter->OnKillEnemy();
	}
}

bool APXCharacterPlayerState::GetIsAttacking()
{
	if (ABasePXCharacter* PXCharacter = GetPawn<ABasePXCharacter>())
	{
		return PXCharacter->GetIsAttacking();
	}
	return false;
}

bool APXCharacterPlayerState::GetIsDefending()
{
	if (ABasePXCharacter* PXCharacter = GetPawn<ABasePXCharacter>())
	{
		return PXCharacter->GetIsDefending();
	}
	return false;
}
