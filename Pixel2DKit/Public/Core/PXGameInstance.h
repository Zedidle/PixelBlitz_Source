// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "Engine/GameInstance.h"
#include "Utilitys/PXCustomStruct.h"
#include "PXGameInstance.generated.h"


UENUM(BlueprintType)
enum class ELevelType : uint8
{
	None UMETA(DisplayName = "无"),
	Race UMETA(DisplayName = "战斗竞速"),
	Arena UMETA(DisplayName = "竞技场"),
};


UCLASS()
class PIXEL2DKIT_API UPXGameInstance : public UGameInstance
{
	GENERATED_BODY()


public:

	UPROPERTY(BlueprintReadWrite, Category = "Game")
	bool bGameStarted = false;

	UPROPERTY(BlueprintReadWrite, Category = "Game")
	ELevelType LevelType = ELevelType::Race;

	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void SetLevelType(ELevelType _LevelType = ELevelType::Race);
	
	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void StartNewGame();

	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void StartGame();

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Game")
	FName GetCurLevelName(bool Continue);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Game")
	FName GetCurLevelName_Simple(bool Next);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Game")
	FName GetCurLevelNameByNum(int Num);



	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game")
	void GetTotalUseTime(float& usetime, bool& newrecord);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Game")
	void OnPlayerDead(bool& End);
	
};
