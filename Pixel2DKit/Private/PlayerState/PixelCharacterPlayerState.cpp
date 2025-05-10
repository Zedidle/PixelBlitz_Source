// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/PixelCharacterPlayerState.h"
#include "GAS/PixelCharacterASComponent.h"
#include "GAS/AttributeSet/PixelAttributeSet.h"


APixelCharacterPlayerState::APixelCharacterPlayerState(const FObjectInitializer& ObjectInitializer)
{
	
	AbilitySystem = CreateDefaultSubobject<UPixelCharacterASComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
	
	// StateComponent = CreateDefaultSubobject<UPixelCharacterStateComponent>(TEXT("StateComponent"));
		
	CharacterAttributeSet = CreateDefaultSubobject<UPixelAttributeSet>(TEXT("AttributeSetBase"));
}

UAbilitySystemComponent* APixelCharacterPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

UPixelAttributeSet* APixelCharacterPlayerState::GetCharacterAttributeSet() const
{
	return CharacterAttributeSet;
}