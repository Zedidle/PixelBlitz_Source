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
	
	FGameplayTag PlusTag = TAG("Ability.SwingFist.Set.AttackDamagePlusPercent");
	FGameplayTag MinusTag = TAG("Ability.SwingFist.Set.AttackDamageMinusPercent");

	if (!AbilityExtendData.Contains(PlusTag) || !AbilityExtendData.Contains(MinusTag)) return;

	SwingFistPower = !SwingFistPower;
	
	FGameplayTag SwingFistTag = TAG("Ability.SwingFist");
	// IBuff_Interface::Execute_RemoveBuff(PXCharacter, SwingFistTag, true);

	FText BuffNameFormat = LOCTEXT("Buff_SwingFist", "摇摆拳{0}");
	if (SwingFistPower)
	{
		PXCharacter->BuffComponent->AddAttributeEffect(SwingFistTag,
			FAttributeEffect(EPXAttribute::CurAttackValue, AbilityExtendData[MinusTag]));
		PXCharacter->BuffComponent->AddBuffOnWidget(SwingFistTag,  FText::Format(BuffNameFormat, FText::FromString(TEXT("↓"))).ToString(),
			FLinearColor(0.093059, 0.027321, 0.0, 1), false);
	}
	else
	{
		PXCharacter->BuffComponent->AddAttributeEffect(SwingFistTag,
			FAttributeEffect(EPXAttribute::CurAttackValue, AbilityExtendData[PlusTag], 0));
		PXCharacter->BuffComponent->AddBuffOnWidget(SwingFistTag,  FText::Format(BuffNameFormat, FText::FromString(TEXT("↑"))).ToString(),
			FLinearColor(1.0, 0.296138, 0.0, 1), false);
	}
}

// Called every frame
void ASkill_SwingFist::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASkill_SwingFist::OnAttackStart()
{
	if (Super::OnAttackStart())
	{
		MakeSwingFistPower();
		return true;
	}
	return false;
}


#undef LOCTEXT_NAMESPACE

