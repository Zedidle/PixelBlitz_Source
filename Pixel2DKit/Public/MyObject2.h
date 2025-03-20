// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyObject2.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PIXEL2DKIT_API UMyObject2 : public UObject
{
	GENERATED_BODY()
	class UMyObject* o1 = nullptr;
	
	public:
		UFUNCTION(BlueprintCallable)
		float GetHeight2();

	
	
};
