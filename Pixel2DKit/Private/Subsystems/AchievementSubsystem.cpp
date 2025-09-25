// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/AchievementSubsystem.h"

#include "Character/BasePXCharacter.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/Config/DataTableSettings.h"
#include "Settings/Config/UserWidgetSettings.h"
#include "UI/Achievement/AchievementCompleteWidget.h"
#include "Utilitys/PXGameplayStatics.h"

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
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem)


	UPXAchievementsSaveGame* AchievementsData = SaveGameSubsystem->GetAchievementsData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AchievementsData);
	
	AchievementsData->KilledBossNum ++;

	// 第一次击杀BOSS
	if (AchievementsData->KilledBossNum == 1)
	{
		CompleteAchievement("2");
	}
	// 击杀BOSS 100次
	if (AchievementsData->KilledBossNum == 100)
	{
		CompleteAchievement("14");
	}
	// 一击必杀
	if (InjuredNum == 0)
	{
		CompleteAchievement("3");
	}

	SaveGameSubsystem->SaveAchievementsData();
}

void UAchievementSubsystem::Achievement_Completion(float UseTime, bool bClearAllEnemy, bool bPickupAllGolds,
	int DieTimes, int HurtTimes)
{
	CompleteAchievement("1");

	if (UseTime < 300)
	{
		CompleteAchievement("5");
	}

	if (UseTime < 360 && bClearAllEnemy)
	{
		CompleteAchievement("7");
	}

	if (UseTime < 480)
	{
		CompleteAchievement("4");
	}

	// 击杀所有怪物
	if (bClearAllEnemy)
	{
		CompleteAchievement("6");
	}

	// 拾取所有金币
	if (bPickupAllGolds)
	{
		CompleteAchievement("15");
	}

	// 一次不死
	if (DieTimes == 0)
	{
		CompleteAchievement("12");
	}

	// 无伤通过
	if (HurtTimes == 0)
	{
		CompleteAchievement("13");
	}

	// 全能的人
	if (bClearAllEnemy && bPickupAllGolds && HurtTimes == 0)
	{
		CompleteAchievement("20");
	}
}

void UAchievementSubsystem::Achievement_LevelPass()
{
	ABasePXCharacter* PxCharacter = UPXGameplayStatics::GetPlayerCharacter(GetGameInstance()->GetWorld(), 0);

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PxCharacter)
	
	if (PxCharacter->PerfectDodgeTimes >= 20)
	{
		CompleteAchievement("8");
	}

	if (PxCharacter->AttackHitDashing >= 20)
	{
		CompleteAchievement("9");
	}

	if (PxCharacter->HasOneHealthPoint)
	{
		CompleteAchievement("18");
	}
}

