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
	
#pragma region GeneralSetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeneralSetting")
	uint8 GeneralSetting_Language;
#pragma endregion



#pragma region VideoSetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VideoSetting")
	int VideoSetting_CameraMode;
#pragma endregion


	
#pragma region SoundSetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundSetting")
	bool SoundSetting_ToggleAll = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundSetting")
	float SoundSetting_VolumeValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundSetting")
	float SoundSetting_MusicValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundSetting")
	float SoundSetting_UIValue = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundSetting")
	float SoundSetting_UIValue_BasicArg = 1.0f;
	
#pragma endregion

	
#pragma region GameSetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSetting")
	bool GameSetting_ShowDamageFloatText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSetting")
	bool GameSetting_ShowRecoverFloatText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSetting")
	bool GameSetting_ShowGoldsFloatText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameSetting")
	bool GameSetting_ShowBlood;
#pragma endregion

	
};
