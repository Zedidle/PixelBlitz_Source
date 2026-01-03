// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Fight/Skills/BaseSkill.h"
#include "TalentSkillSwingFist.generated.h"

UCLASS()
class PIXEL2DKIT_API ATalentSkillSwingFist : public ABaseSkill
{
	GENERATED_BODY()


	// 摇摆拳增强状态
	bool SwingFistPower = false;
	
public:
	// Sets default values for this actor's properties
	ATalentSkillSwingFist();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION()
	void MakeSwingFistPower();
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool OnAttackStart() override;
	
};
