// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/DataTableSubsystem.h"

#include "GameplayTagsManager.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Item/Weapon/BaseWeapon.h"
#include "Pixel2DKit/Pixel2DKit.h"


UDataTableSubsystem* UDataTableSubsystem::GetInstance(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;
	
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;
	
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance) return nullptr;
	
	return GameInstance->GetSubsystem<UDataTableSubsystem>();
}

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
			FAbility* Data = DataTable->FindRow<FAbility>(Row, "LoadAbilityData");
			if (!AbilityData.Contains(Data->AbilityTag))
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
		FTalent* Data = DataTable->FindRow<FTalent>(Row, "LoadTalentData");
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
	UDataTable* DataTable = GetDataTable("Weapon");
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTable)
	
	TArray<FName> Rownames = DataTable->GetRowNames();
	for (auto& Row : Rownames)
	{
		FWeaponData* Data = DataTable->FindRow<FWeaponData>(Row, "LoadWeaponData");
		WeaponData.Add(Data->WeaponTag, *Data);
	}
}

const FWeaponData* UDataTableSubsystem::GetWeaponDataByTag(const FGameplayTag& WeaponTag)
{
	if (!WeaponData.Contains(WeaponTag))
	{
		LoadWeaponData();
	}
	
	return WeaponData.Find(WeaponTag);
}

void UDataTableSubsystem::LoadLevelData()
{
	TArray<FString> LevelTableNames = {"LevelData_Race", "LevelData_Arena"};

	for (auto& LevelName : LevelTableNames)
	{
		UDataTable* DataTable = GetDataTable(LevelName);
		if (!DataTable) continue;
	
		TArray<FName> RowNames = DataTable->GetRowNames();
		for (auto& Row : RowNames)
		{
			FLevelData* Data = DataTable->FindRow<FLevelData>(Row, "LoadLevelData");
			LevelData.Add(Data->LevelInstanceName, *Data);
		}
	}
}

const FLevelData* UDataTableSubsystem::GetLevelDataByName(const FName& LevelInstanceName)
{
	if (!LevelData.Contains(LevelInstanceName))
	{
		LoadLevelData();
	}

	return LevelData.Find(LevelInstanceName);
}

void UDataTableSubsystem::LoadDropTableData()
{
	UDataTable* DataTable = GetDataTable("DropTable");
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTable)
	
	TArray<FName> Rownames = DataTable->GetRowNames();
	for (auto& Row : Rownames)
	{
		FDrop* Data = DataTable->FindRow<FDrop>(Row, "LoadDropTableData");
		DropTableData.Add(Data->DropID, *Data);
	}
}

const FDrop* UDataTableSubsystem::GetDropDataByName(const FName& DropID)
{
	if (!DropTableData.Contains(DropID))
	{
		LoadDropTableData();
	}

	return DropTableData.Find(DropID);
}

void UDataTableSubsystem::LoadBuffOnWidgetData()
{	
	UDataTable* DataTable = GetDataTable("BuffOnWidget");
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTable)
	
	TArray<FName> Rownames = DataTable->GetRowNames();
	for (auto& Row : Rownames)
	{
		FBuffOnWidget* Data = DataTable->FindRow<FBuffOnWidget>(Row, "LoadBuffOnWidgetData");
		Tag2BuffOnWidgetData.Add(Data->Tag, *Data);
	}
}

const FBuffOnWidget* UDataTableSubsystem::GetBuffOnWidgetDataByTag(const FGameplayTag& Tag)
{
	if (!Tag2BuffOnWidgetData.Contains(Tag))
	{
		LoadBuffOnWidgetData();
	}

	return Tag2BuffOnWidgetData.Find(Tag);
}
