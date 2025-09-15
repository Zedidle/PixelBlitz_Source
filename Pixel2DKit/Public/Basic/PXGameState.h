// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BasePXCharacter.h"
#include "GameFramework/GameState.h"
#include "PXGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDayTimeTypeChanged);


USTRUCT(BlueprintType)
struct FLevelWeatherRate: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	TMap<FName, int> WeatherRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	TMap<float, int> TimeOfDayRate;
};

USTRUCT(BlueprintType)
struct FWeather: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	FLocalizedTableData WeatherName_Localized;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	TEnumAsByte<EWeather> WeatherType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	UPrimaryDataAsset* WeatherSetting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	FWeatherEffectData WeatherEffect;
};


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

	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "GameState | Weather")
	FOnDayTimeTypeChanged OnDayTimeTypeChanged;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameState | Weather")
	void EventOnDayTimeTypeChanged();
	
	
	UPROPERTY(BlueprintReadOnly, Category = "GameState | Weather")
	FName CurWeatherIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	FLocalizedTableData WeatherName_Localized;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameState | Weather")
	TEnumAsByte<EWeather> WeatherType;
	
	UPROPERTY(BlueprintReadOnly, Category = "GameState | Weather")
	FWeatherEffectData WeatherEffect;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameState | Weather")
	float GetEPConsumePlusPercent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameState | Weather")
	float GetDamagePlusPercent();
	
	
	UFUNCTION(BlueprintCallable, Category = "GameState | Weather")
	void SetForceWeatherIndex(const FName& WeatherIndex);

	// 每一个关卡都有随机的天气
	UFUNCTION(BlueprintCallable, Category = "GameState | Weather")
	void UpdateWeather();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameState | Weather")
	UPrimaryDataAsset* SetWeather(FName WeatherRowName);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameState | Monster")
	void OnEnemyDie(ABaseEnemy* Enemy);


	UFUNCTION(BlueprintCallable, Category = "GameState | Currency")
	void DealGolds();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameState | GameplayFlow")
	void ToNextLevel();
	
};

inline void APXGameState::EventOnDayTimeTypeChanged_Implementation()
{
	UpdateWeather();
}
