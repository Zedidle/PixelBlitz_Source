// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/BaseSkill.h"

#include "Interfaces/Fight_Interface.h"
#include "Utilitys/SoundFuncLib.h"

bool ABaseSkill::CanDamageEffect_Implementation(AActor* Actor)
{
	if (!Owner || !Actor) return false;
	if (!Owner->Implements<UFight_Interface>() || !Actor->Implements<UFight_Interface>()) return false;

	const FGameplayTagContainer& DamageMakerEnemyCamp = IFight_Interface::Execute_GetEnemyCamp(Owner);
	const FGameplayTagContainer& ActorCamp = IFight_Interface::Execute_GetOwnCamp(Actor);
	
	return DamageMakerEnemyCamp.HasAny(ActorCamp) || ActorCamp.HasAny(DamageMakerEnemyCamp);
}


void ABaseSkill::BeginPlay()
{
	Super::BeginPlay();
	
	if (BeginSound)
	{
		USoundFuncLib::PlaySoundAtLocation(BeginSound, GetActorLocation());
	}
	
}

void ABaseSkill::SetActive(bool v)
{
	bActive = v;
	if (bActive)
	{
		SetActorTickEnabled(true);
		SetActorEnableCollision(true);
		SetActorHiddenInGame(false);
	}
	else
	{
		SetActorTickEnabled(false);
		SetActorEnableCollision(false);
		SetActorHiddenInGame(true);
	}
}

bool ABaseSkill::IsActive() const
{
	return bActive;
}

void ABaseSkill::OnSkillEnd()
{
	bEnding = true;
	SetActorEnableCollision(false);
	
	SetLifeSpan(2.0f);

	BP_OnSkillEnd();
}

