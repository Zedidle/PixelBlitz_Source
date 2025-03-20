// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BTTask_EnemyMoveToAttackLocation.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UBTTask_EnemyMoveToAttackLocation : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

	
	

public:
	UBTTask_EnemyMoveToAttackLocation();


	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;



	
	
	
	
};
