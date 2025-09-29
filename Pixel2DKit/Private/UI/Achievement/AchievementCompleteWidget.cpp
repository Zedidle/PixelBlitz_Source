// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Achievement/AchievementCompleteWidget.h"

void UAchievementCompleteWidget::SetData(const FAchievement& AchievementData)
{

	Name = AchievementData.AchievementName;
}
