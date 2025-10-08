// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Utilitys/PXCustomStruct.h"
#include "DataTableSubsystem.generated.h"

struct FWeaponData;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UDataTableSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	mutable TMap<FString, TObjectPtr<UDataTable>> LocalizedDataTables;
	
	UFUNCTION()
	FString GetConfigPath(FString InName){ return FString::Printf(TEXT("/Game/Assets/Config/%s.%s"), *InName, *InName); };
	
public:
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
	
	
	mutable TMap<FGameplayTag, FWeaponData> WeaponData;
	void LoadWeaponData() ;
	const FWeaponData* GetWeaponDataByTag(const FGameplayTag& WeaponTag) ;

	
	mutable TMap<FName, FLevelData> LevelData;
	void LoadLevelData();
	const FLevelData* GetLevelDataByName(const FName& LevelInstanceName) ;


	mutable TMap<FName, FDrop> DropTableData;
	void LoadDropTableData();
	const FDrop* GetDropDataByName(const FName& DropID) ;
	

	mutable TMap<FGameplayTag, FBuffOnWidget> Tag2BuffOnWidgetData;
	void LoadBuffOnWidgetData();
	const FBuffOnWidget* GetBuffOnWidgetDataByTag(const FGameplayTag& Tag) ;

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