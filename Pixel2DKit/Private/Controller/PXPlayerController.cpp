// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PXPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Character/BasePXCharacter.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXGameDataAsset.h"

class UCustomConfigSettings;
class UEnhancedInputLocalPlayerSubsystem;

void APXPlayerController::BeginPlay()
{
	Super::BeginPlay();
	PXCharacter = Cast<ABasePXCharacter>(GetPawn());

	if (const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>())
	{
		if (const UPXGameDataAsset* GameDataAsset = Settings->GameDataAsset.LoadSynchronous())
		{
			IMC_Default = GameDataAsset->IMC_Default.LoadSynchronous();
		}
	}
}

void APXPlayerController::OnGameStart()
{
	GameAlreadyStart = true;
	OnCharacterControl(true);

	SetShowMouseCursor(false);
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	
	if (PXCharacter)
	{
		PXCharacter->ReadyToStart();
	}
}

void APXPlayerController::OnCharacterControl(bool On)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Subsystem);

	FModifyContextOptions Options;
	Options.bNotifyUserSettings = true;
	
	CharacterControlling = On;
	if (On)
	{
		Subsystem->AddMappingContext(IMC_Default, 1, Options);
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
	else
	{
		Subsystem->RemoveMappingContext(IMC_Default);
		FInputModeUIOnly InputMode;
		SetInputMode(InputMode);
	}
	FlushPressedKeys();
}

bool APXPlayerController::CanPause()
{
	return CharacterControlling && GameAlreadyStart;
}
