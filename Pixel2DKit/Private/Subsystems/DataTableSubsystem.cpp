// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/DataTableSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"


void UDataTableSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}


UDataTable* UDataTableSubsystem::GetDataTable(FString InName)
{
	if (DataTableMap.Contains(*InName))
	{
		return DataTableMap[*InName];
	}
	
	if (UDataTable* NewDataTable = LoadObject<UDataTable>(nullptr, *InName))
	{
		DataTableMap.Add(*InName, NewDataTable);
		return NewDataTable;
	}

	FString ConfigPath = GetConfigPath(InName);
	if (UDataTable* NewDataTable = LoadObject<UDataTable>(nullptr, *ConfigPath))
	{
		DataTableMap.Add(*InName, NewDataTable);
		return NewDataTable;
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to load data table: %s"), *InName);
	return nullptr;
}
