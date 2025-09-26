// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UserSettings/EnhancedInputUserSettings.h"
#include "PlayerMappableKeySettings.h"
#include "PXInputUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXInputUserSettings : public UEnhancedInputUserSettings
{
	GENERATED_BODY()public:
	//~ Begin UEnhancedInputUserSettings interface
	virtual void ApplySettings() override;
	//~ End UEnhancedInputUserSettings interface

	// Add any additional Input Settings here!
	// Some ideas could be:
	// - "toggle vs. hold" to trigger in game actions
	// - aim sensitivity should go here
	// - etc

	// Make sure to mark your properties with the "SaveGame" metadata to have them serialize when saved
	//UPROPERTY(SaveGame, BlueprintReadWrite, Category="Enhanced Input|User Settings")
	// bool bSomeExampleProperty;
};
