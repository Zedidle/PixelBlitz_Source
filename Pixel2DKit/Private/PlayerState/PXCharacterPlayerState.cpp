// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/PXCharacterPlayerState.h"
#include "GAS/PXCharacterASComponent.h"
#include "Character/BasePXCharacter.h"
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

UAbilityComponent* APXCharacterPlayerState::GetAbilityComponent_Implementation()
{
	if (ABasePXCharacter* PXCharacter = GetPawn<ABasePXCharacter>())
	{
		return PXCharacter->AbilityComponent;
	}
	
	return nullptr;
}

APawn* APXCharacterPlayerState::GetPawn_Implementation()
{
	return GetPawn<ABasePXCharacter>();
}

void APXCharacterPlayerState::RecoverPlayerStateOnEnemyDie()
{
	// ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(IFight_Interface::Execute_GetPawn(this));
	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(Super::GetPawn());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);

	UAbilitySystemComponent* ASC = PXCharacter->GetAbilitySystemComponent();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ASC);

	// GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0f, FColor(255,48,16), "RecoverPlayerStateOnMonsterDead");
	ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Ability.GOWRecover")));
}

void APXCharacterPlayerState::OnEnemyDie_Implementation(ABaseEnemy* Enemy)
{
	RecoverPlayerStateOnEnemyDie();

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
