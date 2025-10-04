// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "PXPlayerController.generated.h"


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

	UPROPERTY(BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
	UInputMappingContext* IMC_GamePad;
	UPROPERTY(BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess))
	UInputMappingContext* IMC_KeyBoard;
	
protected:


	virtual void BeginPlay() override;
	
	
public:
	UPROPERTY(BlueprintReadOnly)
	class ABasePXCharacter* PXCharacter = nullptr;
	
	// 是否处于手柄控制
	UPROPERTY(BlueprintReadOnly)
	bool GamePadControlling = false;

	UFUNCTION(BlueprintCallable, Category = Game)
	void OnGameStart();

	UFUNCTION(BlueprintCallable, Category = Game)
	void OnCharacterControl(bool On);

	// UFUNCTION(BlueprintCallable, BlueprintPure, Category = Game)
	// UInputMappingContext* GetCurrentIMC();
};
