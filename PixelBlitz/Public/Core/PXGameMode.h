// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "GameFramework/GameMode.h"
#include "Player/PlayerRespawnPoint.h"

#include "PXGameMode.generated.h"




class APlayerRespawnPoint;
class ULevelStreamingDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartLevelSuccessSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadingLevelSignature);

UCLASS(BlueprintType, Blueprintable)
class PIXELBLITZ_API APXGameMode : public AGameMode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FName PreLevelName;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FName CurLevelName;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	ULevelStreamingDynamic* CurLevelInstance;

	// 当前关卡的全部流送实例（FLevelData.LevelInstanceNames 支持多实例，CurLevelInstance 指向首个）
	TArray<TObjectPtr<ULevelStreamingDynamic>> CurLevelInstances;

	// 尚未完成加载的实例数，归零即整关就绪
	int32 PendingLevelInstanceCount = 0;

	// 已完成加载处理的实例（防止 OnLevelLoaded 重复触发时重复处理）
	TSet<TObjectPtr<ULevelStreamingDynamic>> ProcessedLevelInstances;

	bool IsLevelLoaded = false;
	bool IsLevelStarted = false;

	
protected:
	
	APXGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> GameStartCountWidgetClass;
	
public:
	
#pragma region GameplayFlow
	UPROPERTY(BlueprintReadOnly, Category=GameplayFlow)
	APlayerRespawnPoint* PlayerRespawnPoint;
	
	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void LoadLevel(FName LevelName, FVector StartLocation=FVector(0));

	UFUNCTION(BlueprintNativeEvent, Category=GameplayFlow)
	void DissolvePreLevel(const FName& LevelName, FVector NewLevelLocation);
	
	// 尝试开始当前关卡
	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void TryStartCurLevel();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=GameplayFlow)
	void OnLevelStarted();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=GameplayFlow)
	void ClearPreLevel();
	
	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void PrepareGame();

	
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

	UFUNCTION()
	void OnLevelLoaded();
	
};
