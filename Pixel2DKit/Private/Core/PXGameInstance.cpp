// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PXGameInstance.h"

#include "Core/PXSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXResourceDataAsset.h"
#include "Sound/SoundCue.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Utilitys/PXGameplayStatics.h"
#include "Utilitys/SoundFuncLib.h"

void UPXGameInstance::SetLevelType(ELevelType _LevelType)
{
	LevelType = _LevelType;
}

void UPXGameInstance::StartNewGame()
{
	UPXSaveGameSubsystem* PXSG = GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXSG)

	UDataTableSubsystem* DataTableSubsystem = GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem)

	FString LevelTableName = "";
	if (LevelType == ELevelType::Race)
	{
		LevelTableName = "LevelData_Race";
	}
	else if(LevelType == ELevelType::Arena)
	{
		LevelTableName = "LevelData_Arena";
	}
	
	UDataTable* LevelData = DataTableSubsystem->GetDataTable(LevelTableName);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(LevelData)
	
	PXSG->Main_TotalInit();

	PXSG->GetMainData()->RemLevels.Empty();
	TArray<FLevelData> LevelDataArray = DataTableSubsystem->GetRowMap<FLevelData>(LevelData);
	for (auto& D : LevelDataArray)
	{
		PXSG->GetMainData()->RemLevels.Add(D.LevelInstanceName);
	}
	
	PXSG->SaveMainData();
	PXSG->SaveBasicBuildData();

	StartGame();
}

void UPXGameInstance::StartGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), "L_Main");
}

FName UPXGameInstance::GetCurLevelName(bool Continue)
{
	UPXSaveGameSubsystem* SaveGameSubsystem = GetSubsystem<UPXSaveGameSubsystem>();
	if (!SaveGameSubsystem) return FName();

	UPXMainSaveGame* MainSaveGame = SaveGameSubsystem->GetMainData();
	if (!MainSaveGame) return FName();

	
	if (Continue)
	{
		return MainSaveGame->CurLevelName;
	}

	if (MainSaveGame->RemLevels.Num() == 0) return "";
	
	if (MainSaveGame->RemLevels.Num() == 1)
	{
		MainSaveGame->CurLevelName = MainSaveGame->RemLevels[0];
	}
	else
	{
		const FString s0 = UKismetStringLibrary::GetSubstring(MainSaveGame->RemLevels[0].ToString(), 1, 1);
		float v0 = FCString::Atod(*s0);

		const FString s1 = UKismetStringLibrary::GetSubstring(MainSaveGame->RemLevels[1].ToString(), 1, 1);
		float v1 = FCString::Atod(*s1);

		float r = v0 / (v0 + v1);
		if (FMath::FRand() > r)
		{
			MainSaveGame->CurLevelName = MainSaveGame->RemLevels[0];
		}
		else
		{
			MainSaveGame->CurLevelName = MainSaveGame->RemLevels[1];
		}
	}
	
	MainSaveGame->CurLevel++;
	MainSaveGame->RemLevels.Remove(MainSaveGame->CurLevelName);
	return MainSaveGame->CurLevelName;
	
}

FName UPXGameInstance::GetCurLevelName_Simple(bool Next)
{
	UPXSaveGameSubsystem* SaveGameSubsystem = GetSubsystem<UPXSaveGameSubsystem>();
	if (!SaveGameSubsystem) return FName();

	UPXMainSaveGame* MainSaveGame = SaveGameSubsystem->GetMainData();
	if (!MainSaveGame) return FName();
	
	if (MainSaveGame->RemLevels.Num() == 0) return FName();

	if (Next)
	{
		MainSaveGame->CurLevel++;
		MainSaveGame->RemLevels.RemoveAt(0);
	}

	if (!MainSaveGame->RemLevels.IsValidIndex(0)) return FName();
	
	MainSaveGame->CurLevelName = MainSaveGame->RemLevels[0];
	return MainSaveGame->CurLevelName;
}

FName UPXGameInstance::GetCurLevelNameByNum(int Num)
{
	UPXSaveGameSubsystem* SaveGameSubsystem = GetSubsystem<UPXSaveGameSubsystem>();
	if (!SaveGameSubsystem) return FName();

	UPXMainSaveGame* MainSaveGame = SaveGameSubsystem->GetMainData();
	if (!MainSaveGame) return FName();

	FName NewLevelName = FName();
	
	for (auto& LevelName : MainSaveGame->RemLevels)
	{
		if (LevelName.ToString().Contains( FString::FromInt(Num) ))
		{
			MainSaveGame->CurLevel++;
			NewLevelName = LevelName;
			break;
		}
	}
	
	MainSaveGame->RemLevels.Remove(NewLevelName);
	return NewLevelName;
}

void UPXGameInstance::GetTotalUseTime(float& usetime, bool& newrecord)
{
	UPXSaveGameSubsystem* SaveGameSubsystem = GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem)
	UPXMainSaveGame* MainSaveGame = SaveGameSubsystem->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSaveGame)
	UPXAchievementsSaveGame* AchievementsSaveGame = SaveGameSubsystem->GetAchievementsData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AchievementsSaveGame);
	
	float Time = 0;
	for (float r : MainSaveGame->Results)
	{
		Time += r;
	}
	
	newrecord = AchievementsSaveGame->TotalUseTime > Time;
	if (newrecord)
	{
		AchievementsSaveGame->TotalUseTime = Time;
	}
	usetime = Time;
}

void UPXGameInstance::OnPlayerDead_Implementation(bool& End)
{
	UPXSaveGameSubsystem* SaveGameSubsystem = GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem);
	UPXMainSaveGame* MainSaveGame = SaveGameSubsystem->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSaveGame);
	
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

	const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceDataAsset)
	

	if (USoundCue* SC_OnDie = ResourceDataAsset->SC_OnDie.LoadSynchronous())
	{
		USoundFuncLib::PlaySound2D(SC_OnDie);
	}
	
	
	MainSaveGame->DieTimes++;
	MainSaveGame->RemLife--;
	
	End = MainSaveGame->RemLife == -1;
}
