// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/BaseSkill.h"

#include "Interfaces/Fight_Interface.h"

bool ABaseSkill::CanDamageEffect_Implementation(AActor* Actor)
{
	if (!GetOwner() || !Actor) return false;
	if (!GetOwner()->Implements<UFight_Interface>()) return false;
	if (!Actor->Implements<UFight_Interface>()) return false;

	const FGameplayTagContainer& DamageMakerEnemyCamp = IFight_Interface::Execute_GetEnemyCamp(Actor);
	const FGameplayTagContainer& OwnerCamp = IFight_Interface::Execute_GetOwnCamp(GetOwner());

	return DamageMakerEnemyCamp.HasAny(OwnerCamp);
}
