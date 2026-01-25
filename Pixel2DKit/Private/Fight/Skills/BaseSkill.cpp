// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/BaseSkill.h"

#include "Pixel2DKit.h"
#include "Character/Components/AbilityComponent.h"
#include "Interfaces/Fight_Interface.h"
#include "Subsystems/AchievementSubsystem.h"
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
		}, LifeTime);
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

void ABaseSkill::ApplyAttributeEffects()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)

	if (UAbilityComponent* AbilityComponent = Owner->GetComponentByClass<UAbilityComponent>())
	{
		for (auto& Tag : AbilityTags.GetGameplayTagArray())
		{
			AbilityComponent->ApplyAttributeEffects(Tag);
		}
	}
}

bool ABaseSkill::OnAttackStart()
{
	if (ActivateTiming != EAbilityTiming::AttackStart) return false;
	ApplyAttributeEffects();
	BP_OnAttackStart();
	return true;
}

bool ABaseSkill::OnAttackEffect()
{
	if (ActivateTiming != EAbilityTiming::AttackEffect) return false;
	ApplyAttributeEffects();
	BP_OnAttackEffect();
	return true;
}

bool ABaseSkill::OnAttackHit(AActor* Receiver)
{
	if (ActivateTiming != EAbilityTiming::AttackHit) return false;
	ApplyAttributeEffects();
	BP_OnAttackHit(Receiver);
	return true;
}

bool ABaseSkill::OnAttackFinish()
{
	if (ActivateTiming != EAbilityTiming::AttackFinish) return false;
	ApplyAttributeEffects();
	BP_OnAttackFinish();
	return true;
}

bool ABaseSkill::OnAttackDash()
{
	if (ActivateTiming != EAbilityTiming::AttackSkill) return false;
	ApplyAttributeEffects();
	BP_OnAttackDash();
	return true;
}

bool ABaseSkill::OnSkillStart()
{
	if (ActivateTiming != EAbilityTiming::SkillStart) return false;
	ApplyAttributeEffects();
	BP_OnSkillStart();
	return true;
}

bool ABaseSkill::OnSkillHit()
{
	if (ActivateTiming != EAbilityTiming::SkillHit) return false;
	ApplyAttributeEffects();
	BP_OnSkillHit();
	return true;
}

bool ABaseSkill::OnSkillFinish()
{
	if (ActivateTiming != EAbilityTiming::SkillFinish) return false;
	ApplyAttributeEffects();
	BP_OnSkillFinish();
	return true;
}

bool ABaseSkill::OnKillEnemy()
{
	if (ActivateTiming != EAbilityTiming::KillEnemy) return false;
	ApplyAttributeEffects();
	BP_OnKillEnemy();
	return true;
}

bool ABaseSkill::OnBeAttacked(AActor* Maker, int InDamage, int& OutDamage, bool& Stop)
{
	if (ActivateTiming != EAbilityTiming::BeAttacked) return false;
	ApplyAttributeEffects();
	return BP_OnBeAttacked(Maker, InDamage, OutDamage, Stop);
}

bool ABaseSkill::OnHitWeakPoint(AActor* Receiver)
{
	if (ActivateTiming != EAbilityTiming::AttackWeakPoint) return false;
	ApplyAttributeEffects();
	BP_OnHitWeakPoint(Receiver);
	return true;
}

bool ABaseSkill::OnBeAttackedInvulnerable()
{
	if (ActivateTiming != EAbilityTiming::BeAttackedInvulnerable) return false;
	ApplyAttributeEffects();
	BP_OnBeAttackedInvulnerable();
	return true;
}

bool ABaseSkill::OnBeDamaged()
{
	if (ActivateTiming != EAbilityTiming::BeDamaged) return false;
	ApplyAttributeEffects();
	BP_OnBeDamaged();
	return true;
}

bool ABaseSkill::OnJumpStart()
{
	if (ActivateTiming != EAbilityTiming::JumpStart) return false;
	ApplyAttributeEffects();
	BP_OnJumpStart();
	return true;
}

bool ABaseSkill::OnLanding()
{
	if (ActivateTiming != EAbilityTiming::Landing) return false;
	ApplyAttributeEffects();
	BP_OnLanding();
	return true;
}

bool ABaseSkill::OnDefenseStart()
{
	if (ActivateTiming != EAbilityTiming::DefenseStart) return false;
	ApplyAttributeEffects();
	BP_OnDefenseStart();
	return true;
}

bool ABaseSkill::OnDefenseSuccess()
{
	if (ActivateTiming != EAbilityTiming::DefenseSuccess) return false;
	ApplyAttributeEffects();
	BP_OnDefenseSuccess();
	return true;
}

bool ABaseSkill::OnDefenseFinish()
{
	if (ActivateTiming != EAbilityTiming::DefenseFinish) return false;
	ApplyAttributeEffects();
	BP_OnDefenseFinish();
	return true;
}

bool ABaseSkill::OnDying(int& RemReviveTimes)
{
	if (ActivateTiming != EAbilityTiming::Dying) return false;
	ApplyAttributeEffects();
	BP_OnDying(RemReviveTimes);
	return true;
}

bool ABaseSkill::OnPickGold()
{
	if (ActivateTiming != EAbilityTiming::PickGold) return false;
	ApplyAttributeEffects();
	BP_OnPickGold();
	return true;
}