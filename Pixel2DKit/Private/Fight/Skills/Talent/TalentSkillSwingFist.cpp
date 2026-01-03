// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/Talent/TalentSkillSwingFist.h"

#include "Pixel2DKit.h"
#include "Character/BasePXCharacter.h"

#define LOCTEXT_NAMESPACE "PX"

// Sets default values
ATalentSkillSwingFist::ATalentSkillSwingFist()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATalentSkillSwingFist::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATalentSkillSwingFist::MakeSwingFistPower()
{
	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(GetOwner());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	
	FGameplayTag DecreaseTag = TAG("TalentSet.SwingPunch.AttackDamage_DecreasePercent");
	FGameplayTag IncreaseTag = TAG("TalentSet.SwingPunch.AttackDamage_IncreasePercent");

	if (!EffectGameplayTags.Contains(DecreaseTag) || !EffectGameplayTags.Contains(IncreaseTag)) return;
	if (!PXCharacter->Implements<UBuff_Interface>()) return;

	SwingFistPower = !SwingFistPower;
	
	FGameplayTag SwingFistTag = TAG("Buff.Talent.SwingFist");
	IBuff_Interface::Execute_RemoveBuff(PXCharacter, SwingFistTag, true);

	FText BuffNameFormat = LOCTEXT("Buff_SwingFist", "摇摆拳{0}");
	if (SwingFistPower)
	{
		IBuff_Interface::Execute_BuffEffect_Attack(PXCharacter, SwingFistTag, EffectGameplayTags[IncreaseTag], 0, 999);
		IBuff_Interface::Execute_AddBuff(PXCharacter, SwingFistTag,  FText::Format(BuffNameFormat, FText::FromString(TEXT("↓"))).ToString(),
			FLinearColor(0.093059, 0.027321, 0.0, 1), false);
	}
	else
	{
		IBuff_Interface::Execute_BuffEffect_Attack(PXCharacter, SwingFistTag, EffectGameplayTags[DecreaseTag], 0, 999);
		IBuff_Interface::Execute_AddBuff(PXCharacter, SwingFistTag,  FText::Format(BuffNameFormat, FText::FromString(TEXT("↑"))).ToString(),
			FLinearColor(1.0, 0.296138, 0.0, 1), false);
	}
}

// Called every frame
void ATalentSkillSwingFist::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ATalentSkillSwingFist::OnAttackStart()
{
	if (Super::OnAttackStart())
	{
		MakeSwingFistPower();
		return true;
	}
	return false;
}


#undef LOCTEXT_NAMESPACE

