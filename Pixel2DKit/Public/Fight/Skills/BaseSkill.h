// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "BaseSkill.generated.h"


/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API ABaseSkill : public AActor
{
	GENERATED_BODY()

public:
	// 一般来说，每个技能都会有对应的Tag
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="BaseSkill")
	FGameplayTagContainer AbilityTags;
	
	UFUNCTION(BLueprintCallable, BlueprintNativeEvent, BlueprintPure, Category="Skills")
	bool CanDamageEffect(AActor* Actor); 

};
