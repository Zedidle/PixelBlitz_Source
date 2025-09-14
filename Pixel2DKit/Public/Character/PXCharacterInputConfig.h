// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "PXCharacterInputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS(Blueprintable, BlueprintType)
class PIXEL2DKIT_API UPXCharacterInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	TMap<FGameplayTag, UInputAction*> ActionMap;	
	
};
