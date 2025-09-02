// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Achievement/AchievementCompleteWidget.h"

void UAchievementCompleteWidget::SetData(const FAchievement& AchievementData)
{
	FString ShowText = ULocalizationFuncLib::GetLocalizedString(FLocalizedTableData("Achieve/Achievement", "finishtext")) + ": ";
	ShowText += ULocalizationFuncLib::GetLocalizedString(AchievementData.AchievementName);

	Name = FText::FromString(ShowText);
}
