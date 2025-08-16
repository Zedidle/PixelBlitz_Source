// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "Engine/GameInstance.h"
#include "Utilitys/PXCustomStruct.h"
#include "PXGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXGameInstance : public UGameInstance
{
	GENERATED_BODY()




	
public:


#pragma region GeneralSetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | General")
	uint8 GeneralSetting_Language;
#pragma endregion

#pragma region MainSetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	int Main_SupLife;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	int Main_MaxLife;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	TArray<FName> Main_ChoicedAbilityIndexes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	FName Main_CurLevelChoiceAbility;

	// 前一关是否清空了所有怪物, 不用保存
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	bool Main_bPreLevelClearAllMonster;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	TArray<FName> Main_TakeEffectAbilityIndexes;

	// 每一关的过关时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	TArray<float> Main_Results;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	FString Main_CurLevelSeriesName;

	// 整个过关过程死亡次数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	int Main_LostLife;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	bool Main_bPlayerInjured;

	// （非保存）当前游戏流逝时间，同一关内通用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	float Main_SkyTimeOfDay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	TEnumAsByte<EStyleType> Main_StyleType;
	
	// 这一局内是否仍然保持全部清除怪物
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	bool Main_TotalMonsterClear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	int Main_RoundGoldNum;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	int Main_PlayerBehurtedTimes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	bool Main_PickupAllGolds;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	int Main_TotalPerfectDodgeTimes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	int Main_TotalAttackInDashTimes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	TSubclassOf<UPrimaryDataAsset> Main_CurWeatherType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
 	FCharacterAttribute Main_CharacterInheritAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	FName Main_CurCharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	FName Main_CurLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Main")
	TArray<FName> Main_SurLevels;
	
	
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
