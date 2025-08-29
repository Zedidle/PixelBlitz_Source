// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Utilitys/PXCustomStruct.h"

#include "PXGameMode.generated.h"



/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PIXEL2DKIT_API APXGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EWeather> WeatherType;



	
#pragma region Sound Setting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Setting")
	float SoundSetting_Arg_MusicBasicMulti = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Setting")
	float SoundSetting_Arg_VolumeBasicMulti = 1.0f;
	
#pragma endregion





	
#pragma region Nav
	UFUNCTION(BlueprintCallable, Category = "Nav")
	void NavRebuild();
#pragma endregion
	
};
