// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/AchievementSubsystem.h"

#include "Core/PXSaveGameSubsystem.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/DataTableSettings.h"
#include "Settings/UserWidgetSettings.h"
#include "UI/Achievement/AchievementCompleteWidget.h"

void UAchievementSubsystem::CompleteAchievement(FName AchievementRowName)
{
	const UDataTableSettings* Settings = GetDefault<UDataTableSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings);
	UDataTable* DataTable = Settings->GetAchievementData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTable);

	FAchievement* AchievementData = DataTable->FindRow<FAchievement>(AchievementRowName, TEXT("UAchievementSubsystem::CompleteAchievement"));
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AchievementData);
	UPXSaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem);
	
	if (SaveGameSubsystem->CompleteAchievement(AchievementRowName))
	{
		if (const UUserWidgetSettings* UserWidgetSettings = GetDefault<UUserWidgetSettings>())
		{
			if (UserWidgetSettings->AchievementCompleteWidgetClass)
			{
				if (UAchievementCompleteWidget* Widget = CreateWidget<UAchievementCompleteWidget>(GetWorld(),
					UserWidgetSettings->AchievementCompleteWidgetClass))
				{
					Widget->SetData(*AchievementData);
					Widget->AddToViewport(1000);
				}
			}
		}
	}
	
}
