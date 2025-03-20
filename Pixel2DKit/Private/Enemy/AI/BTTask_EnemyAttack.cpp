// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask_EnemyAttack.h"
#include "AIController.h"
#include "Interfaces/Enemy/AI/EnemyAI_Interface.h"

EBTNodeResult::Type UBTTask_EnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!AIOwner || !AIOwner->GetPawn())
	{
		FinishAbort();
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIOwner->GetPawn();
	if (!Pawn || !Pawn->Implements<UEnemyAI_Interface>())
	{
		FinishAbort();
		return EBTNodeResult::Failed;
	}

	if (IEnemyAI_Interface::Execute_InAtttckRange_EnemyAI(Pawn))
	{
		IEnemyAI_Interface::Execute_OnReachedAttackLocation_EnemyAI(Pawn);
		FinishExecute(true);
	}
	else
	{
		FinishExecute(false);
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
