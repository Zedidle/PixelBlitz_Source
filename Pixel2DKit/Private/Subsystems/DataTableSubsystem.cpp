// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/DataTableSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"


void UDataTableSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

UDataTable* UDataTableSubsystem::LoadDataTable(const FString& Name) const
{
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	FSoftObjectPath DataTablePath( FString::Printf(TEXT("/Game/Assets/Config/%s.%s"), *Name, *Name));
	UDataTable* DataTable = Cast<UDataTable>(StreamableManager.LoadSynchronous(DataTablePath));

	if (DataTable)
	{
		DataTable->AddToRoot();
		return DataTable;
	}
	UE_LOG(LogTemp, Error, TEXT("Failed to load data table: %s"), *DataTablePath.ToString());
	return nullptr;
}

UDataTable* UDataTableSubsystem::GetDataTable(FString InName)
{
	if(!DataTableMap.IsEmpty())
	{
		if (const auto& DataTablePtr = DataTableMap.Find(InName))
		{
			return *DataTablePtr;
		}
	}
	if(UDataTable* NewDataTable = LoadDataTable(InName))
	{
		
		DataTableMap.Add(InName,NewDataTable );
	}
	else
	{
		return nullptr;
	}
	if(DataTableMap[InName])
	{
		return DataTableMap[InName];
	}
	return nullptr;
}
