// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/BaseSkill.h"

#include "Interfaces/Fight_Interface.h"
#include "Subsystems/SkillManager.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "Utilitys/DebugFuncLab.h"
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

}


void ABaseSkill::RepelFromActor(AActor* Other)
{
}

void ABaseSkill::SetActive(bool v)
{
	if (v)
	{
		SetActorTickEnabled(true);
		SetActorEnableCollision(true);
		SetActorHiddenInGame(false);
		SetSkillLifeTimer(true);
		if (BeginSound)
		{
			USoundFuncLib::PlaySoundAtLocation(BeginSound, GetActorLocation());
		}
	}
	else
	{
		SetActorTickEnabled(false);
		SetActorEnableCollision(false);
		SetActorHiddenInGame(true);
		SetSkillLifeTimer(false);

		if (USkillManager* SkillManager = GetGameInstance()->GetSubsystem<USkillManager>())
		{
			SkillManager->DeactivateSkill(this);
		}
	}

	bActive = v;
}

void ABaseSkill::SetSkillLifeTimer(bool bActivate)
{
	FName TimerName = FName(GetActorNameOrLabel() + "_Skill.SetActive");
	if (bActivate)
	{
		UTimerSubsystemFuncLib::SetRetriggerableDelay(this, TimerName,[WeakThis = TWeakObjectPtr(this)]
		{
			if (!WeakThis.IsValid()) return;
			WeakThis->SetActive(false);
		}, LifeSpan);
	}
	else
	{
		UTimerSubsystemFuncLib::CancelDelay(this, TimerName);
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
	SetSkillLifeTimer(true);

	BP_OnSkillEnd();
}

