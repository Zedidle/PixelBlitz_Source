// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnhancedPlayerInput.h"
#include "PXPlayerInput.generated.h"


UCLASS(config = Input, transient)
class PIXEL2DKIT_API UPXPlayerInput : public UEnhancedPlayerInput
{
	GENERATED_BODY()

public:
	UPXPlayerInput();
	virtual ~UPXPlayerInput() override;

protected:
	//~ Begin UEnhancedPlayerInput Interface
	virtual bool InputKey(const FInputKeyEventArgs& Params) override;
	//~ End of UEnhancedPlayerInput interface

	void ProcessInputEventForLatencyMarker(const FInputKeyEventArgs& Params);
	void BindToLatencyMarkerSettingChange();
	void UnbindLatencyMarkerSettingChangeListener();
	void HandleLatencyMarkerSettingChanged();

	bool bShouldTriggerLatencyFlash = false;
};
