// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary/LocalizationFuncLib.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"
#include "Basic/BasePixelGameInstance.h"
#include "Settings/DataTableSettings.h"

FString ULocalizationFuncLib::GetLocalizedString(const FLocationTableData& Data)
{
	UWorld* world = GEngine->GetCurrentPlayWorld();
	if (!IsValid(world)) return "";
	
	FString tDir;
	FString tName;

	const UDataTableSettings* Settings = GetDefault<UDataTableSettings>();
	
	// 尝试读取表
	TObjectPtr<UDataTable> ItemDataTable;
	if (Data.TableName.Split("/", &tDir, &tName, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
	{
		if (tName == "") return "";
		ItemDataTable = Settings->GetLocalizedDataTable(
			FString::Printf(TEXT("/Game/LocalizationData/%s/DT_ML_%s.DT_ML_%s"), *tDir, *tName, *tName));
		if (!IsValid(ItemDataTable))
		{
			ItemDataTable = Settings->GetLocalizedDataTable(
				FString::Printf(TEXT("/Game/LocalizationData/%s/%s.%s"), *tDir, *tName, *tName));
		}
	}
	else
	{
		tName = Data.TableName;
		ItemDataTable = Settings->GetLocalizedDataTable(
			FString::Printf(TEXT("/Game/LocalizationData/DT_ML_%s.DT_ML_%s"),  *tName, *tName));
		if (!IsValid(ItemDataTable))
		{
			ItemDataTable = Settings->GetLocalizedDataTable(
			FString::Printf(TEXT("/Game/LocalizationData/%s.%s"), *tName, *tName));	
		}
	}
	
	if (!IsValid(ItemDataTable))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow,
		FString::Printf(TEXT("ULocalizationFuncLib::GetLocalizedString  %s not found, %d"), *Data.TableName,__LINE__));
		return "";
	}

	FName RowName = Data.RowName;
	FLocalizedStringData* LocalizedString = ItemDataTable->FindRow<FLocalizedStringData>(RowName, TEXT("ULocalizationFuncLibGetLocalizedString"));
	if (!LocalizedString) return "";
	
	UGameInstance* gameInstance = world->GetGameInstance();
	if (!gameInstance) return "";
	
	UBasePixelGameInstance* GI = Cast<UBasePixelGameInstance>(gameInstance);
	if (!GI) return "";
	
	switch (static_cast<ELanguageEnum>(GI->GeneralSetting_Language))
	{
		case ELanguageEnum::Chinese: return LocalizedString->Chinese;
		case ELanguageEnum::English: return LocalizedString->English;
		case ELanguageEnum::OldChinese: return LocalizedString->OldChinese;
		case ELanguageEnum::Japanese: return LocalizedString->Japanese;
		case ELanguageEnum::Korean: return LocalizedString->Korean;
		case ELanguageEnum::Russian: return LocalizedString->Russian;
		case ELanguageEnum::Spanish: return LocalizedString->Spanish;
		case ELanguageEnum::Portuguese: return LocalizedString->Portuguese;
		case ELanguageEnum::German: return LocalizedString->German;
		case ELanguageEnum::French: return LocalizedString->French;
	}
	
	return "";
}
