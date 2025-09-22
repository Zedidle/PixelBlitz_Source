// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/PXTextBlock.h"

TAttribute<FText> UPXTextBlock::GetDisplayText()
{
	if (LocalizedTableData.TableName.IsEmpty() || LocalizedTableData.RowName.IsNone()) return PROPERTY_BINDING(FText, Text);
	
	// FString S = ULocalizationFuncLib::GetLocalizedString(LocalizedTableData);
	FString S = LocalizedTableData(LocalizedTableData);
	
	if (!S.IsEmpty())
	{
		return FText::FromString(S);
	}
	return PROPERTY_BINDING(FText, Text);
}


void UPXTextBlock::UpdateText()
{
	FString S = ULocalizationFuncLib::GetLocalizedString(LocalizedTableData);
	if (!S.IsEmpty())
	{
		SetText(FText::FromString(S));
	}
}

void UPXTextBlock::SetLocalizedTableData(const FLocalizedTableData& _LocalizedTableData)
{
	LocalizedTableData = _LocalizedTableData;
	UpdateText();
}
