// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PXGameState.h"

#include "EngineUtils.h"
#include "Controller/PXPlayerController.h"
#include "Core/PXGameInstance.h"
#include "Core/PXGameMode.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Enemy/EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "PixelBlitz/PixelBlitz.h"
#include "Platform/PlatformFight.h"
#include "Subsystems/AchievementSubsystem.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/PXAudioSubsystem.h"
#include "Subsystems/TimerManagerFuncLib.h"
#include "Subsystems/WeatherSubsystem.h"
#include "UI/UIManager.h"
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/PXGameplayStatics.h"

void APXGameState::BeginPlay()
{
	Super::BeginPlay();
}

void APXGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	UTimerManagerFuncLib::CancelDelay(GetWorld(), "APXGameState.RaceTimer");
}

void APXGameState::EventOnDayTimeTypeChanged_Implementation()
{
	if (!bAbortWeatherChange)
	{
		if (ForceWeather != EWeatherDetail::None)
		{
			CurWeatherDetail = ForceWeather;
			SetWeather(CurWeatherDetail);
		}
		else
		{
			RandomWeather();
		}
	}

	if (UWeatherSubsystem* WeatherSubsystem = UWeatherSubsystem::GetInstance(this))
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

EDayTimeType APXGameState::CalDayTimeType(float NewTimeOfDay) const
{
	// 归一到 [0, 2400)
	auto Norm = [](float V)
	{
		return FMath::Fmod(FMath::Fmod(V, 2400.f) + 2400.f, 2400.f);
	};
	// 环形窗口判定 [Min, Max)，允许跨零点（如黑夜窗口）
	auto InWindow = [](float V, float Min, float Max)
	{
		return Min <= Max ? (V >= Min && V < Max) : (V >= Min || V < Max);
	};

	const float T = Norm(NewTimeOfDay);
	const float DawnStart = Norm(DawnTime - DawnDuskExtend);
	const float DawnEnd = Norm(DawnTime);
	const float DuskStart = Norm(DuskTime);
	const float DuskEnd = Norm(DuskTime + DawnDuskExtend);

	if (InWindow(T, DawnStart, DawnEnd)) return EDayTimeType::Dawn;
	if (InWindow(T, DuskStart, DuskEnd)) return EDayTimeType::Dusk;
	if (InWindow(T, DawnEnd, DuskStart)) return EDayTimeType::Light;
	return EDayTimeType::Night;
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
			UCommonFuncLib::CalRandomMap(WeatherRateData->WeatherRate, CurWeatherDetail);
		}
	}

	SetWeather(CurWeatherDetail);
}

void APXGameState::DealUI_Implementation()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);

	UPXGameInstance* GameInstance = GetGameInstance<UPXGameInstance>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);
	
	UPXMainSaveGame* MainSave = UPXSaveGameSubSystemFuncLib::GetMainData(World);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSave);

	UUIManager* UIManager = UUIManager::GetSelfInstance(this);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(UIManager);
	
	
	if (MainSave->RemLevels.IsEmpty())
	{
		UIManager->OpenUI("LevelFinish");
	}
	else
	{
		UIManager->OpenUI("LevelTransition");
	}
	
}

void APXGameState::DealStatics_Implementation()
{
	UPXMainSaveGame* MainSave = UPXSaveGameSubSystemFuncLib::GetMainData(this);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSave);

	UPXBasicBuildSaveGame* BasicBuildSave = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(this);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BasicBuildSave);
	
	
	MainSave->RoundGoldNum += MainSave->JustPickedGolds;
	BasicBuildSave->RemainGoldNum += MainSave->JustPickedGolds;
	BasicBuildSave->TotalPickupGolds += MainSave->JustPickedGolds;
	MainSave->JustPickedGolds = 0;

	UPXSaveGameSubSystemFuncLib::SaveMainData(this);
	UPXSaveGameSubSystemFuncLib::SaveBasicBuildData(this);
	
	if (UAchievementSubsystem* AchievementSubsystem = UAchievementSubsystem::GetInstance(this))
	{
		AchievementSubsystem->Achievement_LevelTransition();
	}
	
	if (!IsAllMonsterDie())
	{
		MainSave->bTotalMonsterClear = false;
	}
	else
	{
		MainSave->RemRefreshPoints++;
	}
}

void APXGameState::PassDayTime(float Time, bool DirectSet, bool AbortWeatherChange, float TransitionDuration, EWeatherDetail _ForceWeather)
{
	bAbortWeatherChange = AbortWeatherChange;
	
	if (_ForceWeather != EWeatherDetail::None)
	{
		ForceWeather = _ForceWeather;
		UTimerManagerFuncLib::SetDelaySafe(GetWorld(), this, [](APXGameState* GS){ GS->ForceWeather = EWeatherDetail::None; }, TransitionDuration + 0.5);
	}
	
	TimeOfDay = DirectSet ? Time : TimeOfDay + Time;
	BP_OnPassDayTime(TimeOfDay, TransitionDuration);
}

