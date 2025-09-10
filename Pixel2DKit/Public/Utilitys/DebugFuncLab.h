// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DebugFuncLab.generated.h"



UCLASS(Abstract, MinimalAPI)
class UDebugFuncLab : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Debug")
	static void ScreenMessage(FString message, float SustainTime = 3, FColor Color = FColor::Red);

};
