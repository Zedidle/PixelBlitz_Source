// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask_EnemyMoveToActionLocation.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Enemy/BaseEnemy.h"
#include "Enemy/EnemyAIController.h"
#include "Enemy/Components/EnemyAIComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utilitys/SpaceFuncLib.h"
#include "Interfaces/Enemy/AI/EnemyAI_Interface.h"
#include "Utilitys/DebugFuncLab.h"


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

	ABaseEnemy* SelfEnemyPawn = AIOwner->GetPawn<ABaseEnemy>();
	if (!SelfEnemyPawn || !SelfEnemyPawn->Implements<UEnemyAI_Interface>() || SelfEnemyPawn->IsActionMoving())
	{
		FinishAbort();
		return EBTNodeResult::Failed;
	}

	UEnemyAIComponent* EnemyAIComponent = SelfEnemyPawn->GetComponentByClass<UEnemyAIComponent>();
	AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(AIOwner);
	if (!EnemyAIComponent || !EnemyAIController)
	{
		FinishAbort();
		return EBTNodeResult::Failed;
	}
	
	if (!IEnemyAI_Interface::Execute_CanMove(SelfEnemyPawn))
	{
		FinishExecute(true);
		return EBTNodeResult::Failed;
	}
	
	if (IEnemyAI_Interface::Execute_Dash(SelfEnemyPawn))
	{
		FinishExecute(true);
		return EBTNodeResult::Failed;
	}

	FVector PawnLocation = SelfEnemyPawn->GetActorLocation();
	
	// 先做一个简单悬崖判断
	bool bIsCliff = USpaceFuncLib::CheckCliffProcess(PawnLocation,
		PawnLocation + (PlayerPawn->GetActorLocation() - PawnLocation).GetSafeNormal2D() * EnemyAIComponent->GetMinDirSwitchDistance() * 2,
						EnemyAIComponent->GetCheckCliffHeight(), 0.9, EnemyAIComponent->GetMinDirSwitchDistance() / 2);

	bool PlayerOnGround = PlayerPawn->GetMovementComponent() && PlayerPawn->GetMovementComponent()->IsMovingOnGround();
	bool SelfEnemyOnGround = SelfEnemyPawn->GetMovementComponent() && SelfEnemyPawn->GetMovementComponent()->IsMovingOnGround();
	
	if (bIsCliff || !SelfEnemyPawn->InAttackRange_Vertical() && PlayerOnGround && SelfEnemyOnGround)
	{
		TArray<FVector> Points;
		// 判断是否可以直接跳过去？
		if (USpaceFuncLib::GetJumpPoints(Points, PawnLocation, PlayerPawn->GetActorLocation()))
		{
			// 一般下标为 0 的最远
			TArray<FVector> PointsMaybe;
			for (FVector& P : Points)
			{
				if (FMath::Abs(P.Z - PlayerPawn->GetActorLocation().Z) < 50)
				{
					PointsMaybe.Add(P);
				}
			}

			// 剔除较近的，避免由于自身半径卡平台边缘掉落
			if (PointsMaybe.Num() > 1)
			{
				PointsMaybe.RemoveAt(PointsMaybe.Num()-1);
			}

			if (PointsMaybe.IsEmpty())
			{
				FinishExecute(false);
				return EBTNodeResult::Failed;
			}

			FVector JumpPoint = PointsMaybe[FMath::RandRange(0, PointsMaybe.Num() - 1)] + SelfEnemyPawn->GetDefaultHalfHeight();

			SelfEnemyPawn->TryJumpToOtherPlatform(PawnLocation, JumpPoint);

			FinishExecute(true);
			return EBTNodeResult::InProgress;
		}

		FinishExecute(false);
		return EBTNodeResult::Failed;
	}
	
	FHitResult OutHit;
	// 向玩家位置做一个射线检测，判断能否直接看见
	bool bLostSeePlayer = UKismetSystemLibrary::LineTraceSingle(SelfEnemyPawn->GetWorld(), PawnLocation, PlayerPawn->GetActorLocation(),
	TraceTypeQuery1, false, {SelfEnemyPawn},
			EDrawDebugTrace::None, OutHit, true,
			FLinearColor::Red, FLinearColor::Green, 1.0f);

	FVector TargetLocation;
	bool bFoundPathPoint = false;
	if (bLostSeePlayer)
	{
		bFoundPathPoint = EnemyAIComponent->GetPlayerPathPoint(TargetLocation);
		EnemyAIComponent->MoveCheckAllies(TargetLocation, TargetLocation, 50);
	}
	if (!bFoundPathPoint)
	{
		TargetLocation = EnemyAIComponent->GetNearestActionFieldCanAttackLocation();
		TargetLocation += FMath::RandRange(0.0f ,0.1f) * (PlayerPawn->GetActorLocation() - TargetLocation);
	}
	
	bIsCliff = USpaceFuncLib::CheckCliffProcess(SelfEnemyPawn->GetActorLocation(),TargetLocation,
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
