// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask_RandomMove.h"
#include "Interfaces/Enemy/AI/EnemyAI_Interface.h"
#include "Enemy/BaseEnemy.h"
#include "AIController.h"
#include "Utilitys/SpaceFuncLib.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/Components/EnemyAIComponent.h"


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


	// 重新计算方向与距离
	float curDistance = IEnemyAI_Interface::Execute_GetRandomMoveRange(Pawn) * FMath::RandRange(0.8f, 1.2f);
	FVector dir = FRotator(0,FMath::RandRange(-CurRotateValue, CurRotateValue),0).RotateVector(Pawn->GetActorForwardVector())
					.GetSafeNormal2D(0.01f);

	// CliffHeight 还需动态依据怪物身高调整
	while (MinDirSwitchDistance < CliffCheckRate * curDistance)
	{
		CurTargetLocation = Pawn->GetActorLocation() + curDistance * dir;
		curDistance = CliffCheckRate * curDistance;
		if (UEnemyAIComponent* EnemyAIComponent = Pawn->GetComponentByClass<UEnemyAIComponent>())
		{
			if (!USpaceFuncLib::CheckCliff(CurTargetLocation, EnemyAIComponent->GetCheckCliffHeight_EnemyAI()))
			{
				CurTargetLocation = EnemyAIComponent->GetMoveDotDirRandLocation(CurTargetLocation);
				Controller->SimpleMoveToLocation(CurTargetLocation);
				FinishExecute(true);
				CurRotateValue = FMath::Clamp( CurRotateValue * 0.9 -5, MinRotateValue, MaxRotateValue);
				return EBTNodeResult::Succeeded;
			}
		}
	}

	CurRotateValue = FMath::Clamp( CurRotateValue * 1.1 + 5, MinRotateValue, MaxRotateValue);
	FinishExecute(false);
	return EBTNodeResult::Failed;
	// return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTTask_RandomMove::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* Pawn = AIOwner->GetPawn();
	if (!Pawn ||
		!Pawn->Implements<UFight_Interface>() ||
		!IFight_Interface::Execute_IsAlive(Pawn) ||
		!Pawn->Implements<UEnemyAI_Interface>())
	{
		FinishAbort();	
	}
	
	if (IFight_Interface::Execute_GetTarget(Pawn))
	{
		FinishExecute(true);
	}

	if (Pawn->GetActorLocation().Equals( PreLocation, 0.1f))
	{
		// 可能平台边缘卡住，向前小跳
		Pawn->AddActorWorldOffset(
			(CurTargetLocation - Pawn->GetActorLocation()).GetSafeNormal2D(0.01f) * 3 + FVector(0, 0, 5)
		);
		FinishExecute(true);
	}
	else
	{
		PreLocation = Pawn->GetActorLocation();
		if (FVector::DistXY(PreLocation, CurTargetLocation) < MinDirSwitchDistance / 2)
		{
			AIOwner->StopMovement();
			FinishExecute(true);
		}
		else
		{
			if (USpaceFuncLib::CheckCliff(PreLocation +
				(Pawn->GetActorForwardVector().GetSafeNormal2D() * MinDirSwitchDistance / 2)))
			{
				AIOwner->StopMovement();
				FinishExecute(true);
			}
		}
	}

	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}
