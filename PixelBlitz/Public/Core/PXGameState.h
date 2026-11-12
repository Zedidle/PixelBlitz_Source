// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PXCustomStruct.h"
#include "Utilitys/CommonFuncLib.h"
#include "Character/BasePXCharacter.h"
#include "GameFramework/GameState.h"
#include "PXGameState.generated.h"

USTRUCT(BlueprintType)
struct FWeatherEffectData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SightPercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HPEffectPerSecond = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeedEffectPercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EPConsumePercent = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamagePlusPercent = 0.0f;
};

UENUM(BlueprintType)
enum class EDayTimeType : uint8
{
	None UMETA(DisplayName = "None"),
	Light UMETA(DisplayName = "白天"),
	Night UMETA(DisplayName = "黑夜"),
	Dawn  UMETA(DisplayName = "黎明"),
	Dusk  UMETA(DisplayName = "黄昏")
};


USTRUCT(BlueprintType)
struct FLevelWeatherRate: public FTableRowBase
{
	GENERATED_BODY()

	// 天气权重（细分天气，与 Weather 表各行 WeatherDetail 对应）；经 RandomWeather 抽取后按 WeatherDetail 匹配 Weather 表
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	TMap<EWeatherDetail, int> WeatherRate;

	// 时段权重（十二时辰）；用 UCommonFuncLib::CalRandTimeByChineseHourRate / ChineseHourToTime 换算成 PassDayTime 的 Time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	TMap<EChineseHour, int> TimeOfDayRate;
};

USTRUCT(BlueprintType)
struct FWeather: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	FText WeatherName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	EWeather WeatherType = EWeather::Sunny;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	EWeatherDetail WeatherDetail = EWeatherDetail::ClearSky;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	UPrimaryDataAsset* WeatherSetting = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	FWeatherEffectData WeatherEffect;
};



class UWeatherSubsystem;



UCLASS()
class PIXELBLITZ_API APXGameState : public AGameState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameState | Weather", meta = (AllowPrivateAccess))
	EWeatherDetail ForceWeather = EWeatherDetail::None;

	
	float CurRaceTime = 0.0f; 

	// 时间转换期间是否禁止天气转变 
	bool bAbortWeatherChange = false;
	
protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameState | Weather")
	void EventOnDayTimeTypeChanged();

	UPROPERTY(BlueprintReadWrite, Category = "GameState | Weather")
	EDayTimeType DayTimeType;
	
	UPROPERTY(BlueprintReadWrite, Category = "GameState | Weather")
	float TimeOfDay;

	// 当前关卡昼夜判定阈值（关卡加载后由 BP 从 LevelLighting 表行 FUDSLevelArgs 写入）
	UPROPERTY(BlueprintReadWrite, Category = "GameState | Weather")
	float DawnTime = 800.0f;

	UPROPERTY(BlueprintReadWrite, Category = "GameState | Weather")
	float DuskTime = 1800.0f;

	UPROPERTY(BlueprintReadWrite, Category = "GameState | Weather")
	float DawnDuskExtend = 300.0f;

	UPROPERTY(BlueprintReadOnly, Category = "GameState | Weather")
	EWeatherDetail CurWeatherDetail = EWeatherDetail::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	FText WeatherName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameState | Weather")
	EWeather WeatherType;
	
	UPROPERTY(BlueprintReadOnly, Category = "GameState | Weather")
	FWeatherEffectData WeatherEffect;

	// 时刻（HHMM 刻度，800 = 08:00）→ 昼夜时段，阈值取 DawnTime/DuskTime/DawnDuskExtend（来自 LevelLighting 表行）
	// 判定：黎明 [Dawn-Ext, Dawn) / 白天 [Dawn, Dusk) / 黄昏 [Dusk, Dusk+Ext) / 黑夜 其余（含跨零点）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameState | Weather")
	EDayTimeType CalDayTimeType(float NewTimeOfDay) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameState | Weather")
	float GetEPConsumePlusPercent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameState | Weather")
	float GetDamagePlusPercent();
	
	// 每一个关卡都有随机的天气
	UFUNCTION(BlueprintCallable, Category = "GameState | Weather")
	void RandomWeather();
	
	UFUNCTION(BlueprintCallable, Category = "GameState | Weather")
	void PassDayTime(float Time, bool DirectSet, bool AbortWeatherChange = false, float TransitionDuration = 3, EWeatherDetail _ForceWeather = EWeatherDetail::None);
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState | Weather", DisplayName="OnPassDayTime")
	void BP_OnPassDayTime(float NewTime, float TransitionDuration = 3);

	
	UFUNCTION(BlueprintCallable, Category = "GameState | Weather")
	void SetWeather(EWeatherDetail Weather);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, DisplayName = "SetWeather", Category = "GameState | Weather")
	void BP_SetWeather(UPrimaryDataAsset* WeatherDataAsset);

	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameState | Monster")
	void OnEnemyDie(ABaseEnemy* Enemy);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameState | Monster")
	bool IsAllMonsterDie();
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnAllMonsterDie", Category = "GameState | Monster")
	void BP_OnAllMonsterDie();
	
	UFUNCTION(BlueprintNativeEvent, Category = "GameState | Currency")
	void DealStatics();

	UFUNCTION(BlueprintNativeEvent, Category = "GameState | Currency")
	void DealUI();

	UFUNCTION(BlueprintCallable, Category = "GameState | GameplayFlow")
	void ToNextLevel();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameState | GameplayFlow")
	FVector GetNewLevelInitLocation();

	
	UFUNCTION(BlueprintCallable, Category = "GameState | GameplayFlow")
	void OnGameStart();
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState | GameplayFlow", DisplayName = "OnGameStart")
	void BP_OnGameStart();

	// 竞速计时器
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameState | GameplayFlow")
	void StartRaceTimer(bool bStart);

	UFUNCTION(BlueprintNativeEvent)
	void OnEnemyBossDie();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnTimerUpdate")
	void BP_OnTimerUpdate(const FText& TimeToShow);

};


