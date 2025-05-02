// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/PixelAnimSubsystem.h"
#include "Anim/BasePixelAnimInstance.h"


template <typename T>
void UPixelAnimSubsystem::SetAnimInstanceProperty(UPaperZDAnimInstance* AnimInstance, const FName& PropertyName, T V)
{
	if (AnimInstance == nullptr) return;

	// UBasePixelAnimInstance* PixelAnimInstance = Cast<UBasePixelAnimInstance>(AnimInstance);
	// if (!IsValid(PixelAnimInstance)) return;
	
	UClass* Class = AnimInstance->GetClass();
	FProperty* Property = Class->FindPropertyByName(PropertyName);
	
	if constexpr (std::is_same_v<T, bool>)
	{
		if (Property && Property->IsA<FBoolProperty>())
		{
			const FBoolProperty* P = CastField<FBoolProperty>(Property);
			P->SetPropertyValue_InContainer(AnimInstance, V);
		}
	}
	else if constexpr (std::is_same_v<T, float>)
	{
		if (Property && Property->IsA<FFloatProperty>())
		{
			const FFloatProperty* P = CastField<FFloatProperty>(Property);
			P->SetPropertyValue_InContainer(AnimInstance, V);
		}
	}
	else if constexpr (std::is_same_v<T, int>)
	{
		if (Property && Property->IsA<FIntProperty>())
		{
			const FIntProperty* P = CastField<FIntProperty>(Property);
			P->SetPropertyValue_InContainer(AnimInstance, V);
		}
	}
}
