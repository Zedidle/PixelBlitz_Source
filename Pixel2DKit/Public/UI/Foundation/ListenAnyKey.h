// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PXActivatableWidget.h"
#include "ListenAnyKey.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPressAnyKey, FKey, Key);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReleasAnyKey, FKey, Key);

UCLASS(Abstract)
class PIXEL2DKIT_API UListenAnyKey : public UPXActivatableWidget
{
	GENERATED_BODY()


public:
	UListenAnyKey(const FObjectInitializer& Initializer);

	UPROPERTY(BlueprintAssignable)
	FOnPressAnyKey OnKeyPressed;

	UPROPERTY(BlueprintAssignable)
	FOnReleasAnyKey OnKeyReleased;
	
	DECLARE_EVENT(Event_OnPressAnyKey, FOnKeySelectionCanceled);
	FOnKeySelectionCanceled OnKeySelectionCanceled;

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	void HandleKeyPressed(FKey InKey);
	void HandleKeyReleased(FKey InKey);

	UFUNCTION(BlueprintCallable)
	void HandleKeyExit();

private:
	TSharedPtr<class FListenAnyKeyInputPreProcessor> InputProcessor;
};
