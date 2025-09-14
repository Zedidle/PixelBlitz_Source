// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/PXCommonText.h"

TAttribute<FText> UPXCommonText::GetDisplayText()
{
	FString S = ULocalizationFuncLib::GetLocalizedString(LocalizedTableData);
	if (!S.IsEmpty())
	{
		return FText::FromString(S);
	}
	return PROPERTY_BINDING(FText, Text);
}

void UPXCommonText::PostLoad()
{
	Super::PostLoad();

	// 读取配置好的字体
	
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
