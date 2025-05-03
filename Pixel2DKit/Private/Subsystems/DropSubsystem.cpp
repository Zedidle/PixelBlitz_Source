// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/DropSubsystem.h"
#include "Item/BaseItem.h"
#include "Engine/DataTable.h"
#include "Settings/DataTableSettings.h"
#include "Engine/DeveloperSettings.h"
#include "Pixel2DKit/Pixel2DKit.h"


void UDropSubsystem::Deinitialize()
{
	Super::Deinitialize();
	for (int i=0; i < TimerHandles.Num(); i++)
	{
		if (TimerHandles[i].IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandles[i]);
		}
	}
	TimerHandles.Empty();
}

void UDropSubsystem::BeginDestroy()
{
	Super::BeginDestroy();
}

void UDropSubsystem::SpawnItems(const FDrop& DropData, const FVector& SpawnLocation, float SpawnFrequency)
{
	if (DropData.Items.IsEmpty()) return ;
	const UDataTableSettings* Settings = GetDefault<UDataTableSettings>();
	if (!IsValid(Settings)) return;
	
	ItemDataTable = Settings->GetItemData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ItemDataTable)
	
	FTimerHandle timerHandle;
	TimerHandles.Add(timerHandle);
	TimerHandle_Times.Add(DropData.DropTotalNum);
	int handleIndex = TimerHandles.Num()-1;

	FTimerDelegate timerDelegate = FTimerDelegate::CreateLambda(
		[this, DropData, handleIndex, SpawnLocation]()
		{
			// 减少计数并检查生命周期
			if (TimerHandle_Times[handleIndex] <= 0)
			{
				GetWorld()->GetTimerManager().ClearTimer(TimerHandles[handleIndex]);
				return;
			}
			

			TimerHandle_Times[handleIndex] --;
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,
			FString::Printf(TEXT("UDropSubsystem SpawnItems %i, %d"), TimerHandle_Times[handleIndex] , __LINE__));

			TArray<FName> Keys;
			DropData.Items.GetKeys(Keys);

			// 随机出对应的物品ID
			int totalR = 0;
			int tmpR = 0;
			for (int i = 0; i < Keys.Num(); i++)
			{
				totalR += DropData.Items[Keys[i]];
			}
			const int v = FMath::RandHelper(totalR); 
			int _index = 0;
			for (int i = 0; i < Keys.Num(); i++)
			{
				tmpR += DropData.Items[Keys[i]];
				if (v <= tmpR )
				{
					_index = i;
					break;
				}
			}
			
			const FName ItemName = Keys[_index];
			if (FItemData* ItemData = ItemDataTable->FindRow<FItemData>(ItemName, TEXT("DropSystem GetItemDataByName")))
			{
// 				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue,
// FString::Printf(TEXT("UDropSubsystem ABaseItem Spawn Success, %d"), __LINE__));
				ABaseItem* Item = GetWorld()->SpawnActor<ABaseItem>(ItemData->SpawnItemClass, SpawnLocation, FRotator(0, 0, 0));
			}
		});
	
	GetWorld()->GetTimerManager().SetTimer(timerHandle, timerDelegate, SpawnFrequency, true);
}
