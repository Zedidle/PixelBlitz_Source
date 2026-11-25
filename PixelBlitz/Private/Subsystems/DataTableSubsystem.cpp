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

void UDataTableSubsystem::RegisterLevelDataTable(UDataTable* DataTable)
{
	if (!IsValid(DataTable))
	{
		return;
	}

	// 幂等去重：系列驱动与旧约定探测可能指向同一张表
	const FString TablePath = DataTable->GetPathName();
	if (RegisteredLevelTables.Contains(TablePath))
	{
		return;
	}
	RegisteredLevelTables.Add(TablePath);

	const TArray<FName> RowNames = DataTable->GetRowNames();
	for (const FName& Row : RowNames)
	{
		const FLevelData* Data = DataTable->FindRow<FLevelData>(Row, TEXT("RegisterLevelDataTable"));
		if (!Data)
		{
			UE_LOG(LogTemp, Error, TEXT("[LoadLevelData] 行解析失败: %s.%s"), *DataTable->GetName(), *Row.ToString());
			continue;
		}

		if (Data->LevelInstanceNames.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("[LoadLevelData] 行无有效 LevelInstanceNames，跳过: %s.%s"), *DataTable->GetName(), *Row.ToString());
			continue;
		}

		// 行名注册：行名 = 关卡序号推进键（如 L1_Race），同系列表内唯一
		if (LevelDataByRowName.Contains(Row))
		{
			UE_LOG(LogTemp, Warning, TEXT("[LoadLevelData] 行名冲突，后者覆盖: %s (来自 %s)"),
				*Row.ToString(), *DataTable->GetName());
		}
		LevelDataByRowName.Add(Row, *Data);

		// 实例名注册：候选池允许跨行重叠（后者覆盖），仅供任意实例名反查关卡数据
		for (const FName& InstanceName : Data->LevelInstanceNames)
		{
			if (InstanceName.IsNone())
			{
				continue;
			}

			if (LevelData.Contains(InstanceName))
			{
				UE_LOG(LogTemp, Warning, TEXT("[LoadLevelData] LevelInstanceName 冲突，后者覆盖: %s (来自 %s)"),
					*InstanceName.ToString(), *DataTable->GetName());
			}
			LevelData.Add(InstanceName, *Data);
		}
	}
}

void UDataTableSubsystem::LoadLevelData()
{
	// 中介层：由 LevelSeriesData 驱动关卡表加载。
	// 每条系列行（FLevelSeriesData）的 LevelTableName 指向该系列关卡表，
	// 表名不受 LevelData_{模式}_{序号} 约定限制（如 LevelData_Race_Forest）。
	LoadLevelSeriesData();
	for (const auto& Pair : LevelSeriesData)
	{
		const FLevelSeriesData& SeriesData = Pair.Value;
		if (SeriesData.LevelTableName.IsEmpty())
		{
			continue;
		}
		UDataTable* DataTable = GetDataTable(SeriesData.LevelTableName);
		if (!IsValid(DataTable))
		{
			UE_LOG(LogTemp, Warning, TEXT("[LoadLevelData] 系列 %s 的关卡表加载失败: %s"),
				*Pair.Key.ToString(), *SeriesData.LevelTableName);
			continue;
		}
		RegisterLevelDataTable(DataTable);
	}

	// 旧命名约定兜底：LevelData_{模式}_{系列序号两位}，序号连续，首个缺失即结束该模式
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

			RegisterLevelDataTable(DataTable);
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

const FLevelData* UDataTableSubsystem::GetLevelDataByKey(const FName& Key)
{
	if (!LevelDataByRowName.Contains(Key) && !LevelData.Contains(Key))
	{
		LoadLevelData();
	}

	// 行名优先（推进键语义），实例名兜底（旧档/测试路径）
	if (const FLevelData* RowData = LevelDataByRowName.Find(Key))
	{
		return RowData;
	}
	return LevelData.Find(Key);
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
		LoadLevelSeriesData();
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

FLinearColor UDataTableSubsystem::GetBuffOnWidgetColorByTag(const FGameplayTag& Tag) const
{
	if (const FBuffOnWidget* Data = const_cast<UDataTableSubsystem*>(this)->GetBuffOnWidgetDataByTag(Tag))
	{
		return Data->Color;
	}
	return FLinearColor::White;
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
