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

	UFUNCTION(BlueprintCallable, Category = "PXASC")
	bool TryActivateAbilities(const FGameplayTagContainer& GameplayTagContainer, FName CDTagName);

	// 声明安全的获取函数
	TWeakObjectPtr<UGameplayAbility> GetAbilityByTag(FGameplayTag AbilityTag);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PXASC")
	bool HasTag(FName TagName);
};
