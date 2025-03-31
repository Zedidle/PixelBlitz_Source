// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BTTask_EnemyAction.generated.h"

/**
 * Enemy 判断当前所处方位与距离，并执行该方位的行动
 */
UCLASS()
class PIXEL2DKIT_API UBTTask_EnemyAction : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
