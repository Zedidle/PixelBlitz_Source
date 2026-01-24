// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/Talent/Skill_EvadeStrike.h"

#include "Pixel2DKit.h"
#include "Character/BasePXCharacter.h"
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
	if (!Super::OnSkillFinish()) return false;

	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(Owner);
	if (!PXCharacter) return false;
	if (!PXCharacter->BuffComponent) return false;
	
	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	// 闪避突袭
	FGameplayTag Tag_DamagePlusAfterSkill = TAG("CommonSet.DamagePlusAfterDash");
	if (EffectGameplayTags.Contains(Tag_DamagePlusAfterSkill))
	{
		FGameplayTag Tag = TAG("Ability.DodgeStrike");
		PXCharacter->BuffComponent->AddAttributeEffect(Tag,
			FAttributeEffect(EPXAttribute::CurAttackValue, 0.0f,EffectGameplayTags[Tag_DamagePlusAfterSkill]));
		PXCharacter->BuffComponent->AddBuffByTag(Tag);
	}

	return true;
}

