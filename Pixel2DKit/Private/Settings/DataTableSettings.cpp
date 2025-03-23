// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/DataTableSettings.h"

TObjectPtr<UDataTable> UDataTableSettings::GetData(const FString& Path) const
{
	if (DataTables.Contains(*Path))
	{
		return DataTables[*Path];
	}
	
	if (const TObjectPtr<UDataTable> ItemDataTable = LoadObject<UDataTable>(nullptr, *Path))
	{
		DataTables.Add(FName(*Path), ItemDataTable);
		ItemDataTable->AddToRoot();
		return DataTables[*Path];
	}
	
	return nullptr;
}

UDataTable* UDataTableSettings::GetLocalizedDataTable(const FString& Path) const
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
