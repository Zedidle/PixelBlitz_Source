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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SaveGame | Main", meta = (AllowPrivateAccess = "true"))
	UPXMainSaveGame* MainSaveGame;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SaveGame | BasicBuild", meta = (AllowPrivateAccess = "true"))
	UPXBasicBuildSaveGame* BasicBuildSaveGame;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SaveGame | Setting", meta = (AllowPrivateAccess = "true"))
	UPXSettingSaveGame* SettingSaveGame;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SaveGame | Achievements", meta = (AllowPrivateAccess = "true"))
	UPXAchievementsSaveGame* AchievementsSaveGame;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SaveGame | Shop", meta = (AllowPrivateAccess = "true"))
	UPXShopSaveGame* ShopSaveGame;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SaveGame | Talents", meta = (AllowPrivateAccess = "true"))
	UPXTalentsSaveGame* TalentsSaveGame;
	
public:




	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	int UserIndex = 0;

	
#pragma region Main
	
	const FString SlotName_Main = "Main";
	UFUNCTION(BlueprintCallable, Category = "SaveGame | Main")
	void SaveMainData();
	UPXMainSaveGame* GetMainData();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SaveGame | Main")
	bool Main_HasChoiceAbility();


	// EStyleType StyleType 这个配置应该改为全局设置保存，开始时读取即可
	UFUNCTION(BlueprintCallable, Category = "SaveGame | Main")
	void Main_TotalInit();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SaveGame | Main")
	bool Main_Saved();

	UFUNCTION(BlueprintCallable, Category = "SaveGame | Main")
	void Main_AddResult(float Result);

	UFUNCTION(BlueprintCallable, Category = "SaveGame | Main")
	void Main_SyncLevel();

#pragma endregion


#pragma region BasicBuild
	const FString SlotName_BasicBuild = "BasicBuild";
	UFUNCTION(BlueprintCallable, Category = "SaveGame | BasicBuild")
	void SaveBasicBuildData();
	UPXBasicBuildSaveGame* GetBasicBuildData();
	
#pragma endregion



#pragma region Settings

	const FString SlotName_Setting = "Setting";

	UFUNCTION(BlueprintCallable, Category = "SaveGame | Setting")
	void SaveSettingData();
	UPXSettingSaveGame* GetSettingData();

#pragma endregion 


#pragma region Achievements

	const FString SlotName_Achievements = "Achievements";

	UFUNCTION(BlueprintCallable, Category = "SaveGame | Achievements")
	void SaveAchievementsData();
	UPXAchievementsSaveGame* GetAchievementsData();

	UFUNCTION(BlueprintCallable, Category = "SaveGame | Achievements")
	bool CompleteAchievement(FName AchievementKey);
	
#pragma endregion 


#pragma region Shop

	const FString SlotName_Shop = "Shop";

	UFUNCTION(BlueprintCallable, Category = "SaveGame | Shop")
	void SaveShopData();
	UPXShopSaveGame* GetShopData();

#pragma endregion 


#pragma region Talents
	
	const FString SlotName_Talents = "Talents";

	UFUNCTION(BlueprintCallable, Category = "SaveGame | Talents")
	void SaveTalentsData();
	UPXTalentsSaveGame* GetTalentsData();

#pragma endregion 

	
	
};
