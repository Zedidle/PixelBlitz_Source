// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PXASComponent.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXASComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	
	virtual FGameplayEffectSpecHandle MakeOutgoingSpec(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level, FGameplayEffectContextHandle Context) const override;

};
