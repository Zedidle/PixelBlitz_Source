// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveVector.h"
#include "Engine/DataAsset.h"
#include "EnemyActionMoveDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FCurveFloatData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName CurveFloatName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCurveVector> Curve;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CheckBehindDistance = 0.0f;
};


UCLASS(BlueprintType)
class PIXEL2DKIT_API UEnemyActionMoveDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "EnemyActionMove")
	TArray<FCurveFloatData> ActionMoveCurveVectors;

	
};
