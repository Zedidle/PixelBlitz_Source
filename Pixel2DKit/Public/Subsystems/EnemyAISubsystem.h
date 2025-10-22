// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EnemyAISubsystem.generated.h"

class UCurveVector;

UCLASS()
class PIXEL2DKIT_API UEnemyAISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FName, UCurveVector*> ActionMoveCurveVector;

	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void BeginDestroy() override;




	UFUNCTION(BlueprintCallable, BlueprintPure)
	UCurveVector* GetActionMoveCurve(FName CurveName) const;
};