void APXGameState::SetWeather(EWeatherDetail Weather)
{
	UPXGameInstance* GameInstance = GetGameInstance<UPXGameInstance>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem)

	UDataTable* DT= DataTableSubsystem->GetWeatherData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DT)

	// 按行的 WeatherDetail 匹配，行名不再参与索引
	TArray<FWeather*> Rows;
	DT->GetAllRows<FWeather>(TEXT("APXGameState::SetWeather_Implementation"), Rows);
	FWeather* WeatherData = nullptr;
	int32 MatchCount = 0;
	for (FWeather* Row : Rows)
	{
		if (Row && Row->WeatherDetail == Weather)
		{
			WeatherData = Row;
			++MatchCount;
		}
	}
	if (!WeatherData)
	{
		// 找不到匹配行：多为 Weather 表的 WeatherDetail 列未填/与枚举值错位（如中途插入 None 导致整体 +1）
		UE_LOG(LogTemp, Warning,
			TEXT("[SetWeather] Weather 表（%d 行）中不存在 WeatherDetail=%s 的行，本次天气切换被跳过；请检查表列与枚举是否一致"),
			Rows.Num(), *UEnum::GetValueAsString(Weather));
		return;
	}
	if (MatchCount > 1)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[SetWeather] Weather 表中有 %d 行的 WeatherDetail=%s，将使用第一行；请去重"),
			MatchCount, *UEnum::GetValueAsString(Weather));
	}

	WeatherName = WeatherData->WeatherName;
	WeatherType = WeatherData->WeatherType;
	WeatherEffect = WeatherData->WeatherEffect;

	BP_SetWeather(WeatherData->WeatherSetting);
}

bool APXGameState::IsAllMonsterDie()
{
	UWorld* World = GetWorld();
	if (!World) return false;
	
	for (ABaseEnemy* Enemy : TActorRange<ABaseEnemy>(World))
	{
		if (Enemy && Enemy->IsValidLowLevel())
		{
			if (!Enemy->bDead)
			{
				return false;
			}
		}
	}
	return true;
}


void APXGameState::ToNextLevel()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);
	
	UPXGameInstance* GameInstance = GetGameInstance<UPXGameInstance>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);
	
	const FName NextLevelName = GameInstance->GetCurLevelName_Simple(true);

	// 时间记录
	UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem);

	UPXMainSaveGame* MainSaveGame = SaveGameSubsystem->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSaveGame);

	float PreviousUsedTime = 0.0f;
	for (float Result : MainSaveGame->Results)
	{
		PreviousUsedTime += Result;
	}
	SaveGameSubsystem->Main_AddResult(CurRaceTime - PreviousUsedTime);
	
	// 由于UI需要Statics来显示，DealUI要先
	DealUI();
	DealStatics();

	if (APXGameMode* GM = UPXGameplayStatics::GetGameMode(World))
	{
		if (!NextLevelName.IsNone())
		{
			GM->LoadLevel(NextLevelName, GetNewLevelInitLocation());
		}
	}
}

FVector APXGameState::GetNewLevelInitLocation()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(World, FVector::ZeroVector);
	
	if (ABasePXCharacter* Character = UPXGameplayStatics::GetPlayerCharacter(World))
	{
		return Character->GetActorLocation() - Character->GetVectorFaceToCamera().GetSafeNormal2D() * 240 - FVector(0,0,120);
	}
	
	return FVector(0, 0, 0);
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

	if (UWeatherSubsystem* WeatherSubsystem = UWeatherSubsystem::GetInstance(this))
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

	StartRaceTimer(true);
}



void APXGameState::OnEnemyBossDie_Implementation()
{
	StartRaceTimer(false);
	
	UTimerManagerFuncLib::SetDelaySafe(GetWorld(), this, &APXGameState::ToNextLevel, 1.2);

	if (UPXAudioSubsystem* AudioSubsystem = UPXAudioSubsystem::GetInstance(this))
	{
		AudioSubsystem->CloseBGM();
	}
}

void APXGameState::StartRaceTimer_Implementation(bool bStart)
{
    UTimerManager* TimerSubsystem = UTimerManager::GetInstance(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(TimerSubsystem);

	FName TimerName = "APXGameState.RaceTimer";
	
	if (bStart)
	{
		if (TimerSubsystem->HasTimer(TimerName))
		{
			TimerSubsystem->UnPauseDelay(TimerName);
		}
		else
		{
			TimerSubsystem->SetDelayLoop(TimerName, [WeakThis = TWeakObjectPtr(this)]
			{
				if (!WeakThis.IsValid()) return;
				WeakThis->CurRaceTime += 0.01;

				// 1. 创建数字格式选项并设置精度
				FNumberFormattingOptions FormatOptions;
				FormatOptions.MinimumFractionalDigits = 2; // 保证至少两位小数
				FormatOptions.MaximumFractionalDigits = 2; // 保证最多两位小数（即精确到两位）

				// 2. 转换为FText
				FText FormattedText = FText::AsNumber(WeakThis->CurRaceTime, &FormatOptions);
				WeakThis->BP_OnTimerUpdate(FormattedText);
				
			}, 0.01);
		}
	}
	else
	{
		TimerSubsystem->PauseDelay(TimerName);
	}
}



void APXGameState::OnEnemyDie_Implementation(ABaseEnemy* Enemy)
{
	if (IFight_Interface::Execute_GetOwnCamp(Enemy).HasTag(TAG("Enemy.BOSS")))
	{
		OnEnemyBossDie();
	}

	if (IsAllMonsterDie())
	{
		
	}
}
