// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/AbilityFuncLib.h"

#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/DataTableSettings.h"


void UAbilityFuncLib::UnlockAbility(UObject* WorldContextObject, const FGameplayTag& Tag)
{
	const UDataTableSettings& DataTableSettings = UDataTableSettings::Get();

	if (DataTableSettings.GetAbilityDataByTag(Tag))
	{
		UPXBasicBuildSaveGame* BasicBuildSaveGame = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(WorldContextObject);
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BasicBuildSaveGame)
		BasicBuildSaveGame->UnlockedAbilities.AddUnique(Tag);
		UPXSaveGameSubSystemFuncLib::SaveBasicBuildData(WorldContextObject);
	}
}

bool UAbilityFuncLib::AbilityHadUnlock(UObject* WorldContextObject, const FGameplayTag& Tag)
{
	UPXBasicBuildSaveGame* BasicBuildSaveGame = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(WorldContextObject);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(BasicBuildSaveGame, false);
	return BasicBuildSaveGame->UnlockedAbilities.Contains(Tag);
}

FAbility UAbilityFuncLib::GetAbilityByTag(const FGameplayTag& Tag)
{
	if (!Tag.IsValid()) return FAbility();
	
	const UDataTableSettings& DataTableSettings = UDataTableSettings::Get();
	if (const FAbility* Ability = DataTableSettings.GetAbilityDataByTag(Tag))
	{
		return *Ability;
	}
	return FAbility();
}

void UAbilityFuncLib::UnlockTalent(UObject* WorldContextObject, const FGameplayTag& Tag)
{
	const UDataTableSettings& DataTableSettings = UDataTableSettings::Get();

	if (DataTableSettings.GetTalentDataByTag(Tag))
	{
		UPXBasicBuildSaveGame* BasicBuildSaveGame = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(WorldContextObject);
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BasicBuildSaveGame)
		BasicBuildSaveGame->UnlockedTalents.AddUnique(Tag);
		UPXSaveGameSubSystemFuncLib::SaveBasicBuildData(WorldContextObject);

		// 天赋解锁界面弹出
		// WB_Talent_Unlock  CreateWidget  AddToViewPort(1000)
	}
}

bool UAbilityFuncLib::TalentHadUnlock(UObject* WorldContextObject, const FGameplayTag& Tag)
{
	UPXBasicBuildSaveGame* BasicBuildSaveGame = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(WorldContextObject);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(BasicBuildSaveGame, false);
	return BasicBuildSaveGame->UnlockedTalents.Contains(Tag);
}

FTalent UAbilityFuncLib::GetTalentByTag(const FGameplayTag& Tag)
{
	if (!Tag.IsValid()) return FTalent();

	const UDataTableSettings& DataTableSettings = UDataTableSettings::Get();
	if (const FTalent* Talent = DataTableSettings.GetTalentDataByTag(Tag))
	{
		return *Talent;
	}
	return FTalent();
}
