// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PXCustomStruct.h"
#include "Fight/Skills/Common/CommonSummonSkill.h"
#include "DataTableSubsystem.generated.h"

struct FWeaponData;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class PIXELBLITZ_API UDataTableSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	mutable TMap<FString, TObjectPtr<UDataTable>> LocalizedDataTables;
	
	UFUNCTION()
	TArray<FString> GetConfigCandidatePaths(FString InName)
	{
		return {
			FString::Printf(TEXT("/Game/Assets/Config/%s.%s"), *InName, *InName),
			FString::Printf(TEXT("/Game/Assets/Config/Levels/%s.%s"), *InName, *InName)
		};
	}
	
public:
	
	static UDataTableSubsystem* GetInstance(const UObject* WorldContextObject);
	
	
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 通过目录配置
	UFUNCTION(BlueprintCallable)
	UDataTable* GetDataTable(FString InName);


	template <class T>
	T FindRow(UDataTable* DataTable, FName RowName, const TCHAR* ContextString = TEXT(""), bool bWarnIfRowMissing = false) const;	
	template <class T>
	TArray<T> GetRowMap(UDataTable* DataTable) const;

protected:
	UPROPERTY()
	TMap<FName, TObjectPtr<UDataTable>> DataTableMap;

	// 已注册过行数据的关卡表路径（LoadLevelData 幂等去重用）
	TSet<FString> RegisteredLevelTables;

	// 将一张 FLevelData 行结构表注册进 LevelData 反查缓存（按 LevelInstanceNames 逐实例注册）
	void RegisterLevelDataTable(UDataTable* DataTable);


public:
	
	TObjectPtr<UDataTable> GetUIData(){ return GetDataTable("UIConfig"); }
	TObjectPtr<UDataTable> GetCharacterData() { return GetDataTable("CharacterData"); }
	TObjectPtr<UDataTable> GetItemData() { return GetDataTable( "ItemData"); }
	TObjectPtr<UDataTable> GetAchievementData() { return GetDataTable("Achievement"); }
	TObjectPtr<UDataTable> GetLevelWeatherRateData()  { return GetDataTable("LevelWeatherRate"); }
	TObjectPtr<UDataTable> GetWeatherData()  { return GetDataTable("Weather"); }

	
	mutable TMap<FGameplayTag, FAbility> AbilityData;
	void LoadAbilityData();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FAbility GetAbilityDataByTag(const FGameplayTag& AbilityTag);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TMap<FGameplayTag, FAbility>& GetAllAbilities();
	TArray<FString> AbilityDataPaths = {"Ability_Common", "Ability_Saber", "Ability_Archer"};
	TArray<TObjectPtr<UDataTable>> GetAbilityData()
	{
		TArray<TObjectPtr<UDataTable>> Result;
		for (const auto& Path : AbilityDataPaths)
		{
			Result.Add(GetDataTable(Path));
		}
		return Result;
	}

	
	mutable TMap<FGameplayTag, FTalent> TalentData;
	TObjectPtr<UDataTable> GetTalentData() { return GetDataTable("Talent"); }
	void LoadTalentData();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FTalent GetTalentDataByTag(const FGameplayTag& TalentTag);
	

	mutable TMap<FName, FSummoned> SummonedData ;
	TObjectPtr<UDataTable> GetSummonedData() { return GetDataTable("Summoned"); }
	void LoadSummonedData() ;
	const FSummoned* GetSummonedDataByName(const FName& SummonedName) ;

	
	mutable TMap<FGameplayTag, FWeaponData> WeaponData;
	void LoadWeaponData() ;
	const FWeaponData* GetWeaponDataByTag(const FGameplayTag& WeaponTag) ;

	
	// 获取关卡数据
	mutable TMap<FName, FLevelData> LevelData;
	void LoadLevelData();
	const FLevelData* GetLevelDataByName(const FName& LevelInstanceName) ;

	// 行名 → 行数据（行名 = 关卡序号推进键，如 L1_Race；一行内 LevelInstanceNames 为同难度候选池）
	// 候选池允许跨行重叠（如 L2_HomingMoon 同属 L1/L2 行），故实例名不能作推进键
	mutable TMap<FName, FLevelData> LevelDataByRowName;
	// 统一入口：先按行名查（推进键），查不到再按实例名反查（旧档/测试直填实例名的兼容路径）
	const FLevelData* GetLevelDataByKey(const FName& Key);
	
	
	// 获取关卡数据
	mutable TMap<FName, FLevelSeriesData> LevelSeriesData;
	void LoadLevelSeriesData();
	TArray<FString> LevelSeriesDataPaths = {"LevelSeriesData_Arena", "LevelSeriesData_Race"};
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="LevelSeriesData")
	FLevelSeriesData GetLevelSeriesData(const FName& LevelSeriesID) ;
	
	
	
	mutable TMap<FName, FDrop> DropTableData;
	void LoadDropTableData();
	const FDrop* GetDropDataByName(const FName& DropID) ;
	

	mutable TMap<FGameplayTag, FBuffOnWidget> Tag2BuffOnWidgetData;
	void LoadBuffOnWidgetData();
	const FBuffOnWidget* GetBuffOnWidgetDataByTag(const FGameplayTag& Tag) ;
	UFUNCTION(BlueprintPure, Category="BuffOnWidget")
	FLinearColor GetBuffOnWidgetColorByTag(const FGameplayTag& Tag) const;
	
	
	mutable TMap<FGameplayTag, FNPCData> Tag2NPCData;
	void LoadNPCData();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="NPCData", meta=(Categories="Mountea_Dialogue"))
	FNPCData GetNPCDataByTag(const FGameplayTag& Tag);

};


template <class T>
T UDataTableSubsystem::FindRow(UDataTable* DataTable, FName RowName, const TCHAR* ContextString,
	bool bWarnIfRowMissing) const
{
	void* RowPtr = DataTable->FindRowUnchecked(RowName);
	if(RowPtr)
	{
		const UScriptStruct* RowStruct = DataTable->GetRowStruct();
		if(RowStruct)
		{
			T Info;
			if(RowStruct)
			{
				RowStruct->CopyScriptStruct(&Info, RowPtr);
			}
			return  Info;
		}
	}
	return T();
}

template <class T>
TArray<T> UDataTableSubsystem::GetRowMap(UDataTable* DataTable) const
{
	TMap<FName, uint8* > rows = DataTable->GetRowMap();
	TArray<T> Result;
	for (auto it : rows)
	{
		FString RowName = (it.Key).ToString();
 
		const UScriptStruct* RowStruct = DataTable->GetRowStruct();
		T Info;
		if(RowStruct)
		{
			RowStruct->CopyScriptStruct(&Info, it.Value);
		}
		Result.Add(Info);
	}
	return Result;
}