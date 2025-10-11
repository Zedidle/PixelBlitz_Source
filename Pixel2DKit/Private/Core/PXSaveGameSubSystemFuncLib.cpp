// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"

UPXSaveGameSubsystem* UPXSaveGameSubSystemFuncLib::GetSaveGameSubsystem(const UObject* WorldContext)
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

UPXMainSaveGame* UPXSaveGameSubSystemFuncLib::GetMainData(const UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		return SaveGameSubsystem->GetMainData();
	}
	return nullptr;
}

void UPXSaveGameSubSystemFuncLib::SaveMainData(const UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		SaveGameSubsystem->SaveMainData();
	}
}

UPXBasicBuildSaveGame* UPXSaveGameSubSystemFuncLib::GetBasicBuildData(const UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		return SaveGameSubsystem->GetBasicBuildData();
	}
	return nullptr;
}

void UPXSaveGameSubSystemFuncLib::SaveBasicBuildData(const UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		SaveGameSubsystem->SaveBasicBuildData();
	}
}

UPXAchievementsSaveGame* UPXSaveGameSubSystemFuncLib::GetAchievementsData(const UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		return SaveGameSubsystem->GetAchievementsData();
	}
	return nullptr;
}

void UPXSaveGameSubSystemFuncLib::SaveAchievementsData(const UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		SaveGameSubsystem->SaveAchievementsData();
	}
}

UPXShopSaveGame* UPXSaveGameSubSystemFuncLib::GetShopData(const UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		return SaveGameSubsystem->GetShopData();
	}
	return nullptr;
}

void UPXSaveGameSubSystemFuncLib::SaveShopData(const UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		SaveGameSubsystem->SaveShopData();
	}
}

UPXTalentsSaveGame* UPXSaveGameSubSystemFuncLib::GetTalentsData(const UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		return SaveGameSubsystem->GetTalentsData();
	}
	return nullptr;
}

void UPXSaveGameSubSystemFuncLib::SaveTalentsData(const UObject* WorldContext)
{
	if (UPXSaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem(WorldContext))
	{
		SaveGameSubsystem->SaveTalentsData();
	}
}
