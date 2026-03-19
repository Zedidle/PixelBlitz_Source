// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/Talent/Skill_EvadeStrike.h"

#include "Pixel2DKit.h"
#include "Character/BasePXCharacter.h"
#include "Character/Components/AbilityComponent.h"
#include "Character/Components/BuffComponent.h"


// Sets default values
ASkill_EvadeStrike::ASkill_EvadeStrike()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASkill_EvadeStrike::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkill_EvadeStrike::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASkill_EvadeStrike::OnSkillFinish()
{
	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(Owner);
	if (!PXCharacter) return false;
	if (!PXCharacter->BuffComponent) return false;
	if (!PXCharacter->AbilityComponent) return false;
	if (!PXCharacter->CachedASC) return false;
	if (PXCharacter->BuffComponent->BuffExist(AbilityTag)) return false;
	
	if (!Super::OnSkillFinish()) return false;
	
	float CurDamagePlusAfterDashValue = PXCharacter->CachedASC->GetAttributeValue(EPXAttribute::CurDamagePlusAfterDash);
	FAttributeEffect Effect = FAttributeEffect(EPXAttribute::CurAttackValue, 0.0f, CurDamagePlusAfterDashValue);
	
	PXCharacter->BuffComponent->AddAttributeEffect(AbilityTag, Effect);
	PXCharacter->BuffComponent->AddBuffByTag(AbilityTag);

	return true;
}

bool ASkill_EvadeStrike::OnAttackFinish()
{
	RemoveSelfBuff();
	return true;
}

