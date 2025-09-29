// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/AchievementSubsystem.h"
#include "Character/BasePXCharacter.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXWidgetsDataAsset.h"
#include "Subsystems/DataTableSubsystem.h"
#include "UI/Achievement/AchievementCompleteWidget.h"
#include "Utilitys/PXGameplayStatics.h"

void UAchievementSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
}

void UAchievementSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UAchievementSubsystem::LoadAchievementData()
{
	UGameInstance* GameInstance = GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem)
	
	UDataTable* DataTable = DataTableSubsystem->GetAchievementData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTable);
	
	TArray<FAchievement> Achievements = DataTableSubsystem->GetRowMap<FAchievement>(DataTable);
	for (auto& achievement : Achievements)
	{
		AchievementsMap.Add(achievement.AchievementKey, achievement);
	}
}

void UAchievementSubsystem::CompleteAchievement(FName AchievementKey)
{
	UGameInstance* GameInstance = GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)
	
	UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem);

	if (!AchievementsMap.Contains(AchievementKey))
	{
		LoadAchievementData();
	}

	const FAchievement& AchievementData = AchievementsMap.FindRef(AchievementKey);
	check(AchievementData.AchievementKey.IsValid());
	if (SaveGameSubsystem->CompleteAchievement(AchievementKey))
	{
		const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

		const UPXWidgetsDataAsset* WidgetsDataAsset = Settings->WidgetsDataAsset.LoadSynchronous();
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(WidgetsDataAsset)
		
		if (WidgetsDataAsset->AchievementCompleteWidgetClass)
		{
			if (UAchievementCompleteWidget* Widget = CreateWidget<UAchievementCompleteWidget>(GetWorld(),
				WidgetsDataAsset->AchievementCompleteWidgetClass))
			{
				Widget->SetData(AchievementData);
				Widget->AddToViewport(1000);
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

