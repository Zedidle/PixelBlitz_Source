// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UI/Player/BasePlayerStatusWidget.h"
#include "Utilitys/PXCustomStruct.h"
#include "PXCharacterDataAsset.generated.h"


UCLASS()
class PIXEL2DKIT_API UPXCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<UDataTable> AbilityDataTable;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<USoundCue> LandedSound;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<USoundCue> JumpSoundOnGround;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<USoundCue> JumpSoundInAir;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FCharacterAttribute CharacterAttribute;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<UBasePlayerStatusWidget> PlayerStatusWidgetClass;
};
