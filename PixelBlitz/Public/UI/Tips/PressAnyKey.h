// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Misc/GameSettingPressAnyKey.h"
#include "PressAnyKey.generated.h"

/**
 * 
 */
UCLASS()
class PIXELBLITZ_API UPressAnyKey : public UGameSettingPressAnyKey
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

public:
	UFUNCTION(BlueprintNativeEvent)
	void HandleKeyPressed(FKey InKey);

};
