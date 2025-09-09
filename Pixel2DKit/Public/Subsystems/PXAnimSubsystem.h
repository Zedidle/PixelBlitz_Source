// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Animation/BasePXAnimInstance.h"
#include "PXAnimSubsystem.generated.h"

class UPaperZDAnimInstance;


/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXAnimSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	public:
	
	template <typename T>
	static void SetAnimInstanceProperty(UPaperZDAnimInstance* AnimInstance, const FName& PropertyName, T V)
	{
		if (AnimInstance == nullptr) return;
	
		UClass* Class = AnimInstance->GetClass();

		// FindPropertyByName 据说是效率奇差，如何优化？
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
};

