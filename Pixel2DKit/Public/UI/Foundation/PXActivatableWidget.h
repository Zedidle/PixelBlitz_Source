// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PXActivatableWidget.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

	// 对应 UIConfig
	
	

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual bool NativeOnHandleBackAction() override;


public:
	FName UIName;

	UFUNCTION()
	void SetUIName(FName Name){ UIName = Name; }

	
};
