// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BasePixelGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UBasePixelGameInstance : public UGameInstance
{
	GENERATED_BODY()




	
public:

	// GeneralSetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeneralSetting")
	uint8 GeneralSetting_Language;



	
	// VideoSetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VideoSetting")
	int VideoSetting_CameraMode;











	// GameSetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSetting")
	bool GameSetting_ShowDamageFloatText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSetting")
	bool GameSetting_ShowRecoverFloatText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSetting")
	bool GameSetting_ShowGoldsFloatText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSetting")
	bool GameSetting_ShowBlood;

	
};
