// Fill out your copyright notice in the Description page of Project Settings.

#include "Pixel2DKit.h"
#include "Fight/Skills/Talent/Skill_MiracleWalker.h"
#include "Character/Components/AbilityComponent.h"
#include "Character/Components/BuffComponent.h"
#include "Subsystems/TimerManagerFuncLib.h"


// Sets default values
ASkill_MiracleWalker::ASkill_MiracleWalker()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASkill_MiracleWalker::BeginPlay()
{
	Super::BeginPlay();
	
	PrepareEffect();
}

// Called every frame
void ASkill_MiracleWalker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASkill_MiracleWalker::PrepareEffect()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner);
	
	UBuffComponent* BuffComponent = Owner->GetComponentByClass<UBuffComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffComponent);
	
	UAbilityComponent* AbilityComponent = Owner->GetComponentByClass<UAbilityComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilityComponent);
	
	if (!CachedASC.IsValid()) return;
	
	FGameplayTag MiracleWalkerTag = TAG("Ability.MiracleWalker");
	BuffComponent->RemoveBuff(MiracleWalkerTag);
	
	FGameplayTag DamagePlusTag = TAG("Ability.MiracleWalker.Set.DamagePlus");
	FGameplayTag IntervalTag = TAG("Ability.MiracleWalker.CD");

	if (!AbilityComponent->AbilityExtendData.Contains(DamagePlusTag) || 
		!AbilityComponent->AbilityExtendData.Contains(IntervalTag)) return;

	FName TimerName = FName(GetName() + "_MakeMiracleWalker");
	UTimerManagerFuncLib::SetRetriggerableDelay(GetWorld(), TimerName,
	[WeakThis = TWeakObjectPtr(this), DamagePlusTag, MiracleWalkerTag]
		{
			if (!WeakThis.IsValid()) return;
			AActor* Owner = WeakThis->GetOwner();
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner);
	
			UBuffComponent* BuffComponent = Owner->GetComponentByClass<UBuffComponent>();
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffComponent);
	
			UAbilityComponent* AbilityComponent = Owner->GetComponentByClass<UAbilityComponent>();
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilityComponent);

			FEffectGameplayTags& EffectGameplayTags = AbilityComponent->AbilityExtendData;
			if (EffectGameplayTags.Contains(DamagePlusTag))
			{
				if (BuffComponent)
				{
					BuffComponent->AddAttributeEffect( MiracleWalkerTag,
						FAttributeEffect(EPXAttribute::CurAttackValue, EffectGameplayTags[DamagePlusTag])
					);
					BuffComponent->AddBuffByTag(MiracleWalkerTag);
				}
			}
		}, AbilityComponent->AbilityExtendData[IntervalTag]);
}

bool ASkill_MiracleWalker::OnAttackEffect()
{
	if (!Super::OnAttackEffect()) return false;
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(Owner, false)
	
	PrepareEffect();
	
	return true;
}

