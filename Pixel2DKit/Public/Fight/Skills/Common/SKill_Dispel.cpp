// Fill out your copyright notice in the Description page of Project Settings.


#include "SKill_Dispel.h"

#include "Character/Components/AbilityComponent.h"
#include "Enemy/BaseEnemy.h"


// Sets default values
ASKill_Dispel::ASKill_Dispel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASKill_Dispel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASKill_Dispel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASKill_Dispel::OnAttackHit(AActor* Receiver)
{
	if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(Receiver))
	{
		UAbilityComponent* AbilityComponent = Owner->GetComponentByClass<UAbilityComponent>();
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(AbilityComponent, false);

		FGameplayTag DispelBasicRate = TAG("Ability.Dispel.Set.BasicRate");

		float BasicRate;
		if (AbilityComponent->FindExtendData(DispelBasicRate, BasicRate))
		{
			Enemy->OnDispel(SkillLevel, BasicRate);
		}
	}
	
	return true;
}

