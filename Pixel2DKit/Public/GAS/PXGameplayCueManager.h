// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "PXGameplayCueManager.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()

	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override;


};
