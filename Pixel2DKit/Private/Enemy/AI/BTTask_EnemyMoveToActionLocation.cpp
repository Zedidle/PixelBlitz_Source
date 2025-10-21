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

	APawn* SelfPawn = AIOwner->GetPawn();
	if (!SelfPawn || !SelfPawn->Implements<UEnemyAI_Interface>())
	{
		FinishAbort();
		return EBTNodeResult::Failed;
	}

	UEnemyAIComponent* EnemyAIComponent = SelfPawn->GetComponentByClass<UEnemyAIComponent>();
	AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(AIOwner);
	if (!EnemyAIComponent || !EnemyAIController)
	{
		FinishAbort();
		return EBTNodeResult::Failed;
	}
	
	if (!IEnemyAI_Interface::Execute_CanMove(SelfPawn))
	{
		FinishExecute(true);
		return EBTNodeResult::Failed;
	}
	
	if (IEnemyAI_Interface::Execute_Dash(SelfPawn))
	{
		FinishExecute(true);
		return EBTNodeResult::Failed;
	}

	FVector PawnLocation = SelfPawn->GetActorLocation();
	
	// 先做一个简单悬崖判断
	bool bIsCliff = USpaceFuncLib::CheckCliffProcess(PawnLocation,
		PawnLocation + (PlayerPawn->GetActorLocation() - PawnLocation).GetSafeNormal2D() * EnemyAIComponent->GetMinDirSwitchDistance() * 2,
						EnemyAIComponent->GetCheckCliffHeight(), 0.9, EnemyAIComponent->GetMinDirSwitchDistance() / 2);
	
	if (bIsCliff)
	{
		FinishExecute(true);
		return EBTNodeResult::Failed;
	}
	
	FHitResult OutHit;
	// 向玩家位置做一个射线检测，判断能否直接看见
	bool bLostSeePlayer = UKismetSystemLibrary::LineTraceSingle(SelfPawn->GetWorld(), PawnLocation, PlayerPawn->GetActorLocation(),
	TraceTypeQuery1, false, {SelfPawn},
			EDrawDebugTrace::None, OutHit, true,
			FLinearColor::Red, FLinearColor::Green, 1.0f);

	FVector TargetLocation;
	if (bLostSeePlayer)
	{
		TargetLocation = EnemyAIComponent->GetPlayerPathPoint();
		DrawDebugSphere(GetWorld(), SelfPawn->GetActorLocation(), 10, 16, FColor::Green, false, 3);
		// TargetLocation = EnemyAIComponent->GetMoveDotDirRandLocation(TargetLocation, 10.0f, 30.0f);
	}
	else
	{
		TargetLocation = EnemyAIComponent->GetNearestActionFieldCanAttackLocation();
		TargetLocation += FMath::RandRange(0.0f ,0.1f) * (PlayerPawn->GetActorLocation() - TargetLocation);
	}
	
	
	bIsCliff = USpaceFuncLib::CheckCliffProcess(SelfPawn->GetActorLocation(),TargetLocation,
						EnemyAIComponent->GetCheckCliffHeight(), 0.9, EnemyAIComponent->GetMinDirSwitchDistance());
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
