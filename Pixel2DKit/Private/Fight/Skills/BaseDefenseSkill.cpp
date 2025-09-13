// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/BaseDefenseSkill.h"

void ABaseDefenseSkill::OnBeAttacked_Implementation(AActor* Maker, int InDamage, int& OutDamage, bool& Stop)
{
	OutDamage = InDamage;
	Stop = false;
}

void ABaseDefenseSkill::OnDefenseEffect_Implementation(AActor* Actor)
{
	
}

void ABaseDefenseSkill::BeginPlay()
{
	Super::BeginPlay();

	if (DataAsset)
	{
		AbilityTags = DataAsset->AbilityTags;
		Priority = DataAsset->Priority;
		StopPropagation = DataAsset->StopPropagation;
	}
	
}
