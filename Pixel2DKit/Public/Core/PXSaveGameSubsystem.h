// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGame/PXAchievementsSaveGame.h"
#include "SaveGame/PXBasicBuildSaveGame.h"
#include "SaveGame/PXMainSaveGame.h"
#include "SaveGame/PXSettingSaveGame.h"
#include "SaveGame/PXTalentsSaveGame.h"
#include "SaveGame/PXShopSaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PXSaveGameSubsystem.generated.h"



UCLASS()
class PIXEL2DKIT_API UPXSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


	void InitData_Main();
	void InitData_BasicBuild();
	void InitData_Setting();
	void InitData_Achievements();
	void InitData_Shop();
	void InitData_Talents();
	
	
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	int UserIndex = 0;

	
#pragma region Main
	
	const FString SlotName_Main = "Main";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveGame | Main")
	UPXMainSaveGame* MainSaveGame;
	UFUNCTION(BlueprintCallable, Category = "SaveGame | Main")
	void SaveMainData();
	UPXMainSaveGame* LoadMainData();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SaveGame | Main")
	bool Main_HasChoiceAbility();

	UFUNCTION(BlueprintCallable, Category = "SaveGame | Main")
	void Main_TotalInit(TSubclassOf<UPrimaryDataAsset> WeatherType, EStyleType StyleType);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SaveGame | Main")
	bool Main_Saved();

	UFUNCTION(BlueprintCallable, Category = "SaveGame | Main")
	void Main_AddResult(float Result);

	UFUNCTION(BlueprintCallable, Category = "SaveGame | Main")
	void Main_SyncLevel();

#pragma endregion


#pragma region BasicBuild
	const FString SlotName_BasicBuild = "BasicBuild";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveGame | BasicBuild")
	UPXBasicBuildSaveGame* BasicBuildSaveGame;
	UFUNCTION(BlueprintCallable, Category = "SaveGame | BasicBuild")
	void SaveBasicBuildData();
	UPXBasicBuildSaveGame* LoadBasicBuildData();


	
#pragma endregion



#pragma region Settings

	const FString SlotName_Setting = "Setting";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveGame | Setting")
	UPXSettingSaveGame* SettingSaveGame;
	UFUNCTION(BlueprintCallable, Category = "SaveGame | Setting")
	void SaveSettingData();
	UPXSettingSaveGame* LoadSettingData();

#pragma endregion 


#pragma region Achievements

	const FString SlotName_Achievements = "Achievements";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveGame | Achievements")
	UPXAchievementsSaveGame* AchievementsSaveGame;
	UFUNCTION(BlueprintCallable, Category = "SaveGame | Achievements")
	void SaveAchievementsData();
	UPXAchievementsSaveGame* LoadAchievementsData();

#pragma endregion 


#pragma region Shop

	const FString SlotName_Shop = "Shop";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveGame | Shop")
	UPXShopSaveGame* ShopSaveGame;
	UFUNCTION(BlueprintCallable, Category = "SaveGame | Shop")
	void SaveShopData();
	UPXShopSaveGame* LoadShopData();

#pragma endregion 


#pragma region Talents
	
	const FString SlotName_Talents = "Talents";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveGame | Talents")
	UPXTalentsSaveGame* TalentsSaveGame;
	UFUNCTION(BlueprintCallable, Category = "SaveGame | Talents")
	void SaveTalentsData();
	UPXTalentsSaveGame* LoadTalentsData();

#pragma endregion 

	
	
};
