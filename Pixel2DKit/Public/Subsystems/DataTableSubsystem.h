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

	TObjectPtr<UDataTable> GetData(const FString& Path);
	
	TMap<FName, TObjectPtr<UDataTable>> DataTables; // ✅ 安全存储
	FString ItemDataPath = TEXT("/Game/Pixel2D/Blueprints/Items/DT_ItemData.DT_ItemData");

	
	

public:
	TMap<FName, TObjectPtr<UDataTable>> LocalizedDataTables; // ✅ 安全存储
	UDataTable* GetLocalizedDataTable(const FString& Path);

	
	TObjectPtr<UDataTable> GetItemData() { return GetData(ItemDataPath); }
};
