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
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AIOwner)

	TickInterval.Interval = 0.3f;
	CheckMoveStartDistance = 100.f;
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

	
	MinDirSwitchDistance = EnemyAIComponent->GetMinDirSwitchDistance();
	CheckCliffHeight = EnemyAIComponent->GetCheckCliffHeight();
	
	// 计算方向与距离
	if (bMoveSucceeded)
	{
		// 成功移动时，重置移动的偏转角度
		CurRotateValue = MinRotateValue + FMath::FRandRange(0, MinRotateValue);
		CurDistance = CheckMoveStartDistance * FMath::FRandRange(0.8, 1.2);
	}
	else
	{
		bRotateClockwise = FMath::RandBool();

		// 移动失败的话，逐步加大转向角度
		CurRotateValue = FMath::Clamp( CurRotateValue * 1.1 + 5, MinRotateValue, MaxRotateValue);
		// 缩减移动距离
		CurDistance = FMath::Max(CurDistance * DeDistanceRate,   MinDirSwitchDistance * 2);
	}
	
	FVector CurDirection = FRotator(0,bRotateClockwise ? CurRotateValue : -CurRotateValue,0).RotateVector(Pawn->GetActorForwardVector())
					.GetSafeNormal2D(0.01f);

	// CliffHeight 还需动态依据怪物身高调整
	FVector CurTargetLocation = Pawn->GetActorLocation() + CurDistance * CurDirection;
	if (!USpaceFuncLib::CheckCliffProcess( Pawn->GetActorLocation(), CurTargetLocation, CheckCliffHeight, CliffCheckRate, MinDirSwitchDistance))
	{
		Controller->SimpleMoveToLocation(CurTargetLocation);

		bMoveSucceeded = true;
		FinishExecute(true);
		return EBTNodeResult::Succeeded;
	}

	bMoveSucceeded = false;
	FinishExecute(false);
	return EBTNodeResult::Failed;
}

void UBTTask_RandomMove::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}
