// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerState/PXPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Character/BasePXCharacter.h"

void APXPlayerState::RecoverPlayerState()
{
	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(GetPawn());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);

	UAbilitySystemComponent* ASC = PXCharacter->GetAbilitySystemComponent();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ASC);

	ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Ability.GOWRecover")));


}
