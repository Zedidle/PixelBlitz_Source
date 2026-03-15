// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/BaseDefenseSkill.h"


void ABaseDefenseSkill::OnDefenseEffect_Implementation(AActor* Actor)
{
	
}

void ABaseDefenseSkill::BeginPlay()
{
	Super::BeginPlay();

	if (DataAsset)
	{
		AbilityTag = DataAsset->AbilityTag;
		Priority = DataAsset->Priority;
		StopPropagation = DataAsset->StopPropagation;
	}
	
}
