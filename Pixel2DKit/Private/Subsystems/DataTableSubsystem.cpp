// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/DataTableSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Item/Weapon/BaseWeapon.h"
#include "Pixel2DKit/Pixel2DKit.h"


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

	// 直接路径搜索
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

void UDataTableSubsystem::LoadAbilityData()
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

const FAbility UDataTableSubsystem::GetAbilityDataByTag(const FGameplayTag& AbilityTag)
{
	if (!AbilityData.Contains(AbilityTag))
	{
		LoadAbilityData();
	}

	return AbilityData.FindRef(AbilityTag);
}

const TMap<FGameplayTag, FAbility>& UDataTableSubsystem::GetAllAbilities()
{
	LoadAbilityData();
	return AbilityData;
}

void UDataTableSubsystem::LoadTalentData()
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

FTalent UDataTableSubsystem::GetTalentDataByTag(const FGameplayTag& TalentTag)
{
	if (!TalentData.Contains(TalentTag))
	{
		LoadTalentData();
	}

	return TalentData.FindRef(TalentTag);
}

void UDataTableSubsystem::LoadWeaponData()
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

const FWeaponData* UDataTableSubsystem::GetWeaponDataByTag(const FGameplayTag& WeaponTag)
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
