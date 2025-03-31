// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask_EnemyMoveToActionLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/Components/EnemyAIComponent.h"
#include "FunctionLibrary/SpaceFuncLib.h"
#include "Interfaces/Enemy/AI/EnemyAI_Interface.h"


UBTTask_EnemyMoveToActionLocation::UBTTask_EnemyMoveToActionLocation()
{
	TickInterval.Interval = 0.1f;
	
}

EBTNodeResult::Type UBTTask_EnemyMoveToActionLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		FinishExecute(false);
		return EBTNodeResult::Failed;
	}

	if (BlackboardComponent->GetValueAsBool(FName("bInAttackState")))
	{
		FinishExecute(false);
		return EBTNodeResult::Failed;
	}
	

	UObject* PlayerPawn = BlackboardComponent->GetValueAsObject(FName("PlayerPawn"));
	if (!PlayerPawn)
	{
		FinishExecute(false);
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIOwner->GetPawn();
	if (!Pawn || !Pawn->Implements<UEnemyAI_Interface>())
	{
		FinishAbort();
		return EBTNodeResult::Failed;
	}

	UEnemyAIComponent* EnemyAIComponent = Pawn->GetComponentByClass<UEnemyAIComponent>();
	AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(AIOwner);
	if (!EnemyAIComponent || !EnemyAIController)
	{
		FinishAbort();
		return EBTNodeResult::Failed;
	}
	
	if (!IEnemyAI_Interface::Execute_CanMove_EnemyAI(Pawn))
	{
		FinishExecute(true);
		return EBTNodeResult::Failed;
	}
	
	if (IEnemyAI_Interface::Execute_Dash_EnemyAI(Pawn))
	{
		FinishExecute(true);
		return EBTNodeResult::Failed;
	}

	// 朝更近的区间走去
	FVector TargetLocation = EnemyAIComponent->GetActionFieldLocation(true);
	bool bIsCliff = USpaceFuncLib::CheckCliffProcess(
		Pawn->GetActorLocation(),TargetLocation,
			EnemyAIComponent->GetCheckCliffHeight_EnemyAI()
		);
	if (bIsCliff)
	{
		FinishExecute(true);
		return EBTNodeResult::Failed;
	}
	EnemyAIController->SimpleMoveToLocation(TargetLocation);
	FinishExecute(true);
	return EBTNodeResult::InProgress;
}

void UBTTask_EnemyMoveToActionLocation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}
