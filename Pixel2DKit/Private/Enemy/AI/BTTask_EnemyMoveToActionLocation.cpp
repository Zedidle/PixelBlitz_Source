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

	float VerticalDistanceToPlayer = SelfEnemyPawn->GetVerticalDistanceToPlayer();

	bool PlayerOnGround = PlayerPawn->GetMovementComponent() && PlayerPawn->GetMovementComponent()->IsMovingOnGround();
	
	// 中间隔了悬崖，或者高度差超过某数值，触发跳跃
	if (bIsCliff || FMath::Abs(VerticalDistanceToPlayer) >= 50 && PlayerOnGround)
	{
		// 跳跃的前提是在站在地面上
		if (!SelfEnemyPawn->GetCharacterMovement() || !SelfEnemyPawn->GetCharacterMovement()->IsWalking())
		{
			FinishExecute(false);
			return EBTNodeResult::Failed;
		}


		TArray<FVector> Points;
		// 判断是否可以直接跳过去？
		if (USpaceFuncLib::GetJumpPoints(Points, PawnLocation, PlayerPawn->GetActorLocation()))
		{
			// 一般下标为 0 是最远的
			// FVector JumpPoint = Points[0];
			// 应该从所有点位里选一个与目标的高度差不超过50的

			TArray<FVector> PointsMaybe;
			for (FVector& P : Points)
			{
				if (FMath::Abs(P.Z - PlayerPawn->GetActorLocation().Z) < 50)
				{
					PointsMaybe.Add(P);
				}
			}

			if (PointsMaybe.IsEmpty())
			{
				FinishExecute(false);
				return EBTNodeResult::Failed;
			}

			FVector JumpPoint = PointsMaybe[FMath::RandRange(0, PointsMaybe.Num() - 1)] + SelfEnemyPawn->GetDefaultHalfHeight();

			if (FMath::Abs(VerticalDistanceToPlayer) < 50)
			{
				SelfEnemyPawn->SetActionMove(JumpPoint - PawnLocation, "JumpHorizontal", SelfEnemyPawn->JumpDuration, true, false, false);
			}
			else if (VerticalDistanceToPlayer >= 50 && VerticalDistanceToPlayer <= 150)
			{
				FHitResult HitResult_Ceil;
				bool bHitCeil = UKismetSystemLibrary::LineTraceSingle(SelfEnemyPawn->GetWorld(), PawnLocation, PawnLocation + FVector(0,0,150),
				TraceTypeQuery1, false, {SelfEnemyPawn},
						EDrawDebugTrace::None, HitResult_Ceil, true,
						FLinearColor::Black, FLinearColor::Gray, 1.0f);
				
				if (!bHitCeil)
				{
					UDebugFuncLab::ScreenMessage("JumpToHigher");
					SelfEnemyPawn->SetActionMove(JumpPoint - PawnLocation, "JumpToHigher", SelfEnemyPawn->JumpDuration, true, false, false);
				}
			}
			else if (VerticalDistanceToPlayer  <= -50 && VerticalDistanceToPlayer >= -150)
			{
				SelfEnemyPawn->SetActionMove(JumpPoint - PawnLocation, "JumpToLower", SelfEnemyPawn->JumpDuration, true, false, false);
			}

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
