// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PXCustomStruct.h"
#include "UObject/NoExportTypes.h"
#include "SettingFuncLib.generated.h"

/**
 * 
 */
UCLASS(Abstract, MinimalAPI)
class USettingFuncLib : public UObject
{
	GENERATED_BODY()

public:


	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Setting FuncLib")
	static UTexture* GetCameraFilter(ECameraColorFilter Filter);
};
