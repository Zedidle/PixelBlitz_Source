// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/AbilityFuncLib.h"

#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Subsystems/DataTableSubsystem.h"


void UAbilityFuncLib::UnlockAbility(UObject* WorldContextObject, const FGameplayTag& Tag)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem)

	const FAbility& AbilityData = DataTableSubsystem->GetAbilityDataByTag(Tag);
	if (!AbilityData.AbilityTag.IsValid()) return;
	
	UPXBasicBuildSaveGame* BasicBuildSaveGame = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(WorldContextObject);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BasicBuildSaveGame)
	BasicBuildSaveGame->UnlockedAbilities.Add(Tag);
	UPXSaveGameSubSystemFuncLib::SaveBasicBuildData(WorldContextObject);
}

bool UAbilityFuncLib::AbilityHadUnlock(UObject* WorldContextObject, const FGameplayTag& Tag)
{
	UPXBasicBuildSaveGame* BasicBuildSaveGame = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(WorldContextObject);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(BasicBuildSaveGame, false);
	return BasicBuildSaveGame->UnlockedAbilities.Contains(Tag);
}



void UAbilityFuncLib::UnlockTalent(UObject* WorldContextObject, const FGameplayTag& Tag)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem)

	const FTalent& TalentData = DataTableSubsystem->GetTalentDataByTag(Tag);
	if (!TalentData.TalentTag.IsValid()) return;

	UPXBasicBuildSaveGame* BasicBuildSaveGame = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(WorldContextObject);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BasicBuildSaveGame)
	BasicBuildSaveGame->UnlockedTalents.AddUnique(Tag);
	UPXSaveGameSubSystemFuncLib::SaveBasicBuildData(WorldContextObject);

	// 天赋解锁界面弹出
	// WB_Talent_Unlock  CreateWidget  AddToViewPort(1000)
	
}

bool UAbilityFuncLib::TalentHadUnlock(UObject* WorldContextObject, const FGameplayTag& Tag)
{
	UPXBasicBuildSaveGame* BasicBuildSaveGame = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(WorldContextObject);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(BasicBuildSaveGame, false);
	return BasicBuildSaveGame->UnlockedTalents.Contains(Tag);
}
