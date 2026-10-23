// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/Talent/Skill_GoldHunter.h"

#include "Fight/Components/StateComponent.h"


// Sets default values
ASkill_GoldHunter::ASkill_GoldHunter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASkill_GoldHunter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkill_GoldHunter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASkill_GoldHunter::OnPickGold()
{
	if (!Super::OnPickGold()) return false;
	if (!Owner) return false;

	return true;
}

