// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PXGameDataAsset.generated.h"


class UDayTimeEffectSight;
class UInputMappingContext;
class UPXGameplayEffect;

UCLASS(BlueprintType)
class PIXEL2DKIT_API UPXGameDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
	public:
		// 一个可以同时处理手柄和键盘的映射
    	UPROPERTY(EditAnywhere, Category = "Input")
    	TSoftObjectPtr<UInputMappingContext> IMC_Default;
    
    	UPROPERTY(EditAnywhere, Category = "Weather")
    	TSoftObjectPtr<UDayTimeEffectSight> DayTimeEffect;
    
    	UPROPERTY(EditAnywhere, Category = "GAS | GameplayEffect")
    	TSubclassOf<UPXGameplayEffect> GE_WeatherEffect_HP;
    
    	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GameplayFlow | Ability")	
    	int BasePerLevelSkillPointsGet = 2;

		UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GameplayFlow | Ability")	
		int BasePerLevelSkillRefreshPointsGet = 2;
};
