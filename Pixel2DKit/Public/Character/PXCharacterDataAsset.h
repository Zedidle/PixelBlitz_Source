// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PXCharacterInputConfig.h"
#include "Engine/DataAsset.h"
#include "UI/Player/BasePlayerStatusWidget.h"
#include "Utilitys/PXCustomStruct.h"
#include "PXCharacterDataAsset.generated.h"

class ABaseWeapon;

UCLASS()
class PIXEL2DKIT_API UPXCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<TSoftObjectPtr<UDataTable>> AbilityDataTables;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<UPXCharacterInputConfig> InputConfig;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<USoundCue> LandedSound;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<USoundCue> JumpSoundOnGround;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<USoundCue> JumpSoundInAir;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSoftObjectPtr<USoundCue> HurtSound;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FCharacterAttribute CharacterAttribute;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<UBasePlayerStatusWidget> PlayerStatusWidgetClass;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<ABaseWeapon> WeaponClass;
};
