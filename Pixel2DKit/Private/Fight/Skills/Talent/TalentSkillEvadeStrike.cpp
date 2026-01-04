// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/Talent/TalentSkillEvadeStrike.h"

#include "Pixel2DKit.h"
#include "Character/BasePXCharacter.h"
#include "Character/Components/BuffComponent.h"


// Sets default values
ATalentSkillEvadeStrike::ATalentSkillEvadeStrike()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATalentSkillEvadeStrike::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATalentSkillEvadeStrike::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ATalentSkillEvadeStrike::OnSkillFinish()
{
	if (!Super::OnSkillFinish()) return false;

	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(Owner);
	if (!PXCharacter || !PXCharacter->Implements<UBuff_Interface>()) return false;

	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	// 闪避突袭
	FGameplayTag Tag_DamagePlusAfterSkill = TAG("TalentSet.DamagePlusAfterSkill");
	if (EffectGameplayTags.Contains(Tag_DamagePlusAfterSkill))
	{
		FGameplayTag Tag = TAG("Buff.Talent.DodgeStrike");
		IBuff_Interface::Execute_BuffEffect_Attack(PXCharacter, Tag, 0.0f,
			EffectGameplayTags[Tag_DamagePlusAfterSkill], 999.0f
		);

		if (PXCharacter->BuffComponent)
		{
			PXCharacter->BuffComponent->AddBuffByTag(Tag);
		}
	}

	return true;
}

