// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/DataTableSubsystem.h"



TObjectPtr<UDataTable> UDataTableSubsystem::GetData(const FString& Path)
{
	if (DataTables.Contains(*Path))
	{
		return DataTables[*Path];
	}
	
	if (const TObjectPtr<UDataTable> ItemDataTable = LoadObject<UDataTable>(nullptr, *Path))
	{
		DataTables.Add(*Path, ItemDataTable);
		ItemDataTable->AddToRoot();
		return DataTables[*Path];
	}
	
	return nullptr;
}




UDataTable* UDataTableSubsystem::GetLocalizedDataTable(const FString& Path)
{
	if (LocalizedDataTables.Contains(*Path))
	{
		return LocalizedDataTables[*Path];
	}
	
	if (const TObjectPtr<UDataTable> ItemDataTable = LoadObject<UDataTable>(nullptr, *Path))
	{
		LocalizedDataTables.Add(*Path, ItemDataTable);
		ItemDataTable->AddToRoot();
		return LocalizedDataTables[*Path];
	}
	
	return nullptr;
}
