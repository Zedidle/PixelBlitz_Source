// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PXShopSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXShopSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TMap<FName, int> Shop_ItemRemainNum;

};
