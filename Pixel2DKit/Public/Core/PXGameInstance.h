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

	UPROPERTY(BlueprintReadWrite, Category = "Game")
	bool bGameStarted = false;

	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void StartNewGame();

	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void StartGame();

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Game")
	FName GetCurLevelName(bool Continue);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Game")
	FName GetCurLevelName_Simple(bool Next);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game")
	void GetTotalUseTime(float& usetime, bool& newrecord);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Game")
	void OnPlayerDead(bool& End);
	
};
