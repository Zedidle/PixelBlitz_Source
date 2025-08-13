// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSet/PXAttributeSet.h"

void UPXAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UPXAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return Super::PreGameplayEffectExecute(Data);
}

void UPXAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UPXAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if(Attribute == GetHPAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHP());
	}

	if(Attribute == GetEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxEnergy());
	}
	
}

void UPXAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	OnPXAttributeChange.Broadcast(Attribute, OldValue, NewValue);

	// if (Attribute == GetEnergyAttribute())
	// {
	// 	GetOwningActor()
	// }
	
	
}

bool UPXAttributeSet::GetAttrCurrentValueByName(FName AttrName, float& OutValue)
{
	auto* AttrData = GetGameplayAttributeData(AttrName);
	if (!AttrData) return false;
	OutValue = AttrData->GetCurrentValue();

	return true;
}

bool UPXAttributeSet::SetAttrCurrentValueByName(FName AttrName, float Value)
{
	auto* AttrData = GetGameplayAttributeData(AttrName);
	if (!AttrData) return false;
	AttrData->SetCurrentValue(Value);

	return true;	
}

bool UPXAttributeSet::GetAttrBaseValueByName(FName AttrName, float& OutValue)
{
	auto* AttrData = GetGameplayAttributeData(AttrName);
	if (!AttrData) return false;
	OutValue = AttrData->GetBaseValue();
	
	return true;
}

bool UPXAttributeSet::SetAttrBaseValueByName(FName AttrName, float Value)
{
	auto* AttrData = GetGameplayAttributeData(AttrName);
	if (!AttrData) return false;
	AttrData->SetBaseValue(Value);
	
	return true;	
}


FStructProperty* UPXAttributeSet::GetAttrStructProperty(FName AttrName) const
{
	const UClass* SelfCls = GetClass();
	if (SelfCls == nullptr) return nullptr;

	const FName PropertyName = FName(AttrName);
	FProperty* Property = SelfCls->FindPropertyByName(PropertyName);
	if (Property == nullptr) return nullptr;
	if (!Property->IsA(FStructProperty::StaticClass())) return nullptr;

	FStructProperty* StructProperty = CastField<FStructProperty>(Property);
	return StructProperty;
}

FGameplayAttributeData* UPXAttributeSet::GetGameplayAttributeData(FName AttrName)
{
	FStructProperty* StructProperty = GetAttrStructProperty(AttrName);
	if (!StructProperty) return nullptr;
	void* ValuePtr = StructProperty->ContainerPtrToValuePtr<void>(this);
	if (!ValuePtr) return nullptr;

	if (StructProperty->Struct != TBaseStructure<FGameplayAttributeData>::Get()) return nullptr;
	FGameplayAttributeData* GameplayAttributeData = static_cast<FGameplayAttributeData*>(ValuePtr);
	return GameplayAttributeData;
}

void UPXAttributeSet::OnRep_HP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, HP, OldValue);
}

void UPXAttributeSet::OnRep_MaxHP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, MaxHP, OldValue);
}

void UPXAttributeSet::OnRep_Energy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, Energy, OldValue);
}

void UPXAttributeSet::OnRep_MaxEnergy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPXAttributeSet, MaxEnergy, OldValue);
}
