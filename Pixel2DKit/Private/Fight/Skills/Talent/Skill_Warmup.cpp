// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/Talent/Skill_Warmup.h"

#include "Pixel2DKit.h"
#include "Character/BasePXCharacter.h"
#include "Character/Components/AbilityComponent.h"
#include "Character/Components/BuffComponent.h"
#include "Subsystems/TimerManagerFuncLib.h"

#define LOCTEXT_NAMESPACE "PX"

// Sets default values
ASkill_Warmup::ASkill_Warmup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASkill_Warmup::BeginPlay()
{
	Super::BeginPlay();

	if (Owner)
	{
		OwnerPreLocation = GetOwner()->GetActorLocation();
	}

	UTimerManagerFuncLib::SetDelayLoopSafe(GetWorld(), "Ability.WarmUP",
	this, &ThisClass::MoveWarmingUP, 0.2);
}

// Called every frame
void ASkill_Warmup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASkill_Warmup::OnAttackFinish()
{
	if (WarmUP_Power <= 0) return false;
	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(Owner);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(PXCharacter, false)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(PXCharacter->BuffComponent, false)
	
	PXCharacter->BuffComponent->RemoveBuff(AbilityTag);
	WarmUP_Power = 0 ;
	WarmUP_MoveDistance = 0;
	OwnerPreLocation = GetOwner()->GetActorLocation();

	return true;
}

bool ASkill_Warmup::OnAttackSkillEnd()
{
	return OnAttackFinish();
}

void ASkill_Warmup::MoveWarmingUP()
{
	if (WarmUP_Power >= 4) return;

	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(Owner);
	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->BuffComponent);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->AbilityComponent);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC);
	
	WarmUP_MoveDistance += FVector::Distance(PXCharacter->GetActorLocation(), OwnerPreLocation);
	OwnerPreLocation = PXCharacter->GetActorLocation();

	FGameplayTag MoveDistancePerLevelTag = TAG("Ability.Warmup.Set.MoveDistancePerLevel");
	FGameplayTag AttackDamagePlusPercentTag = TAG("Ability.Warmup.Set.AttackDamagePlusPercent");
	FEffectGameplayTags& AbilityExtendData = PXCharacter->AbilityComponent->AbilityExtendData;
	
	if (!AbilityExtendData.Contains(MoveDistancePerLevelTag) ||
		!AbilityExtendData.Contains(AttackDamagePlusPercentTag)
	) return;

	if (WarmUP_MoveDistance < AbilityExtendData[MoveDistancePerLevelTag]) return;

	WarmUP_MoveDistance -= AbilityExtendData[MoveDistancePerLevelTag];
	WarmUP_Power ++;

	FText BuffNameFormat = LOCTEXT("Buff_Warmup", "热身{0}");

	PXCharacter->BuffComponent->AddAttributeEffect(AbilityTag,
		FAttributeEffect(EPXAttribute::CurAttackValue, AbilityExtendData[AttackDamagePlusPercentTag]));

	PXCharacter->BuffComponent->AddBuffOnWidget(AbilityTag, FText::Format(BuffNameFormat, WarmUP_Power).ToString(),
		FLinearColor(0.8, 0.1f, 0.1f, 1.0f), false);
}

#undef LOCTEXT_NAMESPACE
