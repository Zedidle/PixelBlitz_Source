// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/Talent/Skill_SwingFist.h"

#include "Pixel2DKit.h"
#include "Character/BasePXCharacter.h"
#include "Character/Components/AbilityComponent.h"
#include "Character/Components/BuffComponent.h"

#define LOCTEXT_NAMESPACE "PX"

// Sets default values
ASkill_SwingFist::ASkill_SwingFist()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASkill_SwingFist::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASkill_SwingFist::MakeSwingFistPower()
{
	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(GetOwner());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->BuffComponent)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->AbilityComponent)
	FEffectGameplayTags& AbilityExtendData = PXCharacter->AbilityComponent->AbilityExtendData;

	FGameplayTag CalTag = SwingFistPower ? TAG("Ability.SwingFist.Set.AttackDamageMinusPercent") : TAG("Ability.SwingFist.Set.AttackDamagePlusPercent");
	
	if (!AbilityExtendData.Contains(CalTag)) return;

	PXCharacter->BuffComponent->RemoveBuff(AbilityTag);
	
	FText BuffNameFormat = LOCTEXT("Buff_SwingFist", "摇摆拳{0}");

	FAttributeEffect Effect = FAttributeEffect(EPXAttribute::CurAttackValue, AbilityExtendData[CalTag]);
	PXCharacter->BuffComponent->AddAttributeEffect(AbilityTag, Effect);
	
	if (SwingFistPower)
	{
		PXCharacter->BuffComponent->AddBuffOnWidget(AbilityTag,  FText::Format(BuffNameFormat, FText::FromString(TEXT("↓"))).ToString(),
			FLinearColor(0.093059, 0.027321, 0.0, 1), false);
	}
	else
	{
		PXCharacter->BuffComponent->AddBuffOnWidget(AbilityTag,  FText::Format(BuffNameFormat, FText::FromString(TEXT("↑"))).ToString(),
			FLinearColor(1.0, 0.296138, 0.0, 1), false);
	}

	SwingFistPower = !SwingFistPower;
}

// Called every frame
void ASkill_SwingFist::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASkill_SwingFist::OnAttackFinish()
{
	MakeSwingFistPower();
	return true;
}

bool ASkill_SwingFist::OnAttackSkillEnd()
{
	MakeSwingFistPower();
	return true;
}


#undef LOCTEXT_NAMESPACE

