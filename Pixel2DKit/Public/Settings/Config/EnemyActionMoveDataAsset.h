// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveVector.h"
#include "Engine/DataAsset.h"
#include "EnemyActionMoveDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class PIXEL2DKIT_API UEnemyActionMoveDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	// 受惊
	UPROPERTY(EditAnywhere, Category = "EnemyActionMove")
	TSoftObjectPtr<UCurveVector> CV_Startled;
	
	// 后撤步
	UPROPERTY(EditAnywhere, Category = "EnemyActionMove")
	TSoftObjectPtr<UCurveVector> CV_DropBack;

	// 突进
	UPROPERTY(EditAnywhere, Category = "EnemyActionMove")
	TSoftObjectPtr<UCurveVector> CV_Lunge = nullptr;

	// 蓄力突进
	UPROPERTY(EditAnywhere, Category = "EnemyActionMove")
	TSoftObjectPtr<UCurveVector> CV_WindLunge = nullptr;
	
	// 侧闪
	UPROPERTY(EditAnywhere, Category = "EnemyActionMove")
	TSoftObjectPtr<UCurveVector> CV_Dodge = nullptr;

	// 向较高的平台跳跃
	UPROPERTY(EditAnywhere, Category = "EnemyActionMove")
	TSoftObjectPtr<UCurveVector> CV_JumpToHigher = nullptr;

	// 向较低的平台跳跃
	UPROPERTY(EditAnywhere, Category = "EnemyActionMove")
	TSoftObjectPtr<UCurveVector> CV_JumpToLower = nullptr;

	
	
};
