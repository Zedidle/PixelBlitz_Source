// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/DataTableSettings.h"

#include "Item/Weapon/BaseWeapon.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Utilitys/PXCustomStruct.h"

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

void UDataTableSettings::LoadAbilityData() const
{
	TArray<TObjectPtr<UDataTable>> Tables = GetAbilityData();

	for (auto& DataTable : Tables)
	{
		if (!IsValid(DataTable)) continue;
		TArray<FName> Rownames = DataTable->GetRowNames();
		for (auto& Row : Rownames)
		{
			FAbility* Data = DataTable->FindRow<FAbility>(Row, "UDataTableSettings::LoadAbilityData");
			if (AbilityData.Contains(Data->AbilityTag))
			{
				UE_LOG(LogTemp, Warning, TEXT("Loading Ability Data, 技能Tag重复：%s"), *Data->AbilityTag.ToString());
			}
			else
			{
				AbilityData.Add(Data->AbilityTag, *Data);
			}
		}
	}
}

const FAbility* UDataTableSettings::GetAbilityDataByTag(const FGameplayTag& AbilityTag) const
{
	if (!AbilityData.Contains(AbilityTag))
	{
		LoadAbilityData();
	}
	
	if (AbilityData.Contains(AbilityTag))
	{
		return &AbilityData[AbilityTag];
	}
	return nullptr;
}

const TMap<FGameplayTag, FAbility>& UDataTableSettings::GetAllAbilities() const
{
	LoadAbilityData();
	return AbilityData;
}

void UDataTableSettings::LoadTalentData() const
{
	UDataTable* DataTable = GetTalentData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTable)
	
	TArray<FName> Rownames = DataTable->GetRowNames();
	for (auto& Row : Rownames)
	{
		FTalent* Data = DataTable->FindRow<FTalent>(Row, "UDataTableSettings::LoadTalentData");
		TalentData.Add(Data->TalentTag, *Data);
	}
}

const FTalent* UDataTableSettings::GetTalentDataByTag(const FGameplayTag& TalentTag) const
{
	if (!TalentData.Contains(TalentTag))
	{
		LoadTalentData();
	}
	
	if (TalentData.Contains(TalentTag))
	{
		return &TalentData[TalentTag];
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

const FWeaponData* UDataTableSettings::GetWeaponDataByTag(const FGameplayTag& WeaponTag) const
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
