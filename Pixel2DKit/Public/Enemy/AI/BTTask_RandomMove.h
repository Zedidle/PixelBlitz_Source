// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BTTask_RandomMove.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UBTTask_RandomMove : public UBTTask_BlueprintBase
{
	GENERATED_BODY()
	float CliffCheckRate = 0.9; // 悬崖测试 衰减调整率
	float MinDirSwitchDistance = 100; // 最小的方向重置距离
	
	float CurRotateValue = 30;  // 当前转向角度范围 
	float MaxRotateValue = 120;  // 最大转向角度，当碰到悬崖逐渐增大
	float MinRotateValue = 20;  // 最小转向角度，当顺利导航逐渐减少

	FVector CurTargetLocation;
	FVector PreLocation; // TickTask 周期内的上一次位置

public:

	UBTTask_RandomMove();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	
};
