// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Utilitys/PXCustomStruct.h"
#include "PXTalentsSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXTalentsSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Talents")
	TMap<TEnumAsByte<EWeather>, int> Talent_KillBOSSNum_InWeatherType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Talents")
	int Talent_TotalPickupGolds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Talents")
	int Talent_TotalDeathNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Talents")
	int Talent_TotalPerfectDodgeTimes;
};
