// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PXSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"


void UPXSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
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
    if (!LoadedRecord.Contains(SlotName_Main))
    {
        if (UPXMainSaveGame* SaveGame = Cast<UPXMainSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_Main, UserIndex)))
        {
            LoadedRecord.Add(SlotName_Main, true);
            MainSaveGame = SaveGame;
            return MainSaveGame;
        }
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
    if (UPXBasicBuildSaveGame* BasicBuildSaveGame = Cast<UPXBasicBuildSaveGame>(UGameplayStatics::CreateSaveGameObject(UPXBasicBuildSaveGame::StaticClass())))
    {
        BasicBuildSaveGame->Data = BasicBuildSaveData;
        // 保存到角色信息存档槽
        bool result = UGameplayStatics::SaveGameToSlot(BasicBuildSaveGame, SlotName_BasicBuild, UserIndex);
        if (result)
        {
            UE_LOG(LogTemp,Log,TEXT("UPXSaveGameSubsystem::SaveBasicBuildData Success ^ ^"))
        }
    }
}

FPXBasicBuildSaveData& UPXSaveGameSubsystem::GetBasicBuildData()
{
    if (!LoadedRecord.Contains(SlotName_Main))
    {
        if (UPXBasicBuildSaveGame* PXBasicBuildSaveGame = Cast<UPXBasicBuildSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName_BasicBuild, UserIndex)))
        {
            LoadedRecord.Add(SlotName_BasicBuild, true);
            BasicBuildSaveData = PXBasicBuildSaveGame->Data;
        }
    }

    return BasicBuildSaveData;

}
