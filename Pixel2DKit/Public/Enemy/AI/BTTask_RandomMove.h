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

	// 布置关卡时，也应注意平台之间的距离不应低于该值
	// 巡逻时随机移动的范围, 不应该超过平台的间隔导致空间位置不足，从而强行寻路掉落平台
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", UIMin = 50, UIMax = 100))
	float CheckMoveStartDistance = 100;

	// 当前每次移动距离
	float CurDistance = 100;
	// 当在某方向上移动失败时，削减距离
	float DeDistanceRate = 0.9;
	
	// 当前移动偏转是顺时针还是逆时针，当移动成功会确定方向，直至下一次移动失败
	bool bRotateClockwise = false;
	bool bMoveSucceeded = false;
	
	// 当前移动偏转角度
	float CurRotateValue = 5;



	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", UIMin = 5, UIMax = 15))
	float MinRotateValue = 5;
	// 转向范围，当顺利导航逐渐减少
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", UIMin = 30, UIMax = 180))
	float MaxRotateValue = 150;
	

	

	
	// 悬崖测试距离衰减率，0.5 到 0.95 之间， 值越小越快，但越容易漏
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", UIMin = 0.5, UIMax = 0.95))
	float CliffCheckRate = 0.9;

	// 最小的方向重置距离， 当某方向的悬崖测试距离衰减至小于该值，则重置方向与距离
	// 能有效提高寻路效率，但过大会有掉落风险，最好是角色胶囊体的半径
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", UIMin = 10, UIMax = 100))
	float MinDirSwitchDistance = 5; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", UIMin = 10, UIMax = 100))
	float CheckCliffHeight = 50;

	
public:

	UBTTask_RandomMove();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


	
};
