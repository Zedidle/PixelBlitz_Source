// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PXSaveGameSubsystem.h"

#include "InputMappingContext.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "SaveGame/PXSettingSaveGame.h"
#include "SaveGame/PXShopSaveGame.h"
#include "SaveGame/PXTalentsSaveGame.h"
#include "Settings/PXSettingsLocal.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXGameDataAsset.h"
#include "Utilitys/SettingFuncLib.h"


void UPXSaveGameSubsystem::InitData_Main()
{
    if (UPXMainSaveGame* SaveGame = Cast<UPXMainSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Main, UserIndex)))
    {
        MainSaveGame = SaveGame;
    }
    else
    {
        MainSaveGame = NewObject<UPXMainSaveGame>(this);
        SaveMainData();
    }
}

void UPXSaveGameSubsystem::InitData_BasicBuild()
{
    if (UPXBasicBuildSaveGame* SaveGame = Cast<UPXBasicBuildSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_BasicBuild, UserIndex)))
    {
        BasicBuildSaveGame = SaveGame;
    }
    else
    {
        BasicBuildSaveGame = NewObject<UPXBasicBuildSaveGame>(this);
        SaveBasicBuildData();
    }
}


void UPXSaveGameSubsystem::InitData_Achievements()
{
    if (UPXAchievementsSaveGame* SaveGame = Cast<UPXAchievementsSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Achievements, UserIndex)))
    {
        AchievementsSaveGame = SaveGame;
    }
    else
    {
        AchievementsSaveGame = NewObject<UPXAchievementsSaveGame>(this);
        SaveAchievementsData();
    }
}

void UPXSaveGameSubsystem::InitData_Shop()
{
    if (UPXShopSaveGame* SaveGame = Cast<UPXShopSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Shop, UserIndex)))
    {
        ShopSaveGame = SaveGame;
    }
    else
    {
        ShopSaveGame = NewObject<UPXShopSaveGame>(this);
        SaveShopData();
    }
}

void UPXSaveGameSubsystem::InitData_Talents()
{
    if (UPXTalentsSaveGame* SaveGame = Cast<UPXTalentsSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Talents, UserIndex)))
    {
        TalentsSaveGame = SaveGame;
    }
    else
    {
        TalentsSaveGame = NewObject<UPXTalentsSaveGame>(this);
        SaveTalentsData();
    }
}

UPXSaveGameSubsystem* UPXSaveGameSubsystem::GetSelfInstance(const UObject* WorldContextObject)
{
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
    CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GameInstance, nullptr);

    return GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
}

void UPXSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    InitData_Main();
    InitData_BasicBuild();
    InitData_Achievements();
    InitData_Shop();
    InitData_Talents();
}

void UPXSaveGameSubsystem::SaveMainData()
{
    if (UGameplayStatics::SaveGameToSlot(MainSaveGame, SlotName_Main, UserIndex))
    {
        UE_LOG(LogTemp,Log,TEXT("UPXSaveGameSubsystem::SaveMainData Success ^ ^"))
    }
}

UPXMainSaveGame* UPXSaveGameSubsystem::GetMainData()
{
    if (IsValid(MainSaveGame)) return MainSaveGame;

    if (UPXMainSaveGame* SaveGame = Cast<UPXMainSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Main, UserIndex)))
    {
        MainSaveGame = SaveGame;
        return MainSaveGame;
    }

    MainSaveGame = NewObject<UPXMainSaveGame>(this);
    SaveMainData();
    return MainSaveGame;
}

bool UPXSaveGameSubsystem::Main_HasChoiceAbility()
{
    return MainSaveGame->CurLevelToChoiceAbility ==  MainSaveGame->CurLevelName;
}

void UPXSaveGameSubsystem::Main_TotalInit()
{
    UPXMainSaveGame* OldSaveGame = MainSaveGame;
    MainSaveGame = NewObject<UPXMainSaveGame>(this);
    MainSaveGame->CurCharacterName = OldSaveGame->CurCharacterName;
    MainSaveGame->CurLevelName = FName();
}

bool UPXSaveGameSubsystem::Main_Saved()
{
    return !MainSaveGame->CurLevelName.IsNone();
}

void UPXSaveGameSubsystem::Main_AddResult(float Result)
{
    MainSaveGame->Results.Add(Result);
}

