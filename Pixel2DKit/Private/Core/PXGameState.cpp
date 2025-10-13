// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PXGameState.h"

#include "EngineUtils.h"
#include "Controller/PXPlayerController.h"
#include "Core/PXGameInstance.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Enemy/EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Platform/PlatformFight.h"
#include "Subsystems/AchievementSubsystem.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "Subsystems/WeatherSubsystem.h"
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/PXGameplayStatics.h"

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
	if (!ForceWeatherIndex.IsNone())
	{
		CurWeatherIndex = ForceWeatherIndex;
		SetWeather(CurWeatherIndex);
	}
	else
	{
		RandomWeather();
	}

	if (UWeatherSubsystem* WeatherSubsystem = GetGameInstance()->GetSubsystem<UWeatherSubsystem>())
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
		if (FLevelWeatherRate* WeatherRateData = Data->FindRow<FLevelWeatherRate>(LevelName, TEXT("GetLevelWeatherRateData")))
		{
			UCommonFuncLib::CalRandomMap(WeatherRateData->WeatherRate, CurWeatherIndex);
		}
	}
	
	SetWeather(CurWeatherIndex);
}

void APXGameState::DealUI_Implementation()
{
}

void APXGameState::DealStatics_Implementation()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);

	UGameInstance* GameInstance = GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);
	
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

	if (UAchievementSubsystem* AchievementSubsystem = GameInstance->GetSubsystem<UAchievementSubsystem>())
	{
		AchievementSubsystem->Achievement_LevelTransition();
	}
	
}

void APXGameState::PassDayTime(float Time, bool DirectSet, float TransitionDuration, FName _ForceWeatherIndex)
{
	if (!_ForceWeatherIndex.IsNone())
	{
		ForceWeatherIndex = _ForceWeatherIndex;
		UTimerSubsystemFuncLib::SetDelay(GetWorld(), [WeakThis = TWeakObjectPtr(this)]
		{
			if (!WeakThis.IsValid()) return;
			WeakThis->ForceWeatherIndex = FName();
		}, TransitionDuration + 0.5);
	}
	
	TimeOfDay = DirectSet ? Time : TimeOfDay + Time;
	BP_OnPassDayTime(TimeOfDay, TransitionDuration);
}


void APXGameState::ToNextLevel()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);

	
	DealStatics();
	DealUI();
	UPXSaveGameSubSystemFuncLib::SaveMainData(World);
	UPXSaveGameSubSystemFuncLib::SaveBasicBuildData(World);
	UPXSaveGameSubSystemFuncLib::SaveTalentsData(World);
}

void APXGameState::OnGameStart()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	UGameInstance* GameInstance = GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)

	for (AEnemySpawner* Enemy : TActorRange<AEnemySpawner>(World)) {
		if (IsValid(Enemy)) {
			Enemy->SpawnEnemy();
		}
	}

	for (APlatformFight* PlatformFlight : TActorRange<APlatformFight>(World)) {
		if (IsValid(PlatformFlight)) {
			PlatformFlight->RegisterEnemies();
		}
	}

	if (UWeatherSubsystem* WeatherSubsystem = GameInstance->GetSubsystem<UWeatherSubsystem>())
	{
		WeatherSubsystem->MakeWeatherEffect();
	}

	if (UPXSaveGameSubsystem* PXSaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>())
	{
		PXSaveGameSubsystem->Main_SyncLevel();
	}

	if (APXPlayerController* PC = UPXGameplayStatics::GetPlayerController(World))
	{
		PC->OnGameStart();
	}
	
	BP_OnGameStart();
}

void APXGameState::StartRaceTimer_Implementation(bool Start)
{
	
}

UPrimaryDataAsset* APXGameState::SetWeather_Implementation(FName WeatherRowName)
{
	UPXGameInstance* GameInstance = GetGameInstance<UPXGameInstance>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GameInstance, nullptr)

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(DataTableSubsystem, nullptr)

	UDataTable* DT= DataTableSubsystem->GetWeatherData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(DT, nullptr)

	FWeather* WeatherData = DT->FindRow<FWeather>(WeatherRowName, TEXT("APXGameState::SetWeather_Implementation"));
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(WeatherData, nullptr);
	
	WeatherName = WeatherData->WeatherName;
	WeatherType = WeatherData->WeatherType;
	WeatherEffect = WeatherData->WeatherEffect;

	return WeatherData->WeatherSetting;
}

void APXGameState::OnEnemyDie_Implementation(ABaseEnemy* Enemy)
{
}
