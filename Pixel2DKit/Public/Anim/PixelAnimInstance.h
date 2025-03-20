// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimInstance.h"
#include "PixelAnimInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PIXEL2DKIT_API UPixelAnimInstance : public UPaperZDAnimInstance
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	void TestFunction();


	
};
