// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PXGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	// 冷却时长（可随等级缩放）
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
	FScalableFloat CooldownDuration; 

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cost")
	TMap<FGameplayTag, FScalableFloat> Cost;

	
	// 技能专属冷却标签
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
	FGameplayTagContainer CooldownTags; 
	
	UPROPERTY(Transient)
	mutable FGameplayTagContainer TempCooldownTags;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Cooldown")
	float GetCooldownDuration() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Cost")
	float GetCostEP() const;
	
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	
	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PXGameplayAbility | GAS")
	APawn* GetPawn() const;


};
