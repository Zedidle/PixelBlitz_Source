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
class PIXELBLITZ_API APXPlayerController : public ACommonPlayerController
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

	// Possess 时同步角色缓存并把 PlayerState 的 ASC Avatar 重定向到新 Pawn，
	// 切换目标可能不是 ABasePXCharacter 系（如 B_SwitchCharacter_*），不能依赖角色的 PossessedBy
	virtual void OnPossess(APawn* InPawn) override;
	
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
