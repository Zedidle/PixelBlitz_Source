// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DataTableSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "DataTable Settings"))
class PIXEL2DKIT_API UDataTableSettings : public UDeveloperSettings
{
	GENERATED_BODY()


	TObjectPtr<UDataTable> GetData(const FString& Path) const;
	mutable TMap<FName, TObjectPtr<UDataTable>> DataTables;

	UPROPERTY(Config, EditAnywhere, Category = "ItemData")
	FString ItemDataPath = TEXT("/Game/Pixel2D/Blueprints/Items/DT_ItemData.DT_ItemData");

	UPROPERTY(Config, EditAnywhere, Category = "ItemData")
	TSoftObjectPtr<UDataTable> ItemDataPath2;

public:
	static const UDataTableSettings& Get()
	{
		return *GetDefault<UDataTableSettings>();
	}


	
	mutable TMap<FName, TObjectPtr<UDataTable>> LocalizedDataTables;
	UDataTable* GetLocalizedDataTable(const FString& Path) const;


	TObjectPtr<UDataTable> GetItemData() const { return GetData(ItemDataPath2.ToString()); }
	

	


	
};
