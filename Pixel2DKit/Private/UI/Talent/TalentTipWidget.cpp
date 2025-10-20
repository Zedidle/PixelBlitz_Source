// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Talent/TalentTipWidget.h"

void UTalentTipWidget::NativeConstruct()
{
	Super::NativeConstruct();


	UpdateData();
}

void UTalentTipWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UTalentTipWidget::SetTalentData(const FTalent& Data)
{
	TalentData = Data;
}

void UTalentTipWidget::UpdateData()
{
	if (TextBlock_Desc)
	{
		TextBlock_Desc->SetText(TalentData.TalentDesc);
	}
}
