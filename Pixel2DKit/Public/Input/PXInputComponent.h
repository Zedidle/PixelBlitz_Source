// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Character/PXCharacterInputConfig.h"
#include "PXInputComponent.generated.h"


UCLASS(Config = Input)
class PIXEL2DKIT_API UPXInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

	UPROPERTY()
	UPXCharacterInputConfig* InputConfig;

public:
	
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
