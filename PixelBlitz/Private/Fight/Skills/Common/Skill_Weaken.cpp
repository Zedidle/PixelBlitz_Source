// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/Common/Skill_Weaken.h"

#include "Character/Components/AbilityComponent.h"
#include "Character/Components/BuffComponent.h"


// Sets default values
ASkill_Weaken::ASkill_Weaken()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASkill_Weaken::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkill_Weaken::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASkill_Weaken::OnAttackHit(AActor* Receiver)
{
	if (!Owner) return false;
	UAbilityComponent* OwnerAbilityComponent = Owner->GetComponentByClass<UAbilityComponent>();
	if (!OwnerAbilityComponent) return false;
	
	FAttributeEffectArray EffectArray;
	if (OwnerAbilityComponent->FindAttributeEffects(AbilityTag, EffectArray))
	{
		if (UBuffComponent* ReceiverBuffComponent = Receiver->GetComponentByClass<UBuffComponent>())
		{
			ReceiverBuffComponent->AddAttributeEffects(AbilityTag, EffectArray.Data, true);
		}
	}
	return true;
}

