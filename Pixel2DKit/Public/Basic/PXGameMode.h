// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PlayerState/PlayerRespawnPoint.h"
#include "Utilitys/PXCustomStruct.h"

#include "PXGameMode.generated.h"



class APlayerRespawnPoint;
class ULevelStreamingDynamic;


UCLASS(BlueprintType, Blueprintable)
class PIXEL2DKIT_API APXGameMode : public AGameMode
{
	GENERATED_BODY()

	FString PreLevelName;
	FString CurLevelName;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	ULevelStreamingDynamic* CurLevelInstance;
	
public:

#pragma region GameplayFlow
	UPROPERTY(BlueprintReadOnly, Category=GameplayFlow)
	APlayerRespawnPoint* PlayerRespawnPoint;

	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void LoadLevel(FString LevelName, FVector Location);


	
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
	
};
