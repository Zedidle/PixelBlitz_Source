// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/PXInputConfig.h"
#include "Character/PXCharacterInputConfig.h"
#include "PXInputComponent.generated.h"


UCLASS(Config = Input)
class UPXInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

	UPROPERTY()
	UPXCharacterInputConfig* RecordInputConfig;

public:

	UPXInputComponent(const FObjectInitializer& ObjectInitializer);

	void AddInputMappings(const UPXInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMappings(const UPXInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UPXInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UPXInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);



	
	static UInputAction* GetInputActionByTag(UPXCharacterInputConfig* InputConfig, const FGameplayTag& Tag);
	UInputAction* GetInputActionByTag(const FGameplayTag& Tag);

	void SetInputConfig(UPXCharacterInputConfig* _InputConfig);
	
	template<class UserClass>
	void BindActionByTag(const FGameplayTag& Tag, ETriggerEvent TriggerEvent, UserClass* Object, FEnhancedInputActionHandlerSignature::TMethodPtr< UserClass > Func)
	{
		if (UInputAction* Action = GetInputActionByTag(Tag))
		{
			BindAction(Action, TriggerEvent, Object, Func);
		}
	}
	template<class UserClass>
	void BindActionByTag(const FGameplayTag& Tag, ETriggerEvent TriggerEvent, UserClass* Object, FEnhancedInputActionHandlerValueSignature::TMethodPtr< UserClass > Func)
	{
		if (UInputAction* Action = GetInputActionByTag(Tag))
		{
			BindAction(Action, TriggerEvent, Object, Func);
		}
	}

	
	template<class UserClass>
	void BindActionByTagName(const FName& TagName, ETriggerEvent TriggerEvent, UserClass* Object, FEnhancedInputActionHandlerSignature::TMethodPtr< UserClass > Func)
	{
		BindActionByTag(FGameplayTag::RequestGameplayTag(TagName), TriggerEvent, Object, Func);
	}

	// 应对 ETriggerEvent::Triggered 有一个参数
	template<class UserClass>
	void BindActionByTagName(const FName& TagName, ETriggerEvent TriggerEvent, UserClass* Object, FEnhancedInputActionHandlerValueSignature::TMethodPtr< UserClass > Func)
	{
		BindActionByTag(FGameplayTag::RequestGameplayTag(TagName), TriggerEvent, Object, Func);
	}

};


template<class UserClass, typename FuncType>
void UPXInputComponent::BindNativeAction(const UPXInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UPXInputComponent::BindAbilityActions(const UPXInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FPXInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}
