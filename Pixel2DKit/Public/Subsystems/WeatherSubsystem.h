// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Basic/PXGameState.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Utilitys/PXCustomStruct.h"
#include "WeatherSubsystem.generated.h"


UCLASS(Blueprintable)
class PIXEL2DKIT_API UDayTimeEffectSight : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBuffValueEffect DawnEffect = {0.05f,10.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBuffValueEffect LightEffect = {0.05, 20.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBuffValueEffect DuskEffect = {-0.05, -10.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBuffValueEffect NightEffect = {-0.05, -20.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TEnumAsByte<EDayTimeType>, FGameplayTag> DayTimeType2BuffTag;	
};




UCLASS()
class PIXEL2DKIT_API UWeatherSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UDayTimeEffectSight* DayTimeEffectSight;
	
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	
	UFUNCTION(BlueprintCallable, Category = "WeatherSystem")
	void MakeWeatherEffect();
};
