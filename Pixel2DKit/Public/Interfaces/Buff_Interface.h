// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "Buff_Interface.generated.h"


UINTERFACE(MinimalAPI, Blueprintable)
class UBuff_Interface : public UInterface
{
	GENERATED_BODY()
};

class PIXEL2DKIT_API IBuff_Interface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Buff_Interface | BuffEffect")
	void BuffEffect_Speed(FGameplayTag Tag, float Percent, float Value, float SustainTime);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Buff_Interface | BuffEffect")
	void BuffUpdate_Speed();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Buff_Interface | BuffEffect")
	void BuffEffect_Attack(FGameplayTag Tag, float Percent, int32 Value, float SustainTime);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Buff_Interface | BuffEffect")
	void BuffUpdate_Attack();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Buff_Interface | BuffEffect")
	void BuffEffect_Sight(FGameplayTag Tag, float Percent, float Value, float SustainTime);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Buff_Interface | BuffEffect")
	void BuffUpdate_Sight();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Buff_Interface | BuffEffect")
	int32 Buff_CalDamage(int32 InDamage);


	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Buff_Interface | BuffText")
	void AddBuff(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor, bool Permanent);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Buff_Interface | BuffText")
	void RemoveBuff(FGameplayTag Tag);


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Buff_Interface | BuffResistance")
	float GetShortSightResistancePercent();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Buff_Interface | BuffResistance")
	float GetSlowDownResistancePercent();
	
};