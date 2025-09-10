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

	// 技能专属冷却标签
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
	FGameplayTagContainer CooldownTags; 

	// 临时容器：存储冷却 GE 标签与技能标签的并集
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags; 

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Cooldown")
	float GetCooldownDuration() const;
	
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;


	virtual const FGameplayTagContainer* GetCooldownTags() const override;


};