void UPXSaveGameSubsystem::Main_SyncLevel()
{
    MainSaveGame->CurLevelToChoiceAbility = MainSaveGame->CurLevelName;
}

void UPXSaveGameSubsystem::SaveBasicBuildData()
{
    bool result = UGameplayStatics::SaveGameToSlot(BasicBuildSaveGame, SlotName_BasicBuild, UserIndex);
    if (result)
    {
        UE_LOG(LogTemp,Log,TEXT("UPXSaveGameSubsystem::SaveBasicBuildData Success ^ ^"))
    }
}

UPXBasicBuildSaveGame* UPXSaveGameSubsystem::GetBasicBuildData()
{
    if (IsValid(BasicBuildSaveGame)) return BasicBuildSaveGame;
    
    if (UPXBasicBuildSaveGame* SaveGame = Cast<UPXBasicBuildSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Main, UserIndex)))
    {
        BasicBuildSaveGame = SaveGame;
        return BasicBuildSaveGame;
    }

    BasicBuildSaveGame = NewObject<UPXBasicBuildSaveGame>(this);
    SaveBasicBuildData();
    return BasicBuildSaveGame;
}

void UPXSaveGameSubsystem::SaveAchievementsData()
{
    bool result = UGameplayStatics::SaveGameToSlot(AchievementsSaveGame, SlotName_Achievements, UserIndex);
    if (result)
    {
        UE_LOG(LogTemp,Log,TEXT("UPXSaveGameSubsystem::SaveAchievementsData Success ^ ^"))
    }
}

UPXAchievementsSaveGame* UPXSaveGameSubsystem::GetAchievementsData()
{
    if (IsValid(AchievementsSaveGame)) return AchievementsSaveGame;
    
    if (UPXAchievementsSaveGame* SaveGame = Cast<UPXAchievementsSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Achievements, UserIndex)))
    {
        AchievementsSaveGame = SaveGame;
        return AchievementsSaveGame;
    }

    AchievementsSaveGame = NewObject<UPXAchievementsSaveGame>(this);
    SaveAchievementsData();
    return AchievementsSaveGame;
}

bool UPXSaveGameSubsystem::CompleteAchievement(FName AchievementKey)
{
    CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(AchievementsSaveGame, false);
    if (AchievementsSaveGame->CompletedAchievements.Contains(AchievementKey)) return false;

    AchievementsSaveGame->CompletedAchievements.Add(AchievementKey);

    SaveAchievementsData();

    return true;
}

void UPXSaveGameSubsystem::SaveShopData()
{
    bool result = UGameplayStatics::SaveGameToSlot(ShopSaveGame, SlotName_Shop, UserIndex);
    if (result)
    {
        UE_LOG(LogTemp,Log,TEXT("UPXSaveGameSubsystem::SaveShopData Success ^ ^"))
    }
}

UPXShopSaveGame* UPXSaveGameSubsystem::GetShopData()
{
    if (IsValid(ShopSaveGame)) return ShopSaveGame;
    
    if (UPXShopSaveGame* SaveGame = Cast<UPXShopSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Shop, UserIndex)))
    {
        ShopSaveGame = SaveGame;
        return ShopSaveGame;
    }

    AchievementsSaveGame = NewObject<UPXAchievementsSaveGame>(this);
    SaveShopData();
    return ShopSaveGame;
}

void UPXSaveGameSubsystem::SaveTalentsData()
{
    bool result = UGameplayStatics::SaveGameToSlot(TalentsSaveGame, SlotName_Talents, UserIndex);
    if (result)
    {
        UE_LOG(LogTemp,Log,TEXT("UPXSaveGameSubsystem::SaveTalentsData Success ^ ^"))
    }
}

UPXTalentsSaveGame* UPXSaveGameSubsystem::GetTalentsData()
{
    if (IsValid(TalentsSaveGame)) return TalentsSaveGame;

    if (UPXTalentsSaveGame* SaveGame = Cast<UPXTalentsSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Talents, UserIndex)))
    {
        TalentsSaveGame = SaveGame;
        return TalentsSaveGame;
    }

    TalentsSaveGame = NewObject<UPXTalentsSaveGame>(this);
    SaveTalentsData();
    return TalentsSaveGame;
}
