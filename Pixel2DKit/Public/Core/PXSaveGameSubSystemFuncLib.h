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
	static UPXSaveGameSubsystem* GetSaveGameSubsystem(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static UPXMainSaveGame* GetMainData(const UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static void SaveMainData(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static UPXBasicBuildSaveGame* GetBasicBuildData(const UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static void SaveBasicBuildData(const UObject* WorldContext);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static UPXAchievementsSaveGame* GetAchievementsData(const UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static void SaveAchievementsData(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static UPXShopSaveGame* GetShopData(const UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static void SaveShopData(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static UPXTalentsSaveGame* GetTalentsData(const UObject* WorldContext);
	UFUNCTION(BlueprintCallable, Category = SaveGame, meta = (WorldContext = "WorldContext"))
	static void SaveTalentsData(const UObject* WorldContext);
};
