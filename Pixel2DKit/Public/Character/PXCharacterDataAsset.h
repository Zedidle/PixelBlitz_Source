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
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TArray<TSoftObjectPtr<UDataTable>> AbilityDataTables;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TSoftObjectPtr<UPXCharacterInputConfig> InputConfig;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TSoftObjectPtr<USoundCue> LandedSound;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TSoftObjectPtr<USoundCue> JumpSoundOnGround;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TSoftObjectPtr<USoundCue> JumpSoundInAir;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TSoftObjectPtr<USoundCue> HurtSound;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	FCharacterAttribute CharacterAttribute;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TSubclassOf<UBasePlayerStatusWidget> PlayerStatusWidgetClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	TSubclassOf<ABaseWeapon> WeaponClass;

	// 死亡复活所需时间
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset | GameplayFlow")
	float ReviveDelayTime = 3.0f;

	// 死亡复活期间过渡时间
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "HeroDataAsset")
	float RevivePassTime = 600.0f;
};
