// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"

UPXSaveGameSubsystem* UPXSaveGameSubSystemFuncLib::GetSaveGameSubsystem(UObject* WorldContext)
{
	if (!WorldContext) return nullptr;
	
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContext);
	if (!GameInstance) return nullptr;

	if (UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>())
	{
		return SaveGameSubsystem;
	}
	
	return nullptr;
}

UPXMainSaveGame* UPXSaveGameSubSystemFuncLib::GetMainData(UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		return SaveGameSubsystem->GetMainData();
	}
	return nullptr;
}

void UPXSaveGameSubSystemFuncLib::SaveMainData(UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		SaveGameSubsystem->SaveMainData();
	}
}

UPXBasicBuildSaveGame* UPXSaveGameSubSystemFuncLib::GetBasicBuildData(UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		return SaveGameSubsystem->GetBasicBuildData();
	}
	return nullptr;
}

void UPXSaveGameSubSystemFuncLib::SaveBasicBuildData(UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		SaveGameSubsystem->SaveBasicBuildData();
	}
}

UPXAchievementsSaveGame* UPXSaveGameSubSystemFuncLib::GetAchievementsData(UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		return SaveGameSubsystem->GetAchievementsData();
	}
	return nullptr;
}

void UPXSaveGameSubSystemFuncLib::SaveAchievementsData(UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		SaveGameSubsystem->SaveAchievementsData();
	}
}

UPXSettingSaveGame* UPXSaveGameSubSystemFuncLib::GetSettingData(UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		return SaveGameSubsystem->GetSettingData();
	}
	return nullptr;
}

void UPXSaveGameSubSystemFuncLib::SaveSettingData(UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		SaveGameSubsystem->SaveSettingData();
	}
}

UPXShopSaveGame* UPXSaveGameSubSystemFuncLib::GetShopData(UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		return SaveGameSubsystem->GetShopData();
	}
	return nullptr;
}

void UPXSaveGameSubSystemFuncLib::SaveShopData(UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		SaveGameSubsystem->SaveShopData();
	}
}

UPXTalentsSaveGame* UPXSaveGameSubSystemFuncLib::GetTalentsData(UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		return SaveGameSubsystem->GetTalentsData();
	}
	return nullptr;
}

void UPXSaveGameSubSystemFuncLib::SaveTalentsData(UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		SaveGameSubsystem->SaveTalentsData();
	}
}
