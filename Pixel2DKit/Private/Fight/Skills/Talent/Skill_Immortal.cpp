// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/Talent/Skill_Immortal.h"

#include "Pixel2DKit.h"
#include "Character/BasePXCharacter.h"
#include "Character/Components/AbilityComponent.h"
#include "Character/Components/BuffComponent.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Subsystems/TimerManagerFuncLib.h"


class UAbilityComponent;
class UBuffComponent;
// Sets default values
ASkill_Immortal::ASkill_Immortal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASkill_Immortal::BeginPlay()
{
	Super::BeginPlay();
	BeginPrepare();
}

// Called every frame
void ASkill_Immortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASkill_Immortal::BeginPrepare()
{
	bReady = false;
	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)
	
	UBuffComponent* BuffComponent = Owner->GetComponentByClass<UBuffComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffComponent);
	
	UAbilityComponent* AbilityComponent = Owner->GetComponentByClass<UAbilityComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilityComponent);
	
	UStateComponent* StateComponent = Owner->GetComponentByClass<UStateComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(StateComponent);
	
	if (!CachedASC.IsValid()) return;
	
	FGameplayTag ImmortalPowerTag = TAG("Ability.Immortal");
	FGameplayTag AttackDamagePlusOnMaxHPPercentTag = TAG("Ability.Immortal.Set.AttackDamagePlusOnMaxHPPercent");
	FGameplayTag CDTag = TAG("Ability.Immortal.CD");
	
	UTimerManagerFuncLib::SetDelay(GetWorld(),
[WeakThis = TWeakObjectPtr(this), AttackDamagePlusOnMaxHPPercentTag, ImmortalPowerTag]
		{
			if (!WeakThis.IsValid()) return;
			if (!WeakThis->Owner) return;
				
			UBuffComponent* BuffComponent = WeakThis->Owner->GetComponentByClass<UBuffComponent>();
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffComponent);
						
			UAbilityComponent* AbilityComponent = WeakThis->Owner->GetComponentByClass<UAbilityComponent>();
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilityComponent);
					
			UStateComponent* StateComponent = WeakThis->Owner->GetComponentByClass<UStateComponent>();
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN(StateComponent);
				
			BuffComponent->AddAttributeEffect( ImmortalPowerTag,
				FAttributeEffect(EPXAttribute::CurAttackValue, 0.0f,
					FMath::RoundToInt(StateComponent->GetMaxHP() * AbilityComponent->AbilityExtendData[AttackDamagePlusOnMaxHPPercentTag])));
					
			WeakThis->bReady = true;
			BuffComponent->AddBuffByTag(ImmortalPowerTag);
		
		}, AbilityComponent->AbilityExtendData[CDTag]);
}

void ASkill_Immortal::MakeEffect()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Owner)
	
	UBuffComponent* BuffComponent = Owner->GetComponentByClass<UBuffComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffComponent);
	
	UAbilityComponent* AbilityComponent = Owner->GetComponentByClass<UAbilityComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AbilityComponent);
	
	UStateComponent* StateComponent = Owner->GetComponentByClass<UStateComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(StateComponent);
	
	if (!CachedASC.IsValid()) return;
	
	FGameplayTag ImmortalPowerTag = TAG("Ability.Immortal");
	FGameplayTag MaxHPPlusAfterAttackTag = TAG("Ability.Immortal.Set.MaxHPPlusAfterAttack");
	
	if (!AbilityComponent->AbilityExtendData.Contains(MaxHPPlusAfterAttackTag)) return;

	// 判断是否就绪
	if (bReady)
	{
		// 不灭中，恢复生命值的部分
		UGameInstance* GameInstance = Owner->GetGameInstance();
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);
		
		UPXMainSaveGame* MainSaveGame= UPXSaveGameSubSystemFuncLib::GetMainData(GameInstance->GetWorld());
		
		MainSaveGame->CharacterInheritAttribute.BasicMaxHP ++;
		
		float HPEffect = AbilityComponent->AbilityExtendData[MaxHPPlusAfterAttackTag];
		
		BuffComponent->RemoveBuff(ImmortalPowerTag, true);
		BuffComponent->AddAttributeEffect(ImmortalPowerTag, EPXAttribute::BasicMaxHP, 0.0f , HPEffect);
		StateComponent->IncreaseHP(HPEffect, Owner);
		
		BeginPrepare();
	}
}

bool ASkill_Immortal::OnAttackEffect()
{
	if (!Super::OnAttackEffect()) return false;
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(Owner, false)
	
	MakeEffect();
	
	return true;
}

