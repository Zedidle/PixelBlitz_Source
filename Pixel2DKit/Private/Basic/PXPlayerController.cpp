// Fill out your copyright notice in the Description page of Project Settings.


#include "Basic/PXPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Character/BasePXCharacter.h"
#include "Pixel2DKit/Pixel2DKit.h"

class UEnhancedInputLocalPlayerSubsystem;

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
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Subsystem);
	
	CharacterControlling = On;
	if (On)
	{
		Subsystem->AddMappingContext(IMC_GamePad.LoadSynchronous(), 1);
		Subsystem->AddMappingContext(IMC_KeyBoard.LoadSynchronous(), 1);
	}
	else
	{
		Subsystem->RemoveMappingContext(IMC_GamePad.LoadSynchronous());
		Subsystem->RemoveMappingContext(IMC_KeyBoard.LoadSynchronous());
	}
}

UInputMappingContext* APXPlayerController::GetCurrentIMC()
{
	if (GamePadControlling)
	{
		return IMC_GamePad.LoadSynchronous();
	}
	else
	{
		return IMC_KeyBoard.LoadSynchronous();
	}
}
