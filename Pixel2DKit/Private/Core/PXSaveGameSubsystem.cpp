// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PXSaveGameSubsystem.h"

#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/PXSettingSaveGame.h"
#include "SaveGame/PXShopSaveGame.h"
#include "SaveGame/PXTalentsSaveGame.h"
#include "Settings/PXInputSettings.h"
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

void UPXSaveGameSubsystem::InitData_Setting()
{
    if (UPXSettingSaveGame* SaveGame = Cast<UPXSettingSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Setting, UserIndex)))
    {
        SettingSaveGame = SaveGame;


        if (const UPXInputSettings* Settings = GetDefault<UPXInputSettings>())
        {
            Settings->Gamepad_IMC->UnmapAll();
            Settings->Keyboard_IMC->UnmapAll();

            for (FEnhancedActionKeyMapping KM : SettingSaveGame->ControlSetting_CharacterControl_GamePad_Mapping)
            {
                Settings->Gamepad_IMC->MapKey(KM.Action, KM.Key);
            }
            
            for (FEnhancedActionKeyMapping KM : SettingSaveGame->ControlSetting_CharacterControl_KeyBoard_Mapping)
            {
                Settings->Keyboard_IMC->MapKey(KM.Action, KM.Key);
            } 
        }
    }
    else
    {
        SettingSaveGame = NewObject<UPXSettingSaveGame>(this);
        // 按键存档
        if (const UPXInputSettings* Settings = GetDefault<UPXInputSettings>())
        {
            UInputMappingContext* Gamepad_IMC = Cast<UInputMappingContext>(Settings->Gamepad_IMC.Get());
            UInputMappingContext* Keyboard_IMC = Cast<UInputMappingContext>(Settings->Keyboard_IMC.Get());
            
            SettingSaveGame->ControlSetting_CharacterControl_GamePad_Mapping = Gamepad_IMC->GetMappings();
            SettingSaveGame->ControlSetting_CharacterControl_KeyBoard_Mapping = Keyboard_IMC->GetMappings();
        }

        SaveSettingData();
    }

    // 第一次开游戏时，以及后续读档时
    UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
    UserSettings->SetScreenResolution(USettingFuncLib::GetScreenResolution(SettingSaveGame->VideoSetting_Resolution));
    UserSettings->SetFullscreenMode(USettingFuncLib::GetWindowMode(SettingSaveGame->VideoSetting_ScreenMode));
    UserSettings->ApplySettings(false);
    
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

void UPXSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    InitData_Main();
    InitData_BasicBuild();
    InitData_Setting();
    InitData_Achievements();
    InitData_Shop();
    InitData_Talents();
}

void UPXSaveGameSubsystem::SaveMainData()
{
    bool result = UGameplayStatics::SaveGameToSlot(MainSaveGame, SlotName_Main, UserIndex);
    if (result)
    {
        UE_LOG(LogTemp,Log,TEXT("UPXSaveGameSubsystem::SaveMainData Success ^ ^"))
    }
}

UPXMainSaveGame* UPXSaveGameSubsystem::GetMainData()
{
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
    return MainSaveGame->CurLevelChoiceAbility ==  MainSaveGame->CurCharacterName;
}

void UPXSaveGameSubsystem::Main_TotalInit(TSubclassOf<UPrimaryDataAsset> WeatherType, EStyleType StyleType)
{
    MainSaveGame = NewObject<UPXMainSaveGame>(this);
    MainSaveGame->CurWeatherType = WeatherType;
    MainSaveGame->StyleType = StyleType;
}

bool UPXSaveGameSubsystem::Main_Saved()
{
    return MainSaveGame->CurLevelName == "";
}

void UPXSaveGameSubsystem::Main_AddResult(float Result)
{
    MainSaveGame->Results.Add(Result);
}

void UPXSaveGameSubsystem::Main_SyncLevel()
{
    MainSaveGame->CurLevelChoiceAbility = MainSaveGame->CurLevelName;
    MainSaveGame->PreLevelClearAllMonster = false;
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
    if (UPXBasicBuildSaveGame* SaveGame = Cast<UPXBasicBuildSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Main, UserIndex)))
    {
        BasicBuildSaveGame = SaveGame;
        return BasicBuildSaveGame;
    }

    BasicBuildSaveGame = NewObject<UPXBasicBuildSaveGame>(this);
    SaveBasicBuildData();
    return BasicBuildSaveGame;
}

void UPXSaveGameSubsystem::SaveSettingData()
{
    bool result = UGameplayStatics::SaveGameToSlot(SettingSaveGame, SlotName_Setting, UserIndex);
    if (result)
    {
        UE_LOG(LogTemp,Log,TEXT("UPXSaveGameSubsystem::SaveSettingData Success ^ ^"))
    }
}

UPXSettingSaveGame* UPXSaveGameSubsystem::GetSettingData()
{
    if (UPXSettingSaveGame* SaveGame = Cast<UPXSettingSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Setting, UserIndex)))
    {
        SettingSaveGame = SaveGame;
        return SettingSaveGame;
    }

    SettingSaveGame = NewObject<UPXSettingSaveGame>(this);
    SaveSettingData();
    return SettingSaveGame;
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
    if (UPXAchievementsSaveGame* SaveGame = Cast<UPXAchievementsSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Setting, UserIndex)))
    {
        AchievementsSaveGame = SaveGame;
        return AchievementsSaveGame;
    }

    AchievementsSaveGame = NewObject<UPXAchievementsSaveGame>(this);
    SaveAchievementsData();
    return AchievementsSaveGame;
}

bool UPXSaveGameSubsystem::CompleteAchievement(FName AchievementRowName)
{
    if (AchievementsSaveGame->Achieve_CompletedAchievements.Contains(AchievementRowName)) return false;

    AchievementsSaveGame->Achieve_CompletedAchievements.Add(AchievementRowName);

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
    if (UPXTalentsSaveGame* SaveGame = Cast<UPXTalentsSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Talents, UserIndex)))
    {
        TalentsSaveGame = SaveGame;
        return TalentsSaveGame;
    }

    TalentsSaveGame = NewObject<UPXTalentsSaveGame>(this);
    SaveTalentsData();
    return TalentsSaveGame;
}
