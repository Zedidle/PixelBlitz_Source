// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyObject.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PIXEL2DKIT_API UMyObject : public UObject
{
	GENERATED_BODY()
	float height = 170.0f;
	
	public:
		UFUNCTION(BlueprintPure, BlueprintCallable, Category = "MyObject")
		float GetHeight();


	
	
};
