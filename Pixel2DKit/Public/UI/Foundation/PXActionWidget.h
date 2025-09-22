// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActionWidget.h"
#include "PXActionWidget.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UCommonInputPlatformSettings;
class UInputAction;

UCLASS(BlueprintType, Blueprintable)
class PIXEL2DKIT_API UPXActionWidget : public UCommonActionWidget
{
	GENERATED_BODY()
public:

	//~ Begin UCommonActionWidget interface
	virtual FSlateBrush GetIcon() const override;
	//~ End of UCommonActionWidget interface

	/** The Enhanced Input Action that is associated with this Common Input action. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	const TObjectPtr<UInputAction> AssociatedInputAction;

private:

	UEnhancedInputLocalPlayerSubsystem* GetEnhancedInputSubsystem() const;
};
