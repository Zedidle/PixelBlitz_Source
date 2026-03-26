// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Fight/Skills/BaseSkill.h"
#include "SKill_Dispel.generated.h"

UCLASS()
class PIXEL2DKIT_API ASKill_Dispel : public ABaseSkill
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASKill_Dispel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
	virtual bool OnAttackHit(AActor* Receiver) override;
};
