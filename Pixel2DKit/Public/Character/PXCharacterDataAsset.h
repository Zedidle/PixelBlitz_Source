// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PXCharacterDataAsset.generated.h"


UCLASS()
class PIXEL2DKIT_API UPXCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<USoundCue> LandedSound;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 MaxJumpCount = 2;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float BasicJumpMaxHoldTime = 0.3f;
	
};
