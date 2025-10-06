// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Player/PlayerRespawnPoint.h"
#include "Utilitys/PXCustomStruct.h"

#include "PXGameMode.generated.h"



class APlayerRespawnPoint;
class ULevelStreamingDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelLoadedSignature);

UCLASS(BlueprintType, Blueprintable)
class PIXEL2DKIT_API APXGameMode : public AGameMode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FName PreLevelName;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FName CurLevelName;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	ULevelStreamingDynamic* CurLevelInstance;

	bool IsLevelLoaded = false;


	
protected:
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	
public:


	UPROPERTY(BlueprintAssignable)
	FOnLevelLoadedSignature OnStartLevelLoaded;
	
#pragma region GameplayFlow
	UPROPERTY(BlueprintReadOnly, Category=GameplayFlow)
	APlayerRespawnPoint* PlayerRespawnPoint;


	
	
	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void LoadLevel(FName LevelName, FVector StartLocation=FVector(0), FRotator StartRotation= FRotator(0, 0,0));

	// 延迟加载效果
	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void StartCurLevel();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=GameplayFlow)
	void OnStartLevelSuccess();
	
	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void UnloadCurLevel();
	
	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void HandleLevelUnloaded();
	
	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void PrepareForRole();

	
#pragma endregion 



	

	
#pragma region Sound Setting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Setting")
	float SoundSetting_Arg_MusicBasicMulti = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Setting")
	float SoundSetting_Arg_VolumeBasicMulti = 1.0f;
	
#pragma endregion

	
#pragma region Nav
	UFUNCTION(BlueprintCallable, Category = "Nav")
	void NavRebuild();
#pragma endregion

	UFUNCTION(BlueprintNativeEvent)
	void OnLevelLoaded();
	
};
