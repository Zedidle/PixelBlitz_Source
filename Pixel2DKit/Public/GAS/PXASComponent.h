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

	TMap<FName, FGameplayTag> TagCache;
	
public:
	
	virtual FGameplayEffectSpecHandle MakeOutgoingSpec(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level, FGameplayEffectContextHandle Context) const override;

	UFUNCTION(BlueprintCallable, Category = "PXASC")
	bool TryActivateAbilities(const FGameplayTagContainer& GameplayTagContainer, FName CDTagName);

	UFUNCTION(BlueprintCallable, Category = "PXASC")
	bool TryActivateAbilityByTagName(FName TagName, bool bAllowRemoteActivation = true);
	
	// 声明安全的获取函数
	FGameplayAbilitySpec* GetAbilityByTagName(FName AbilityTagName);

	// 检测是否有某个Tag，一般用于检测冷却标记
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PXASC")
	bool HasTag(FName TagName);
};
