// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIEventCenter.generated.h"


UCLASS()
class PIXEL2DKIT_API UUIEventCenter : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	static UUIEventCenter* Get(const UObject* WorldContextObject);

	UFUNCTION()
	static UUIEventCenter* GetEventCenter(const UObject* WorldContextObject) { return Get(WorldContextObject); }

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterHPChange, float, HPPercent);
	FOnCharacterHPChange OnCharacterHPChange;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterRadiationChange, float, HPPercent);
	FOnCharacterRadiationChange OnCharacterRadiationChange;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterStaminaChange, float, StaminaPercent);
	FOnCharacterStaminaChange OnCharacterStaminaChange;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShowCrossChair, FVector2D, HPPercent);
	FShowCrossChair OnCrossChairShow;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHideCrossChair);
	FHideCrossChair OnCrossChairHide;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogEvent, FName&, DialogEvent);
	FOnDialogEvent OnDialogEvent;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivateWeapon);
	FOnActivateWeapon OnActivateWeapon;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBotHit);
	FOnBotHit OnBotHit;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildModeSelectFirstTab, int32, SelectIdx);
	FOnBuildModeSelectFirstTab OnBuildModeSelectFirstTab;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildModeSelectBuildItem, int32, SelectIdx);
	FOnBuildModeSelectBuildItem OnBuildModeSelectBuildItem;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuildItemRefreshCanBuildNum);
	FOnBuildItemRefreshCanBuildNum OnBuildItemRefreshCanBuildNum;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopPlaceBuilding);
	FOnStopPlaceBuilding OnStopPlaceBuilding;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCommonTipRemove, UCommonTipWidget*, Tip);
	FOnCommonTipRemove OnCommonTipRemove;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectMenuListChange);
	FOnSelectMenuListChange OnSelectMenuListChange;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftMouseClick);
	FOnLeftMouseClick OnLeftMouseClick;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHandCraftClickChangeMaterial, int32, ItemID);
	FOnHandCraftClickChangeMaterial OnHandCraftClickChangeMaterial;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHandCraftSelectMaterial, int32, ItemIdx);
	FOnHandCraftSelectMaterial OnHandCraftSelectMaterial;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEncumbranceOrCapacityChange, int32, Capacity, int32, Encumbrance);
	FOnEncumbranceOrCapacityChange OnEncumbranceOrCapacityChange;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBackpackItemClick, const FGuid&, ItemUID);
	FOnBackpackItemClick OnBackpackItemClick;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPressRKey);
	FOnPressRKey OnPressRKey;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBackpackItemClickSplit, const FGuid&, ItemUID);
	FOnBackpackItemClickSplit OnBackpackItemClickSplit;
	
};
