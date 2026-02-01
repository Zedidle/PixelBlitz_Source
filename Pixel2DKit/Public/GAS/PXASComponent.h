// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet/PXAttributeSet.h"
#include "PXASComponent.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXASComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

	TMap<FName, FGameplayTag> CacheTags;
	
public:
	
	virtual FGameplayEffectSpecHandle MakeOutgoingSpec(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level, FGameplayEffectContextHandle Context) const override;


	UFUNCTION(BlueprintCallable, Category = "PXASC")
	bool HasAbility(const FGameplayTag& Tag);
	
	UFUNCTION(BlueprintCallable, Category = "PXASC")
	bool TryActivateAbilities(const FGameplayTagContainer& GameplayTagContainer, FName CDTagName);

	UFUNCTION(BlueprintCallable, Category = "PXASC")
	bool TryActivateAbilityByTagName(FName TagName, bool bAllowRemoteActivation = true);

	UFUNCTION(BlueprintCallable, Category = "PXASC")
	bool TryActivateAbilityByTag(const FGameplayTag& Tag, bool bAllowRemoteActivation = true);
	
	
	// 声明安全的获取函数
	FGameplayAbilitySpec* GetAbilityByTagName(FName AbilityTagName);

	// 检测是否有某个Tag，一般用于检测冷却标记
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PXASC")
	bool HasTag(FName TagName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PXASC")
	bool IsAbilityInCD(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PXASC")
	float GetRemainingCD(const FGameplayTag& CooldownTag);
	
	UFUNCTION(BlueprintCallable, Category = "PXASC")
	void RemoveAbilityCD(const FGameplayTag& AbilityTag);


	template<typename T = UPXAttributeSet>
	void SetAttributeValue(EPXAttribute AttributeName, float Value)
	{
		if (const T* AttributeSet = GetSet<T>())
		{
			// 如果没有实现 GetAttribute 则会报错
			const FGameplayAttribute& Attribute = AttributeSet->GetAttribute(AttributeName);
			SetNumericAttributeBase(Attribute, Value);
		}
		
		// if (const T* AttributeSet = GetSet<T>())
		// {
		// 	T* MutableSet = const_cast<T*>(AttributeSet);
		// 	FGameplayAttribute Attribute = AttributeSet->GetAttribute(AttributeName);
		// 	if (FGameplayAttributeData* Data = Attribute.GetGameplayAttributeData(MutableSet))
		// 	{
		// 		// 几种设置值的方式都需要注意一下
		// 		// SetNumericAttributeBase
		// 		Data->SetBaseValue(Value);
		// 		Data->SetCurrentValue(Value);
		// 	}
		// }
	}
	template<typename T = UPXAttributeSet>
	void SetAttributeValueByName(const FString& AttributeName, float Value)
	{
		if (AttributeNameToEnumMap.Contains(AttributeName))
		{
			SetAttributeValue<T>(AttributeNameToEnumMap[AttributeName], Value);
		}
	}
	
	
	template<typename T = UPXAttributeSet>
	void ModifyAttributeValue(EPXAttribute AttributeName, float ModifyValue)
	{
		if (const T* AttributeSet = GetSet<T>())
		{
			const FGameplayAttribute& Attribute = AttributeSet->GetAttribute(AttributeName);
			float CurValue = Attribute.GetNumericValue(AttributeSet);
			SetNumericAttributeBase(Attribute, CurValue + ModifyValue);
		}
	}
	template<typename T = UPXAttributeSet>
	void ModifyAttributeValue(const FString& AttributeName, float Value)
	{
		if (AttributeNameToEnumMap.Contains(AttributeName))
		{
			ModifyAttributeValue<T>(AttributeNameToEnumMap[AttributeName], Value);
		}
	}


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PXASC", DisplayName = "GetAttributeValue")
	float BP_GetAttributeValue(EPXAttribute AttributeName);
	
	template<typename T = UPXAttributeSet> // 设置默认模板参数
	float GetAttributeValue(EPXAttribute AttributeName)
	{
		if (const T* AttributeSet = GetSet<T>()) // 使用模板参数T
		{
			const FGameplayAttribute& Attribute = AttributeSet->GetAttribute(AttributeName);
			return Attribute.GetNumericValue(AttributeSet);
		}
		return 0;
	}
	template<typename T = UPXAttributeSet>
	float GetAttributeValue(const FString& AttributeName)
	{
		if (AttributeNameToEnumMap.Contains(AttributeName))
		{
			return GetAttributeValue<T>(AttributeNameToEnumMap[AttributeName]);
		}
		return 0;
	}
};
