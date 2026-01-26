// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/PXGameplayStatics.h"

#include "CommonInputSubsystem.h"
#include "CommonUIExtensions.h"
#include "Pixel2DKit.h"
#include "Core/PXGameMode.h"
#include "Core/PXGameState.h"
#include "Controller/PXPlayerController.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PXLocalPlayer.h"
#include "Settings/PXSettingsShared.h"
#include "Utilitys/DebugFuncLab.h"

class APXPlayerController;

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

UPXLocalPlayer* UPXGameplayStatics::GetLocalPlayer(const UObject* WorldContextObject, int32 PlayerIndex)
{
	APXPlayerController* PC = GetPlayerController(WorldContextObject, 0);
	if (PC)
	{
		return Cast<UPXLocalPlayer>(PC->GetLocalPlayer());
	}

	return nullptr;
}

UPXSettingsShared* UPXGameplayStatics::GetSettingsShared(const UObject* WorldContextObject)
{
	UPXLocalPlayer* LP = GetLocalPlayer(WorldContextObject, 0);
	if (LP)
	{
		return LP->GetSharedSettings();
	}

	return nullptr;
}

bool UPXGameplayStatics::IsChosenTalent(const UObject* WorldContextObject, FGameplayTag Tag)
{
	if (UPXBasicBuildSaveGame* SG = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(WorldContextObject))
	{
		return SG->ChosenTalents.Contains(Tag);		
	}
	return false;
}

void UPXGameplayStatics::ChoiceTalent(const UObject* WorldContextObject, FGameplayTag Tag)
{
	if (UPXBasicBuildSaveGame* SG = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(WorldContextObject))
	{
		SG->ChosenTalents.AddUnique(Tag);		
	}
}

void UPXGameplayStatics::UnChoiceTalent(const UObject* WorldContextObject, FGameplayTag Tag)
{
	if (UPXBasicBuildSaveGame* SG = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(WorldContextObject))
	{
		SG->ChosenTalents.Remove(Tag);		
	}
}

void UPXGameplayStatics::SpawnForceFeedbackAttached(const UObject* WorldContextObject, UForceFeedbackEffect* ForceFeedbackEffect, USceneComponent* AttachToComponent)
{
	if (!IsGamepadControlling(WorldContextObject)) return;

	bool bForceFeedBack =  GetDefault<UPXSettingsShared>()->GetForceFeedbackEnabled();

	UDebugFuncLab::ScreenMessage(FString::Printf(TEXT("bForceFeedBack: %s"), bForceFeedBack ? TEXT("ForceFeedBack Open") : TEXT("ForceFeedBack Close")));
	if (bForceFeedBack)
	{
		UGameplayStatics::SpawnForceFeedbackAttached(ForceFeedbackEffect, AttachToComponent);
	}
}

bool UPXGameplayStatics::IsGamepadControlling(const UObject* WorldContextObject)
{
	APXPlayerController* PXPlayerController = GetPlayerController(WorldContextObject);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(PXPlayerController, false)
	if(!PXPlayerController->IsLocalController()) return false;

	const UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(PXPlayerController->GetLocalPlayer());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(PXPlayerController, false)

	return InputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad;
}

