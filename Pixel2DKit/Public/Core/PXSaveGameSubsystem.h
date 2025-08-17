// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGame/PXBasicBuildSaveGame.h"
#include "SaveGame/PXMainSaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PXSaveGameSubsystem.generated.h"

// 定义各类存档的结构



UCLASS()
class PIXEL2DKIT_API UPXSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;






	
	int UserIndex = 0;
	TMap<FString, bool> LoadedRecord;
	
#pragma region Main
	
	const FString SlotName_Main = "Main";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveGame | Main")
	UPXMainSaveGame* MainSaveGame;
	UFUNCTION(BlueprintCallable, Category = "SaveGame | Main")
	void SaveMainData();
	UPXMainSaveGame* GetMainData();

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
	FPXBasicBuildSaveData BasicBuildSaveData;
	UFUNCTION(BlueprintCallable, Category = "SaveGame | BasicBuild")
	void SaveBasicBuildData();
	FPXBasicBuildSaveData& GetBasicBuildData();
	
#pragma endregion


	
};
