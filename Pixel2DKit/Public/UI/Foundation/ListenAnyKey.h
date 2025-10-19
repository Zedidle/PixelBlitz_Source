// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PXActivatableWidget.h"
#include "ListenAnyKey.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPressAnyKey, FKey, Name);

UCLASS(Abstract)
class PIXEL2DKIT_API UListenAnyKey : public UPXActivatableWidget
{
	GENERATED_BODY()


public:
	UListenAnyKey(const FObjectInitializer& Initializer);

	// DECLARE_EVENT_OneParam(Event_OnPressAnyKey, FOnKeySelected, FKey);
	UPROPERTY(BlueprintAssignable)
	FOnPressAnyKey OnKeyPressed;

	DECLARE_EVENT(Event_OnPressAnyKey, FOnKeySelectionCanceled);
	FOnKeySelectionCanceled OnKeySelectionCanceled;

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	void HandleKeyPressed(FKey InKey);
	void HandleKeyExit();

private:
	TSharedPtr<class FListenAnyKeyInputPreProcessor> InputProcessor;
};
