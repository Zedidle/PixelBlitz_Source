// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "PXAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPixelAttributeChangeDelegate, const FGameplayAttribute&, Attribute, float,  OldValue, float, NewValue);



UCLASS()
class PIXEL2DKIT_API UPXAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;


public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetAttrCurrentValueByName(FName AttrName, float& OutValue);
	
	UFUNCTION(BlueprintCallable)
	bool SetAttrCurrentValueByName(FName AttrName, float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetAttrBaseValueByName(FName AttrName, float& OutValue);

	UFUNCTION(BlueprintCallable)
	bool SetAttrBaseValueByName(FName AttrName, float Value);
	

private:
	FORCEINLINE FStructProperty* GetAttrStructProperty(FName AttrName) const;
	FORCEINLINE FGameplayAttributeData* GetGameplayAttributeData(FName AttrName);
	
	
public:
	UPROPERTY(BlueprintAssignable, Category = "AttributeSet|Prop")
	FOnPixelAttributeChangeDelegate OnPXAttributeChange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_HP)
	FGameplayAttributeData HP;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, HP);

	UFUNCTION()
	virtual void OnRep_HP(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_MaxHP)
	FGameplayAttributeData MaxHP;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, MaxHP);

	UFUNCTION()
	virtual void OnRep_MaxHP(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", Replicated)
	FGameplayAttributeData HPIncrease;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, HPIncrease);

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_Energy)
	FGameplayAttributeData EP;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, EP);

	UFUNCTION()
	virtual void OnRep_Energy(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_MaxEnergy)
	FGameplayAttributeData MaxEP;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, MaxEP);

	UFUNCTION()
	virtual void OnRep_MaxEnergy(const FGameplayAttributeData& OldValue);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", Replicated)
	FGameplayAttributeData EPIncrease;
	ATTRIBUTE_ACCESSORS(UPXAttributeSet, EPIncrease);


	
};
