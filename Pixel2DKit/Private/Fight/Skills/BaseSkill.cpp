// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/BaseSkill.h"

#include "RayTracingDebugVisualizationMenuCommands.h"
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
		ReinitializeNiagara();
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
		bIdle = false;
		if (USkillManager* SkillManager = GetGameInstance()->GetSubsystem<USkillManager>())
		{
			SkillManager->DeactivateSkill(this);
		}
	}

	bActive = v;
}

void ABaseSkill::SetSkillLifeTimer(bool bActivate)
{
	FName TimerName = FName(GetName() + "_Skill.SetActive");
	if (bActivate)
	{
		UTimerSubsystemFuncLib::SetDelayLoop(this, TimerName,[WeakThis = TWeakObjectPtr(this)]
		{
			if (!WeakThis.IsValid()) return;
			if (WeakThis->bIdle) return;
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

void ABaseSkill::SetActivateTiming(EAbilityTiming Timing)
{
	ActivateTiming = Timing;
}

bool ABaseSkill::OnAttackStart()
{
	if (ActivateTiming != EAbilityTiming::AttackStart) return false;
	BP_OnAttackStart();
	return true;
}

bool ABaseSkill::OnAttackHit()
{
	if (ActivateTiming != EAbilityTiming::AttackHit) return false;
	BP_OnAttackHit();
	return true;
}

bool ABaseSkill::OnAttackFinish()
{
	if (ActivateTiming != EAbilityTiming::AttackFinish) return false;
	BP_OnAttackFinish();
	return true;
}

bool ABaseSkill::OnAttackSkill()
{
	if (ActivateTiming != EAbilityTiming::AttackSkill) return false;
	BP_OnAttackSkill();
	return true;
}

bool ABaseSkill::OnSkillStart()
{
	if (ActivateTiming != EAbilityTiming::SkillStart) return false;
	BP_OnSkillStart();
	return true;
}

bool ABaseSkill::OnSkillHit()
{
	if (ActivateTiming != EAbilityTiming::SkillHit) return false;
	BP_OnSkillHit();
	return true;
}

bool ABaseSkill::OnSkillFinish()
{
	if (ActivateTiming != EAbilityTiming::SkillFinish) return false;
	BP_OnSkillFinish();
	return true;
}

bool ABaseSkill::OnKillEnemy()
{
	if (ActivateTiming != EAbilityTiming::KillEnemy) return false;
	BP_OnKillEnemy();
	return true;
}

bool ABaseSkill::OnBeAttacked(AActor* Maker, int InDamage, int& OutDamage, bool& Stop)
{
	if (ActivateTiming != EAbilityTiming::BeAttacked) return false;
	return BP_OnBeAttacked(Maker, InDamage, OutDamage, Stop);
}

bool ABaseSkill::OnBeAttackedInvulnerable()
{
	if (ActivateTiming != EAbilityTiming::BeAttackedInvulnerable) return false;
	BP_OnBeAttackedInvulnerable();
	return true;
}

bool ABaseSkill::OnBeDamaged()
{
	if (ActivateTiming != EAbilityTiming::BeDamaged) return false;
	BP_OnBeDamaged();
	return true;
}

bool ABaseSkill::OnJumpStart()
{
	if (ActivateTiming != EAbilityTiming::JumpStart) return false;
	BP_OnJumpStart();
	return true;
}

bool ABaseSkill::OnLanding()
{
	if (ActivateTiming != EAbilityTiming::Landing) return false;
	BP_OnLanding();
	return true;
}

bool ABaseSkill::OnDefenseStart()
{
	if (ActivateTiming != EAbilityTiming::DefenseStart) return false;
	BP_OnDefenseStart();
	return true;
}

bool ABaseSkill::OnDefenseSuccess()
{
	if (ActivateTiming != EAbilityTiming::DefenseSuccess) return false;
	BP_OnDefenseSuccess();
	return true;
}

bool ABaseSkill::OnDefenseFinish()
{
	if (ActivateTiming != EAbilityTiming::DefenseFinish) return false;
	BP_OnDefenseFinish();
	return true;
}

bool ABaseSkill::OnDying()
{
	if (ActivateTiming != EAbilityTiming::Dying) return false;
	BP_OnDying();
	return true;
}

bool ABaseSkill::OnPickGold()
{
	if (ActivateTiming != EAbilityTiming::PickGold) return false;
	BP_OnPickGold();
	return true;
}

// bool ABaseSkill::ActivateByTiming(EAbilityTiming Timing)
// {
// 	if (ActivateTiming != Timing) return false;
// 	BP_ActivateByTiming();
// }
