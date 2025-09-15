// Fill out your copyright notice in the Description page of Project Settings.


#include "Basic/PXGameState.h"

#include "Basic/PXGameInstance.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/DataTableSettings.h"
#include "Utilitys/CommonFuncLib.h"

void APXGameState::BeginPlay()
{
	Super::BeginPlay();

	OnDayTimeTypeChanged.AddDynamic(this, &ThisClass::EventOnDayTimeTypeChanged);
}

void APXGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnDayTimeTypeChanged.RemoveAll(this);
}

float APXGameState::GetEPConsumePlusPercent()
{
	return WeatherEffect.EPConsumePercent;
}

float APXGameState::GetDamagePlusPercent()
{
	return WeatherEffect.DamageAddPercent;
}

void APXGameState::SetForceWeatherIndex(const FName& WeatherIndex)
{
	CurWeatherIndex = WeatherIndex;
}

void APXGameState::UpdateWeather()
{
	if (ForceWeatherIndex.IsNone())
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

				WeatherName_Localized = WeatherData->WeatherName_Localized;
				WeatherType = WeatherData->WeatherType;
				WeatherEffect = WeatherData->WeatherEffect;

				return WeatherData->WeatherSetting;
			}
		}
	}
	return nullptr;
}

void APXGameState::OnEnemyDie_Implementation(ABaseEnemy* Enemy)
{
}
