// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Talent/TalentTipWidget.h"

void UTalentTipWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (TextBlock_Desc)
	{
		TextBlock_Desc->SetText(TalentData.TalentDesc);
	}
}

void UTalentTipWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UTalentTipWidget::SetTalentData(FTalent& Data)
{
	TalentData = Data;
}
