// Fill out your copyright notice in the Description page of Project Settings.


#include "Basic/PXGameState.h"

#include "Basic/PXGameInstance.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/DataTableSettings.h"
#include "Utilitys/CommonFuncLib.h"

void APXGameState::LoadWeatherEffect()
{
}

void APXGameState::SetWeatherEffect(const FWeatherEffectData& data)
{
	WeatherEffectData = data;
}

void APXGameState::UpdateWeather()
{
	if (ForceWeatherIndex == FName())
	{
		if (const UDataTableSettings* DataTableSettings = GetDefault<UDataTableSettings>())
		{
			if (UDataTable* Data= DataTableSettings->GetLevelWeatherRateData())
			{
				if (UPXGameInstance* GameInstance= GetGameInstance<UPXGameInstance>())
				{
					FName LevelName = GameInstance->GetCurLevelName(true);
					FLevelWeatherRate* WeatherRateData = Data->FindRow<FLevelWeatherRate>(LevelName, TEXT("GetLevelWeatherRateData"));

					// 概率计算
					UCommonFuncLib::CalRandomMap(WeatherRateData->WeatherRate, CurWeatherIndex);
				}
			}
		}
	}
	else
	{
		CurWeatherIndex = ForceWeatherIndex;
	}
	SetWeather(CurWeatherIndex);
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
	if (const UDataTableSettings* DataTableSettings = GetDefault<UDataTableSettings>())
	{
		if (UDataTable* DT= DataTableSettings->GetWeatherData())
		{
			FWeather* WeatherData = DT->FindRow<FWeather>(WeatherRowName, TEXT("GetWeather"));
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(WeatherData, nullptr);
			if (UPXSettingSaveGame* SettingSaveGame = UPXSaveGameSubSystemFuncLib::GetSettingData(GetWorld()))
			{
				SettingSaveGame->GameSetting_WeatherType = UGameplayStatics::GetObjectClass(WeatherData->WeatherSetting);
				// UPXSaveGameSubSystemFuncLib::SaveSettingData(GetWorld());
				WeatherType = WeatherData->WeatherType;
				return WeatherData->WeatherSetting;
			}
		}
	}
	return nullptr;
}

void APXGameState::OnEnemyDie_Implementation(ABaseEnemy* Enemy)
{
}
