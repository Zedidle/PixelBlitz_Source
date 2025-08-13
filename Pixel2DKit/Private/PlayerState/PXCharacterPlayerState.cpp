// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/PXCharacterPlayerState.h"
#include "GAS/PXCharacterASComponent.h"
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