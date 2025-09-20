// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/PXGameplayStatics.h"
#include "Core/PXGameMode.h"
#include "Core/PXGameState.h"
#include "Controller/PXPlayerController.h"
#include "Kismet/GameplayStatics.h"

UPXGameInstance* UPXGameplayStatics::GetGameInstance(const UObject* WorldContextObject)
{
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		return Cast<UPXGameInstance>(GameInstance);
	}

	return nullptr;
}

APXGameMode* UPXGameplayStatics::GetGameMode(const UObject* WorldContextObject)
{
	if (AGameModeBase* GameMode = UGameplayStatics::GetGameMode(WorldContextObject))
	{
		return Cast<APXGameMode>(GameMode);
	}

	return nullptr;
}

APXGameState* UPXGameplayStatics::GetGameState(const UObject* WorldContextObject)
{
	if (AGameStateBase* GameState = UGameplayStatics::GetGameState(WorldContextObject))
	{
		return Cast<APXGameState>(GameState);
	}

	return nullptr;
}

APXPlayerController* UPXGameplayStatics::GetPlayerController(const UObject* WorldContextObject, int32 PlayerIndex)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex))
	{
		if (APXPlayerController* PxPlayerController = Cast<APXPlayerController>(PC))
		{
			return PxPlayerController;
		}
	}

	return nullptr;
}

ABasePXCharacter* UPXGameplayStatics::GetPlayerCharacter(const UObject* WorldContextObject, int32 PlayerIndex)
{
	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(WorldContextObject, PlayerIndex))
	{
		if (ABasePXCharacter* PxCharacter = Cast<ABasePXCharacter>(Character))
		{
			return PxCharacter;
		}
	}

	return nullptr;
}
