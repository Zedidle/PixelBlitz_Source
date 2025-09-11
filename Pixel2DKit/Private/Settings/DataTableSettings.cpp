// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/DataTableSettings.h"

#include "Item/Weapon/BaseWeapon.h"
#include "Pixel2DKit/Pixel2DKit.h"

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

void UDataTableSettings::PostInitProperties()
{
	Super::PostInitProperties();

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

void UDataTableSettings::LoadWeaponData() const
{
	UDataTable* DataTable = GetWeaponData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTable)
	
	TArray<FName> Rownames = DataTable->GetRowNames();
	for (auto& Row : Rownames)
	{
		FWeaponData* Data = DataTable->FindRow<FWeaponData>(Row, "UDataTableSettings::LoadWeaponData");
		WeaponData.Add(Data->WeaponTag, *Data);
	}
}

const FWeaponData* UDataTableSettings::GetWeaponDataByTag(FGameplayTag WeaponTag) const
{
	if (!WeaponData.Contains(WeaponTag))
	{
		LoadWeaponData();
	}
	
	if (WeaponData.Contains(WeaponTag))
	{
		return &WeaponData[WeaponTag];
	}
	return nullptr;
}
