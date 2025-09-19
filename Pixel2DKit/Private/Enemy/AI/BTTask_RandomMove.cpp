// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask_RandomMove.h"
#include "Interfaces/Enemy/AI/EnemyAI_Interface.h"
#include "Enemy/BaseEnemy.h"
#include "AIController.h"
#include "Utilitys/SpaceFuncLib.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/Components/EnemyAIComponent.h"
#include "Utilitys/DebugFuncLab.h"


UBTTask_RandomMove::UBTTask_RandomMove()
{
	TickInterval.Interval = 0.2f;

}

EBTNodeResult::Type UBTTask_RandomMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!World)
	{
		FinishExecute(false);
		return EBTNodeResult::Failed;
	}
	
	if (!AIOwner)
	{
		FinishExecute(false);
		return EBTNodeResult::Failed;
	}
	
	AEnemyAIController* Controller = Cast<AEnemyAIController>(AIOwner);
	if (!Controller)
	{
		FinishExecute(false);
		return EBTNodeResult::Failed;
	}
	
	APawn* Pawn = AIOwner->GetPawn();

	if (!Pawn ||
		!Pawn->Implements<UFight_Interface>() ||
		!IFight_Interface::Execute_IsAlive(Pawn) ||
		!Pawn->Implements<UEnemyAI_Interface>())
	{
		FinishExecute(false);
		return EBTNodeResult::Failed;
	}

	UEnemyAIComponent* EnemyAIComponent = Pawn->GetComponentByClass<UEnemyAIComponent>();
	if (!EnemyAIComponent)
	{
		FinishExecute(false);
		return EBTNodeResult::Failed;
	}
	
	// 重新计算方向与距离
	float curDistance = IEnemyAI_Interface::Execute_GetRandomMoveRange(Pawn) * FMath::RandRange(0.8f, 1.2f);
	if (!bMoveSucceeded)
	{
		bRotateClockwise = FMath::RandBool();
	}
	FVector dir = FRotator(0,bRotateClockwise ? CurRotateValue : -CurRotateValue,0).RotateVector(Pawn->GetActorForwardVector())
					.GetSafeNormal2D(0.01f);

	// CliffHeight 还需动态依据怪物身高调整
	while (MinDirSwitchDistance < curDistance)
	{
		FVector CurTargetLocation = Pawn->GetActorLocation() + curDistance * dir;
		curDistance = CliffCheckRate * curDistance;
		if (!USpaceFuncLib::CheckCliff(CurTargetLocation, EnemyAIComponent->GetCheckCliffHeight_EnemyAI()))
		{
			Controller->SimpleMoveToLocation(CurTargetLocation);
			FinishExecute(true);
			// 成功移动时，重置移动的偏转角度
			CurRotateValue = MinRotateValue + FMath::FRandRange(0, MinRotateValue);
			bMoveSucceeded = true;
			return EBTNodeResult::Succeeded;
		}
	}

	CurRotateValue = FMath::Clamp( CurRotateValue * 1.1 + 5, MinRotateValue, MaxRotateValue);
	bMoveSucceeded = false;
	FinishExecute(false);
	return EBTNodeResult::Failed;
	// return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTTask_RandomMove::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}
