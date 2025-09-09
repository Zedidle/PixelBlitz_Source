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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static UPXSaveGameSubsystem* GetSaveGameSubsystem(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static UPXMainSaveGame* GetMainData(UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static void SaveMainData(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static UPXBasicBuildSaveGame* GetBasicBuildData(UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static void SaveBasicBuildData(UObject* WorldContext);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static UPXAchievementsSaveGame* GetAchievementsData(UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static void SaveAchievementsData(UObject* WorldContext);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static UPXSettingSaveGame* GetSettingData(UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static void SaveSettingData(UObject* WorldContext);

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static UPXShopSaveGame* GetShopData(UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static void SaveShopData(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static UPXTalentsSaveGame* GetTalentsData(UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static void SaveTalentsData(UObject* WorldContext);
};
