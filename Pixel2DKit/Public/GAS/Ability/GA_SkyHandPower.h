// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/PXGameplayAbility.h"
#include "GA_SkyHandPower.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UGA_SkyHandPower : public UPXGameplayAbility
{
	GENERATED_BODY()


	protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
