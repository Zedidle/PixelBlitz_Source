// Fill out your copyright notice in the Description page of Project Settings.


#include "Basic/PXPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Character/BasePXCharacter.h"
#include "Character/PXCharacterDataAsset.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/PXInputSettings.h"

class UPXInputSettings;
class UEnhancedInputLocalPlayerSubsystem;

void APXPlayerController::BeginPlay()
{
	Super::BeginPlay();
	PXCharacter = Cast<ABasePXCharacter>(GetPawn());

	if (const UPXInputSettings* Settings = GetDefault<UPXInputSettings>())
	{
		IMC_GamePad = Settings->Gamepad_IMC.LoadSynchronous();
		IMC_KeyBoard = Settings->Keyboard_IMC.LoadSynchronous();
	}
}

void APXPlayerController::OnGameStart()
{
	GameAlreadyStart = true;
	OnCharacterControl(true);
	if (ABasePXCharacter* PxCharacter = Cast<ABasePXCharacter>(GetPawn()))
	{
		PxCharacter->ReadyToStart();
	}
}

void APXPlayerController::OnCharacterControl(bool On)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Subsystem);

	
	CharacterControlling = On;
	if (On)
	{
		Subsystem->AddMappingContext(IMC_GamePad, 1);
		Subsystem->AddMappingContext(IMC_KeyBoard, 1);
	}
	else
	{
		Subsystem->RemoveMappingContext(IMC_GamePad);
		Subsystem->RemoveMappingContext(IMC_KeyBoard);
	}
}

UInputMappingContext* APXPlayerController::GetCurrentIMC()
{
	return GamePadControlling ? IMC_GamePad : IMC_KeyBoard;
}
