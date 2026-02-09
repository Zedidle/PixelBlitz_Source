// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pixel2DKit.h"
#include "Core/PXGameState.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Utilitys/PXCustomStruct.h"
#include "WeatherSubsystem.generated.h"


UCLASS(Blueprintable)
class PIXEL2DKIT_API UDayTimeEffectSight : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// 后续扩展 TArray<FAttributeEffect> ?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttributeEffect DawnEffect = {EPXAttribute::CurSpeed, 0.05f, 10.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttributeEffect LightEffect = {EPXAttribute::CurSpeed, 0.05, 20.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttributeEffect DuskEffect = {EPXAttribute::CurSpeed, -0.05, -10.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttributeEffect NightEffect = {EPXAttribute::CurSpeed, -0.05, -20.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EDayTimeType, FText> DayTime2Name;	
};


UCLASS()
class PIXEL2DKIT_API UWeatherSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UDayTimeEffectSight* DayTimeEffectSight;
	
	
public:

	static UWeatherSubsystem* GetInstance(const UObject* WorldContextObject);
	
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	FString DayTimeNameString;
	FString WeatherNameString;
	FGameplayTag DayTimeTag = TAG("Buff.DayTime");
	FGameplayTag WeatherTag = TAG("Buff.Weather");
	
	UFUNCTION(BlueprintCallable, Category = "WeatherSystem")
	void MakeWeatherEffect();
};
