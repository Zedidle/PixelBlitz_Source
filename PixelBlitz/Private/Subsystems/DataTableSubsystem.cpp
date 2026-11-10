// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/DataTableSubsystem.h"

#include "GameplayTagsManager.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Item/Weapon/BaseWeapon.h"
#include "PixelBlitz/PixelBlitz.h"


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

	for (const FString& CandidatePath : GetConfigCandidatePaths(InName))
	{
		if (UDataTable* NewDataTable = LoadObject<UDataTable>(nullptr, *CandidatePath))
		{
			DataTableMap.Add(*InName, NewDataTable);
			return NewDataTable;
		}
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

void UDataTableSubsystem::LoadSummonedData()
{
	UDataTable* DataTable = GetSummonedData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTable)
	
	TArray<FName> Rownames = DataTable->GetRowNames();
	for (auto& Row : Rownames)
	{
		FSummoned* Data = DataTable->FindRow<FSummoned>(Row, "LoadSummonedData");
		SummonedData.Add(Data->SummonedID, *Data);
	}
}

const FSummoned* UDataTableSubsystem::GetSummonedDataByName(const FName& SummonedName)
{
	if (!SummonedData.Contains(SummonedName))
	{
		LoadSummonedData();
	}

	return SummonedData.Find(SummonedName);
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
	// 表名约定：LevelData_{模式}_{系列序号两位}，序号连续，首个缺失即结束该模式
	static const TArray<FString> ModeNames = { TEXT("Race"), TEXT("Arena") };
	constexpr int32 MaxSeriesIndex = 9;

	for (const FString& ModeName : ModeNames)
	{
		for (int32 SeriesIdx = 1; SeriesIdx <= MaxSeriesIndex; ++SeriesIdx)
		{
			const FString TableName = FString::Printf(TEXT("LevelData_%s_%02d"), *ModeName, SeriesIdx);

			// 探测加载：缺表是"序列结束"的正常信号，不走 GetDataTable 的报错路径
			UDataTable* DataTable = nullptr;
			for (const FString& CandidatePath : GetConfigCandidatePaths(TableName))
			{
				DataTable = LoadObject<UDataTable>(nullptr, *CandidatePath);
				if (DataTable) break;
			}
			if (!DataTable)
			{
				break;
			}
			DataTableMap.Add(FName(*TableName), DataTable);

			const TArray<FName> RowNames = DataTable->GetRowNames();
			for (const FName& Row : RowNames)
			{
				const FLevelData* Data = DataTable->FindRow<FLevelData>(Row, TEXT("LoadLevelData"));
				if (!Data)
				{
					UE_LOG(LogTemp, Error, TEXT("[LoadLevelData] 行解析失败: %s.%s"), *TableName, *Row.ToString());
					continue;
				}

				if (LevelData.Contains(Data->LevelInstanceName))
				{
					UE_LOG(LogTemp, Warning, TEXT("[LoadLevelData] LevelInstanceName 冲突，后者覆盖: %s (来自 %s)"),
						*Data->LevelInstanceName.ToString(), *TableName);
				}
				LevelData.Add(Data->LevelInstanceName, *Data);
			}
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

void UDataTableSubsystem::LoadLevelSeriesData()
{
	for (auto LevelSeriesDataPath : LevelSeriesDataPaths)
	{
		UDataTable* DataTable = GetDataTable(LevelSeriesDataPath);
		if (!IsValid(DataTable)) continue;
		
		TArray<FName> Rownames = DataTable->GetRowNames();
		for (auto& Row : Rownames)
		{
			FLevelSeriesData* Data = DataTable->FindRow<FLevelSeriesData>(Row, "LoadLevelSeriesData");
			LevelSeriesData.Add(Data->LevelSeriesID, *Data);
		}
	}
}

FLevelSeriesData UDataTableSubsystem::GetLevelSeriesData(const FName& LevelSeriesID)
{
	if (!LevelSeriesData.Contains(LevelSeriesID))
	{
		LoadLevelData();
	}

	return LevelSeriesData.FindRef(LevelSeriesID);
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

void UDataTableSubsystem::LoadNPCData()
{
	UDataTable* DataTable = GetDataTable("NPCData");
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTable)
	
	TArray<FName> Rownames = DataTable->GetRowNames();
	for (auto& Row : Rownames)
	{
		FNPCData* Data = DataTable->FindRow<FNPCData>(Row, "LoadNPCData");
		Tag2NPCData.Add(Data->DialogueParticipantTag, *Data);
	}
}

FNPCData UDataTableSubsystem::GetNPCDataByTag(const FGameplayTag& Tag)
{
	if (!Tag2NPCData.Contains(Tag))
	{
		LoadNPCData();
	}

	return Tag2NPCData.FindRef(Tag);
}
