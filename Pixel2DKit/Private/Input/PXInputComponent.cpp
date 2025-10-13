// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/PXInputComponent.h"
#include "Pixel2DKit/Pixel2DKit.h"

UPXInputComponent::UPXInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UPXInputComponent::AddInputMappings(const UPXInputConfig* InputConfig,
                                         UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
}

void UPXInputComponent::RemoveInputMappings(const UPXInputConfig* InputConfig,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
}

void UPXInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
}

UInputAction* UPXInputComponent::GetInputActionByTag(UPXCharacterInputConfig* InputConfig, const FGameplayTag& Tag)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(InputConfig, nullptr);

	return InputConfig->ActionMap.FindRef(Tag);
}

UInputAction* UPXInputComponent::GetInputActionByTag(const FGameplayTag& Tag)
{
	return GetInputActionByTag(RecordInputConfig, Tag);
}

void UPXInputComponent::SetInputConfig(UPXCharacterInputConfig* _InputConfig)
{
	RecordInputConfig = _InputConfig;
}
