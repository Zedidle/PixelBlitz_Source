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

void UAchievementSubsystem::Achievement_KillBoss(int InjuredNum)
{
	UPXSaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<UPXSaveGameSubsystem>();
	UAchievementSubsystem* AchievementSubsystem = GetGameInstance()->GetSubsystem<UAchievementSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AchievementSubsystem)

	UPXAchievementsSaveGame* AchievementsData = SaveGameSubsystem->GetAchievementsData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AchievementsData);
	
	AchievementsData->KilledBossNum ++;

	// 第一次击杀BOSS
	if (AchievementsData->KilledBossNum == 1)
	{
		AchievementSubsystem->CompleteAchievement("2");
	}
	// 击杀BOSS 100次
	if (AchievementsData->KilledBossNum == 100)
	{
		AchievementSubsystem->CompleteAchievement("14");
	}
	// 一击必杀
	if (InjuredNum == 0)
	{
		AchievementSubsystem->CompleteAchievement("3");
	}

	SaveGameSubsystem->SaveAchievementsData();
}

void UAchievementSubsystem::Achievement_Completion(float UseTime, bool bClearAllEnemy, bool bPickupAllGolds,
	int DieTimes, int HurtTimes)
{
	UAchievementSubsystem* AchievementSubsystem = GetGameInstance()->GetSubsystem<UAchievementSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AchievementSubsystem)

	AchievementSubsystem->CompleteAchievement("1");

	if (UseTime < 300)
	{
		AchievementSubsystem->CompleteAchievement("5");
	}

	if (UseTime < 360 && bClearAllEnemy)
	{
		AchievementSubsystem->CompleteAchievement("7");
	}

	if (UseTime < 480)
	{
		AchievementSubsystem->CompleteAchievement("4");
	}

	// 击杀所有怪物
	if (bClearAllEnemy)
	{
		AchievementSubsystem->CompleteAchievement("6");
	}

	// 拾取所有金币
	if (bPickupAllGolds)
	{
		AchievementSubsystem->CompleteAchievement("15");
	}

	// 一次不死
	if (DieTimes == 0)
	{
		AchievementSubsystem->CompleteAchievement("12");
	}

	// 无伤通过
	if (HurtTimes == 0)
	{
		AchievementSubsystem->CompleteAchievement("13");
	}

	// 全能的人
	if (bClearAllEnemy && bPickupAllGolds && HurtTimes == 0)
	{
		AchievementSubsystem->CompleteAchievement("20");
	}
}

void UAchievementSubsystem::Achievement_LevelPass(int PerfectDodgeTimes, int AttackHitInDashTimes)
{
	UAchievementSubsystem* AchievementSubsystem = GetGameInstance()->GetSubsystem<UAchievementSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AchievementSubsystem)

	if (PerfectDodgeTimes >= 20)
	{
		AchievementSubsystem->CompleteAchievement("8");
	}

	if (AttackHitInDashTimes >= 20)
	{
		AchievementSubsystem->CompleteAchievement("9");

	}
}

