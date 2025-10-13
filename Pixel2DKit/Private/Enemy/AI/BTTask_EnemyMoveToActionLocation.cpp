// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask_EnemyMoveToActionLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/Components/EnemyAIComponent.h"
#include "Utilitys/SpaceFuncLib.h"
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
	

	APawn* PlayerPawn = Cast<APawn>(BlackboardComponent->GetValueAsObject(FName("PlayerPawn")));
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
	
	if (!IEnemyAI_Interface::Execute_CanMove(Pawn))
	{
		FinishExecute(true);
		return EBTNodeResult::Failed;
	}
	
	if (IEnemyAI_Interface::Execute_Dash(Pawn))
	{
		FinishExecute(true);
		return EBTNodeResult::Failed;
	}

	// 朝更近的区间走去
	FVector TargetLocation = EnemyAIComponent->GetNearestActionFieldCanAttackLocation();
	bool bIsCliff = USpaceFuncLib::CheckCliffProcess(
		Pawn->GetActorLocation(),TargetLocation,
			EnemyAIComponent->GetCheckCliffHeight_EnemyAI()
		);
	if (bIsCliff)
	{
		FinishExecute(true);
		return EBTNodeResult::Failed;
	}

	// 再往玩家方向做点随机距离
	TargetLocation += FMath::RandRange(0.0f ,0.1f) * (PlayerPawn->GetActorLocation() - TargetLocation);
	
	EnemyAIController->SimpleMoveToLocation(TargetLocation);
	FinishExecute(true);
	return EBTNodeResult::InProgress;
}

void UBTTask_EnemyMoveToActionLocation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}
