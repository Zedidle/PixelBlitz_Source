// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/PXInputComponent.h"
#include "Pixel2DKit/Pixel2DKit.h"

UInputAction* UPXInputComponent::GetInputActionByTag(UPXCharacterInputConfig* InputConfig, const FGameplayTag& Tag)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(InputConfig, nullptr);

	if (InputConfig->ActionMap.Contains(Tag))
	{
		return InputConfig->ActionMap[Tag];
	}

	return nullptr;	
}

UInputAction* UPXInputComponent::GetInputActionByTag(const FGameplayTag& Tag)
{
	return GetInputActionByTag(InputConfig, Tag);
}

void UPXInputComponent::SetInputConfig(UPXCharacterInputConfig* _InputConfig)
{
	InputConfig = _InputConfig;
}

// void UPXInputComponent::BindActionByTag(const FGameplayTag& Tag, ETriggerEvent TriggerEvent, UObject* Object,
// 	FName FunctionName)
// {
//
// 	if (UInputAction* Action = GetInputActionByTag(Tag))
// 	{
// 		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0f, FColor(255,48,16), "UPXInputComponent::BindActionByTag Success!");
// 		BindAction(Action, TriggerEvent, Object, FunctionName);
// 	}
// }

// void UPXInputComponent::BindActionByTagName(const FName& TagName, ETriggerEvent TriggerEvent, UObject* Object,
// 	FName FunctionName)
// {
// 	BindActionByTag(FGameplayTag::RequestGameplayTag(TagName), TriggerEvent, Object, FunctionName);
// }
