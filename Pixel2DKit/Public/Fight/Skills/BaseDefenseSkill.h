// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Fight/Skills/BaseSkill.h"
#include "BaseDefenseSkill.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API ABaseDefenseSkill : public ABaseSkill
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefenseSkill")
	FGameplayTagContainer AbilityTags;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefenseSkill")
	int Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefenseSkill")
	bool StopPropagation = false;
	

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DefenseSkill")
	void OnBeAttacked(AActor* DamageMaker, int InDamage, int& OutDamage, bool& Stop);
	

};
