// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "PXPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API APXPlayerController : public APlayerController
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool GameAlreadyStart = false;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool CharacterControlling = false;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool Pausing = false;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	bool PausingToStartCounting = false;
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input)
	TSoftObjectPtr<UInputMappingContext> IMC_GamePad;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input)
	TSoftObjectPtr<UInputMappingContext> IMC_KeyBoard;
	
	UFUNCTION(BlueprintCallable, Category = Game)
	void OnGameStart();

	UFUNCTION(BlueprintCallable, Category = Game)
	void OnCharacterControl(bool On);
};
