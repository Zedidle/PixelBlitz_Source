// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/PXCommonText.h"

TAttribute<FText> UPXCommonText::GetDisplayText()
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


void UPXCommonText::UpdateText()
{
	FString S = ULocalizationFuncLib::GetLocalizedString(LocalizedTableData);
	if (!S.IsEmpty())
	{
		SetText(FText::FromString(S));
	}
}

void UPXCommonText::SetLocalizedTableData(const FLocalizedTableData& _LocalizedTableData)
{
	LocalizedTableData = _LocalizedTableData;
	UpdateText();
}
