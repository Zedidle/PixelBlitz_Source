// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PixelGameModeBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PIXEL2DKIT_API APixelGameModeBase : public AGameModeBase
{
	GENERATED_BODY()



	public:

	UFUNCTION(BlueprintCallable, Category = "Nav")
	void NavRebuild();
	
	
};
