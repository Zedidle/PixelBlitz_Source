// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Foundation/PXActivatableWidget.h"
#include "PXHUDLayout.generated.h"

class UCommonActivatableWidget;
class UObject;

UCLASS(Abstract, BlueprintType, Blueprintable, Meta = (DisplayName = "PX HUD Layout", Category = "PX|HUD"))
class PIXEL2DKIT_API UPXHUDLayout : public UPXActivatableWidget
{
	GENERATED_BODY()

public:

	UPXHUDLayout(const FObjectInitializer& ObjectInitializer);

	void NativeOnInitialized() override;

protected:
	void HandleEscapeAction();

	UPROPERTY(EditDefaultsOnly)
	FName EscapeWidgetName;

	
};
