// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Engine/DeveloperSettings.h"
#include "PXInputSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Input Settings"))
class PIXEL2DKIT_API UPXInputSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Custom Resource")
	TSoftObjectPtr<UInputMappingContext> Gamepad_IMC;

	UPROPERTY(Config, EditAnywhere, Category = "Custom Resource")
	TSoftObjectPtr<UInputMappingContext> Keyboard_IMC;
	
};
