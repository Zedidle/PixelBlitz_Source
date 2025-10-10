// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BasePXCharacter.h"
#include "GameFramework/GameState.h"
#include "PXGameState.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	TMap<FName, int> WeatherRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	TMap<FVector2D, int> TimeOfDayRate;
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
	UPrimaryDataAsset* WeatherSetting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	FWeatherEffectData WeatherEffect;
};



class UWeatherSubsystem;



UCLASS()
class PIXEL2DKIT_API APXGameState : public AGameState
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameState | Weather", meta = (AllowPrivateAccess))
	FName ForceWeatherIndex;

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
	
	UPROPERTY(BlueprintReadOnly, Category = "GameState | Weather")
	FName CurWeatherIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	FText WeatherName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameState | Weather")
	EWeather WeatherType;
	
	UPROPERTY(BlueprintReadOnly, Category = "GameState | Weather")
	FWeatherEffectData WeatherEffect;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameState | Weather")
	float GetEPConsumePlusPercent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameState | Weather")
	float GetDamagePlusPercent();
	
	// 每一个关卡都有随机的天气
	UFUNCTION(BlueprintCallable, Category = "GameState | Weather")
	void RandomWeather();
	
	UFUNCTION(BlueprintCallable, Category = "GameState | Weather")
	void PassDayTime(float Time, bool DirectSet, float TransitionDuration = 3, FName _ForceWeatherIndex = FName("None"));
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState | Weather", DisplayName="OnPassDayTime")
	void BP_OnPassDayTime(float NewTime, float TransitionDuration = 3);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameState | Weather")
	UPrimaryDataAsset* SetWeather(FName WeatherRowName);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameState | Monster")
	void OnEnemyDie(ABaseEnemy* Enemy);


	UFUNCTION(BlueprintNativeEvent, Category = "GameState | Currency")
	void DealStatics();

	UFUNCTION(BlueprintNativeEvent, Category = "GameState | Currency")
	void DealUI();

	UFUNCTION(BlueprintCallable, Category = "GameState | GameplayFlow")
	void ToNextLevel();

	UFUNCTION(BlueprintCallable, Category = "GameState | GameplayFlow")
	void OnGameStart();
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState | GameplayFlow", DisplayName = "OnGameStart")
	void BP_OnGameStart();
};


