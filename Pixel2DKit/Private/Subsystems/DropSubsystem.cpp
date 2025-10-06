// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/DropSubsystem.h"
#include "Item/BaseItem.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"
#include "Item/BaseInteractableItem.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/PXCustomStruct.h"


void UDropSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UDropSubsystem::BeginDestroy()
{
	Super::BeginDestroy();
}

void UDropSubsystem::SpawnItems(const FDrop& DropData, const FVector& SpawnLocation)
{
	if (DropData.Items.IsEmpty()) return ;
	
	UGameInstance* GameInstance = GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem)

	ItemDataTable = DataTableSubsystem->GetItemData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ItemDataTable)

	FString UniqueTimerName = FString::Printf(TEXT("UDropSubsystem_SpawnItems_%s"), *FGuid::NewGuid().ToString());
	
	UTimerSubsystemFuncLib::SetDelayLoop(GetWorld(), FName(*UniqueTimerName),
		[WeakThis=TWeakObjectPtr<ThisClass>(this), DropData, SpawnLocation]
		{
			if (!WeakThis.IsValid()) return;
				
			FName ItemName;
			UCommonFuncLib::CalRandomMap(DropData.Items, ItemName);
			if (FItemData* ItemData = WeakThis->ItemDataTable->FindRow<FItemData>(ItemName, TEXT("DropSystem GetItemDataByName")))
			{
				// 	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("UDropSubsystem ABaseItem Spawn Success, %d"), __LINE__));
				ABaseInteractableItem* Item = WeakThis->GetWorld()->SpawnActor<ABaseInteractableItem>(ItemData->SpawnItemClass,
									SpawnLocation, FRotator(0, 0, 0));
				if (Item)
				{
					Item->SetVelocityOnSpawn(DropData.SpawnRandRotate, DropData.SpawnSpeed);
				}
			}
	}, DropData.SpawnFrequency, -1, DropData.DropTotalNum);

}
