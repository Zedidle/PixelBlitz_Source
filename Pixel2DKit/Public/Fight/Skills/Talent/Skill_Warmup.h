// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Fight/Skills/BaseSkill.h"
#include "Skill_Warmup.generated.h"

UCLASS()
class PIXEL2DKIT_API ASkill_Warmup : public ABaseSkill
{
	GENERATED_BODY()

	// 热身
	int WarmUP_Power = 0;
	int WarmUP_MoveDistance = 0;
	FVector OwnerPreLocation;
	
public:
	// Sets default values for this actor's properties
	ASkill_Warmup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool OnAttackFinish() override;
	virtual bool OnAttackSkillEnd() override;

	void MoveWarmingUP();
};
