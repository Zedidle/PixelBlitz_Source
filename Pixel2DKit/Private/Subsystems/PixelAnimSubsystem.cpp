// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/PixelAnimSubsystem.h"
#include "Anim/BasePixelAnimInstance.h"

void UPixelAnimSubsystem::SetAnimInstanceProperty(UPaperZDAnimInstance* AnimInstance, const FName& PropertyName, bool V)
{
	if (AnimInstance == nullptr) return;
	
	UClass* Class = AnimInstance->GetClass();
	FProperty* Property = Class->FindPropertyByName(PropertyName);
	if (Property && Property->IsA<FBoolProperty>()) {
		FBoolProperty* P = CastField<FBoolProperty>(Property);
		P->SetPropertyValue_InContainer(AnimInstance, V);
		UE_LOG(LogTemp, Warning, TEXT("UPixelAnimSubsystem::SetAnimInstanceProperty, Set %s to %f"), *PropertyName.ToString(), V);
	} else {
		UE_LOG(LogTemp, Error, TEXT("UPixelAnimSubsystem::SetAnimInstanceProperty, Property %s not found or type mismatch!"), *PropertyName.ToString());
	}
}

void UPixelAnimSubsystem::SetAnimInstanceProperty(UPaperZDAnimInstance* AnimInstance, const FName& PropertyName,
	float V)
{
	
}

void UPixelAnimSubsystem::SetAnimInstanceProperty(UPaperZDAnimInstance* AnimInstance, const FName& PropertyName, int V)
{
	
}
