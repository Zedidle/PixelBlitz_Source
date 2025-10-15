// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "InputMappingContext.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "PXPlayerController.generated.h"


struct FDefaultEmptyMessage;

UCLASS()
class PIXEL2DKIT_API APXPlayerController : public ACommonPlayerController
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
	UInputMappingContext* IMC_Default;
	
protected:


	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	UPROPERTY(BlueprintReadOnly)
	class ABasePXCharacter* PXCharacter = nullptr;

	UFUNCTION(BlueprintCallable, Category = Game)
	void OnGameStart();

	UFUNCTION(BlueprintCallable, Category = Game)
	void OnCharacterControl(bool On);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Game)
	bool CanPause();




	UFUNCTION(BlueprintNativeEvent)
	void OnLevelLoading(FGameplayTag Channel, const FDefaultEmptyMessage& Message);

	UFUNCTION(BlueprintNativeEvent)
	void OnLevelLoaded(FGameplayTag Channel, const FDefaultEmptyMessage& Message);



#pragma region Massage
	
	FGameplayMessageListenerHandle ListenerHandle_OnLevelLoading;
	FGameplayMessageListenerHandle ListenerHandle_OnLevelLoaded;
	
#pragma endregion
	
};
