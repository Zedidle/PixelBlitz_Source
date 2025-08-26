// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ArrowLineWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class PIXEL2DKIT_API UArrowLineWidget : public UUserWidget
{
	GENERATED_BODY()



	bool TwoWay = true;
	int MaxArrowBodyNum = 10;
	float MaxDistance = 600;
	FLinearColor Color = FLinearColor::White;
	float LifeSpan = 1.5;
	FVector2D BodySize = FVector2D(36, 54);
	FVector2D HeadSize = FVector2D(64, 64);
	FVector StartOffset = FVector::Zero();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	AActor* StartActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	AActor* EndActor;
	
	UFUNCTION()
	void InitializeData(AActor* _StartActor, AActor* _EndActor, bool _TwoWay = true, int _MaxArrowBodyNum = 10,
		float _MaxDistance = 600, FLinearColor _Color = FLinearColor::White, float _LifeSpan = 1.5,
		FVector2D _BodySize = FVector2D(36, 54), FVector2D _HeadSize = FVector2D(64, 64),
		FVector _StartOffset = FVector::Zero()
		);
	
};
