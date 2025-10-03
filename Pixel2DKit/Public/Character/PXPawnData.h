// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Engine/DataAsset.h"
#include "PXPawnData.generated.h"

class APawn;
class UPXAbilitySet;
class UPXAbilityTagRelationshipMapping;
class UPXCameraMode;
class UPXInputConfig;
class UObject;

UCLASS(BlueprintType, Const, Meta = (DisplayName = "PX Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class PIXEL2DKIT_API UPXPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UPXPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from APXPawn or APXCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PX|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PX|Abilities")
	// TArray<TObjectPtr<UPXAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PX|Abilities")
	// TObjectPtr<UPXAbilityTagRelationshipMapping> TagRelationshipMapping;
	//
	// // Input configuration used by player controlled pawns to create input mappings and bind input actions.
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PX|Input")
	// TObjectPtr<UPXInputConfig> InputConfig;
	//
	// // Default camera mode used by player controlled pawns.
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PX|Camera")
	// TSubclassOf<UPXCameraMode> DefaultCameraMode;
};
