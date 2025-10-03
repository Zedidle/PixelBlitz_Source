// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "DropSubsystem.generated.h"

class ABaseItem;

USTRUCT(BlueprintType)
struct FItemData: public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	FName ItemName;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	FText ItemDisplayName;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	TSubclassOf<ABaseItem> SpawnItemClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSoftObjectPtr<USoundCue> UseSound;
};






/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UDropSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UDataTable> ItemDataTable;
	TArray<FTimerHandle> TimerHandles;
	TArray<int32> TimerHandle_Times;
	

	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
	}

	virtual void Deinitialize() override;

	
	virtual void BeginDestroy() override;
	
	
	UFUNCTION(BlueprintCallable, Category="DropSubsystem")
	void SpawnItems(const FDrop& DropData, const FVector& SpawnLocation, float SpawnFrequency = 0.2f);
	
};
