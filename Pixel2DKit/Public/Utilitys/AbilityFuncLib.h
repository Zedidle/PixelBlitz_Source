// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PXCustomStruct.h"
#include "UObject/Object.h"
#include "AbilityFuncLib.generated.h"

struct FGameplayTag;
/**
 * 
 */
UCLASS(Abstract, MinimalAPI)
class UAbilityFuncLib : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category="AbilityFuncLib", meta =(WorldContext = "WorldContextObject"))
	static void UnlockAbility(UObject* WorldContextObject, const FGameplayTag& Tag);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AbilityFuncLib", meta =(WorldContext = "WorldContextObject"))
	static bool AbilityHadUnlock(UObject* WorldContextObject, const FGameplayTag& Tag);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AbilityFuncLib")
	static FAbility GetAbilityByTag(const FGameplayTag& Tag);



	
	UFUNCTION(BlueprintCallable, Category="AbilityFuncLib", meta =(WorldContext = "WorldContextObject"))
	static void UnlockTalent(UObject* WorldContextObject, const FGameplayTag& Tag);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AbilityFuncLib", meta =(WorldContext = "WorldContextObject"))
	static bool TalentHadUnlock(UObject* WorldContextObject, const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AbilityFuncLib")
	static FTalent GetTalentByTag(const FGameplayTag& Tag);

};
