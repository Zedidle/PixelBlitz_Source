// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Fight/Skills/BaseSkill.h"
#include "Skill_Revive.generated.h"

UCLASS()
class PIXEL2DKIT_API ASkill_Revive : public ABaseSkill
{
	GENERATED_BODY()

	int RemReviveTimes = 0;
	
public:
	// Sets default values for this actor's properties
	ASkill_Revive();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool OnDying(int& _RemReviveTimes) override;

};
