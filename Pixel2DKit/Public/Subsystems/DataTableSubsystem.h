// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DataTableSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UDataTableSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UDataTable* LoadDataTable(const FString& Name) const;
	
	
	
	UFUNCTION(BlueprintCallable)
	UDataTable* GetDataTable(FString InName);
	
	template <class T>
	T FindRow(UDataTable* DataTable, FName RowName, const TCHAR* ContextString = TEXT(""), bool bWarnIfRowMissing = false) const;	
	template <class T>
	TArray<T> GetRowMap(UDataTable* DataTable) const;

protected:
	UPROPERTY()
	TMap<FString, UDataTable*> DataTableMap;
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