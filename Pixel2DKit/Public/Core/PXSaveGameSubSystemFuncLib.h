// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/PXSaveGameSubsystem.h"
#include "PXSaveGameSubSystemFuncLib.generated.h"

UCLASS(Abstract, MinimalAPI)
class UPXSaveGameSubSystemFuncLib : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = SaveGame)
	static UPXSaveGameSubsystem* GetSaveGameSubsystem(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame)
	static UPXMainSaveGame* GetMainData(UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame)
	static void SaveMainData(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame)
	static UPXBasicBuildSaveGame* GetBasicBuildData(UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame)
	static void SaveBasicBuildData(UObject* WorldContext);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame)
	static UPXAchievementsSaveGame* GetAchievementsData(UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame)
	static void SaveAchievementsData(UObject* WorldContext);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame)
	static UPXSettingSaveGame* GetSettingData(UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame)
	static void SaveSettingData(UObject* WorldContext);

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame)
	static UPXShopSaveGame* GetShopData(UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame)
	static void SaveShopData(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame)
	static UPXTalentsSaveGame* GetTalentsData(UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame)
	static void SaveTalentsData(UObject* WorldContext);
};
