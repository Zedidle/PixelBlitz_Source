// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/LocalizationFuncLib.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Settings/DataTableSettings.h"
#include "UI/Foundation/PXTextBlock.h"

FString ULocalizationFuncLib::GetLocalizedString(const FLocalizedTableData& Data)
{
	if (Data.TableName.IsEmpty() || Data.RowName.IsNone())
	{
		return TEXT("");
	}
	
	
	UWorld* world = GEngine->GetCurrentPlayWorld();
	if (!IsValid(world)) return "";


	const UDataTableSettings& Settings = UDataTableSettings::Get();
	
	// 尝试读取表
	TObjectPtr<UDataTable> ItemDataTable = Settings.GetLocalizedDataTable(Data);
	if (!IsValid(ItemDataTable)) return "";

	FName RowName = Data.RowName;
	FLocalizedStringData* LocalizedString = ItemDataTable->FindRow<FLocalizedStringData>(RowName, TEXT("ULocalizationFuncLibGetLocalizedString"));
	if (!LocalizedString) return "";
	
	UGameInstance* gameInstance = world->GetGameInstance();
	if (!gameInstance) return "";
	
	UPXSaveGameSubsystem* SaveGameSubsystem = gameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	if (!SaveGameSubsystem) return "";

	UPXSettingSaveGame* SettingSaveGame = SaveGameSubsystem->GetSettingData();
	if (!SettingSaveGame) return "";
	
	switch (static_cast<ELanguageEnum>(SettingSaveGame->GeneralSetting_Language))
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
	
	return TEXT("");
}

FString ULocalizationFuncLib::GetLocalizedString(const FString& TableName, const FName& RowName)
{
	return GetLocalizedString(FLocalizedTableData(TableName, RowName));
}

FString ULocalizationFuncLib::GetBuffText(FName BuffName)
{
	const FLocalizedTableData D = FLocalizedTableData("Buff/BuffText", BuffName);
	return GetLocalizedString(D);
}

void ULocalizationFuncLib::SetLanguage(UObject* WorldContextContext, ELanguageEnum Language)
{
	if ( !WorldContextContext ) return;
	const UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextContext, EGetWorldErrorMode::LogAndReturnNull);
	if ( !World ) return;
	
	UPXSettingSaveGame* SettingSaveGame = UPXSaveGameSubSystemFuncLib::GetSettingData(WorldContextContext);
	SettingSaveGame->GeneralSetting_Language = Language;
	UPXSaveGameSubSystemFuncLib::SaveSettingData(WorldContextContext);

	for ( TObjectIterator<UPXTextBlock> Itr; Itr; ++Itr )
	{
		UPXTextBlock* LiveWidget = *Itr;
		
		if (LiveWidget->GetWorld() != World) continue;
		LiveWidget->UpdateText();
	}
}
