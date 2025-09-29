// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PXGameState.h"
#include "Core/PXGameInstance.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "Subsystems/WeatherSubsystem.h"
#include "Utilitys/CommonFuncLib.h"

void APXGameState::BeginPlay()
{
	Super::BeginPlay();
}

void APXGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

void APXGameState::EventOnDayTimeTypeChanged_Implementation()
{
	if (ForceWeatherIndex.IsValid())
	{
		CurWeatherIndex = ForceWeatherIndex;
		SetWeather(CurWeatherIndex);
	}
	else
	{
		RandomWeather();
	}

	UWeatherSubsystem* WeatherSubsystem = GetGameInstance()->GetSubsystem<UWeatherSubsystem>();
	if (WeatherSubsystem)
	{
		WeatherSubsystem->MakeWeatherEffect();
	}
}

float APXGameState::GetEPConsumePlusPercent()
{
	return WeatherEffect.EPConsumePercent;
}

float APXGameState::GetDamagePlusPercent()
{
	return WeatherEffect.DamagePlusPercent;
}

void APXGameState::RandomWeather()
{
	UPXGameInstance* GameInstance= GetGameInstance<UPXGameInstance>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem);
	
	if (UDataTable* Data= DataTableSubsystem->GetLevelWeatherRateData())
	{
		FName LevelName = GameInstance->GetCurLevelName(true);
		FLevelWeatherRate* WeatherRateData = Data->FindRow<FLevelWeatherRate>(LevelName, TEXT("GetLevelWeatherRateData"));

		// 概率计算
		UCommonFuncLib::CalRandomMap(WeatherRateData->WeatherRate, CurWeatherIndex);
	}
	
	SetWeather(CurWeatherIndex);
}

void APXGameState::PassDayTime_Implementation(float Time, bool DirectSet, float TransitionDuration,
	FName _ForceWeatherIndex)
{
	if (_ForceWeatherIndex.IsValid())
	{
		ForceWeatherIndex = _ForceWeatherIndex;
		UTimerSubsystemFuncLib::SetDelay(GetWorld(), [WeakThis = TWeakObjectPtr<ThisClass>(this)]
		{
			if (WeakThis.IsValid())
			{
				WeakThis->ForceWeatherIndex = FName("");
			}
		}, TransitionDuration + 0.5);
	}
}

void APXGameState::DealGolds()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);
	UPXMainSaveGame* MainSaveGame = UPXSaveGameSubSystemFuncLib::GetMainData(World);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSaveGame);

	UPXBasicBuildSaveGame* BasicBuildSaveGame = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(World);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BasicBuildSaveGame);

	UPXTalentsSaveGame* TalentSaveGame = UPXSaveGameSubSystemFuncLib::GetTalentsData(World);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(TalentSaveGame);

	MainSaveGame->RoundGoldNum += MainSaveGame->JustPickedGolds;
	BasicBuildSaveGame->RemainGoldNum += MainSaveGame->JustPickedGolds;
	TalentSaveGame->TotalPickupGolds += MainSaveGame->JustPickedGolds;
	MainSaveGame->JustPickedGolds = 0;
	
}

void APXGameState::ToNextLevel_Implementation()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);

	
	DealGolds();
	UPXSaveGameSubSystemFuncLib::SaveMainData(World);
	UPXSaveGameSubSystemFuncLib::SaveBasicBuildData(World);
	UPXSaveGameSubSystemFuncLib::SaveTalentsData(World);
}

UPrimaryDataAsset* APXGameState::SetWeather_Implementation(FName WeatherRowName)
{
	UPXGameInstance* GameInstance = GetGameInstance<UPXGameInstance>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GameInstance, nullptr)

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(DataTableSubsystem, nullptr)


	if (UDataTable* DT= DataTableSubsystem->GetWeatherData())
	{
		FWeather* WeatherData = DT->FindRow<FWeather>(WeatherRowName, TEXT("GetWeather"));
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(WeatherData, nullptr);
		if (UPXSettingSaveGame* SettingSaveGame = UPXSaveGameSubSystemFuncLib::GetSettingData(GetWorld()))
		{
			SettingSaveGame->GameSetting_WeatherType = UGameplayStatics::GetObjectClass(WeatherData->WeatherSetting);

			WeatherName = WeatherData->WeatherName;
			WeatherType = WeatherData->WeatherType;
			WeatherEffect = WeatherData->WeatherEffect;

			return WeatherData->WeatherSetting;
		}
	}
	
	return nullptr;
}

void APXGameState::OnEnemyDie_Implementation(ABaseEnemy* Enemy)
{
}
