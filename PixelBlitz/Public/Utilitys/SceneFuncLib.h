// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SceneFuncLib.generated.h"

/**
 * 
 */
UCLASS(Abstract, MinimalAPI)
class USceneFuncLib : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Quat")
	static void SetActorWorldRotationByQuat(AActor* Actor, const FQuat& NewRotation);


	UFUNCTION(BlueprintCallable, Category="Quat")
	static void AddActorWorldRotationByQuat(AActor* Actor, const FQuat& DeltaRotation);

	UFUNCTION(BlueprintCallable, Category="Quat")
	static void SetSceneComponentRotationByQuat(USceneComponent* Component, const FQuat& NewRotation);

	
};
