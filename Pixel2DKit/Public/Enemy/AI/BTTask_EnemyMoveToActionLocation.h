// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BTTask_EnemyMoveToActionLocation.generated.h"

/**
 * Enemy 朝玩家行动，一般是越走越近
 */
UCLASS()
class PIXEL2DKIT_API UBTTask_EnemyMoveToActionLocation : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

	
	

public:
	UBTTask_EnemyMoveToActionLocation();


	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;



	
	
	
	
};
