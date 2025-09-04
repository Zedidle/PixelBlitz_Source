// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HeartsWidget.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UHeartsWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void FillHearts(int Num);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetMaxHeartsNum(int Num);
	
};
