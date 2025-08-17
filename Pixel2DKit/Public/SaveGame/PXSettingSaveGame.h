// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PXSettingSaveGame.generated.h"

struct FEnhancedActionKeyMapping;

UCLASS()
class PIXEL2DKIT_API UPXSettingSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

#pragma region GeneralSetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | General")
	uint8 GeneralSetting_Language;
#pragma endregion
	
#pragma region VideoSetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Video")
	int VideoSetting_CameraMode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Video")
	int VideoSetting_Resolution;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Video")
	UTexture* VideoSetting_PPVColorGradingLUT;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Video")
	int VideoSetting_ScreenMode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Control")
	TArray<FEnhancedActionKeyMapping> ControlSetting_CharacterControl_KeyBoard_Mapping;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Control")
	TArray<FEnhancedActionKeyMapping> ControlSetting_CharacterControl_GamePad_Mapping;
#pragma endregion

#pragma region SoundSetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Sound")
	bool SoundSetting_ToggleAll = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Sound")
	float SoundSetting_VolumeValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Sound")
	float SoundSetting_MusicValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Sound")
	float SoundSetting_UIValue = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Sound")
	float SoundSetting_UIValue_BasicArg = 1.0f;
	
#pragma endregion

#pragma region InGameSetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | InGame")
	TSubclassOf<UPrimaryDataAsset> GameSetting_WeatherType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | InGame")
	float GameSetting_ViewPitch;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | InGame")
	bool GameSetting_ShowDamageFloatText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | InGame")
	bool GameSetting_ShowRecoverFloatText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | InGame")
	bool GameSetting_ShowGoldsFloatText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | InGame")
	bool GameSetting_ShowBlood;
#pragma endregion

	
};
