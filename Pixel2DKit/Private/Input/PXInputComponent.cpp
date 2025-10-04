// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/PXInputComponent.h"
#include "Pixel2DKit/Pixel2DKit.h"

UInputAction* UPXInputComponent::GetInputActionByTag(UPXCharacterInputConfig* InputConfig, const FGameplayTag& Tag)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(InputConfig, nullptr);

	return InputConfig->ActionMap.FindRef(Tag);
}

UInputAction* UPXInputComponent::GetInputActionByTag(const FGameplayTag& Tag)
{
	return GetInputActionByTag(InputConfig, Tag);
}

void UPXInputComponent::SetInputConfig(UPXCharacterInputConfig* _InputConfig)
{
	InputConfig = _InputConfig;
}
