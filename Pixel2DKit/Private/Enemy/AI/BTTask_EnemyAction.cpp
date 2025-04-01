// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask_EnemyAction.h"
#include "AIController.h"
#include "Anim/BasePixelAnimInstance.h"
#include "PaperZDAnimationComponent.h"
#include "Enemy/Components/EnemyAIComponent.h"
#include "Interfaces/Enemy/AI/EnemyAI_Interface.h"

EBTNodeResult::Type UBTTask_EnemyAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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


	if (UEnemyAIComponent* EnemyAIComponent = Pawn->GetComponentByClass<UEnemyAIComponent>())
	{
		AActor* Player = EnemyAIComponent->PixelCharacter;
		if (!IsValid(Player))
		{
			FinishAbort();
			return EBTNodeResult::Failed;
		}
		float distance = (Player->GetActorLocation() - Pawn->GetActorLocation()).Size2D();
		EActionField ActionField = EnemyAIComponent->GetActionFieldByPlayer();

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow,
FString::Printf(TEXT("UBTTask_EnemyAction %d, %d"), ActionField ,__LINE__));
		
		
		// 设置行动区间
		if (UPaperZDAnimationComponent* AnimInstanceComp = Pawn->GetComponentByClass<UPaperZDAnimationComponent>())
		{
			if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(AnimInstanceComp->GetAnimInstance()))
			AnimInst->SetActionField(ActionField);
		}
		
		if (ActionField == EastNear)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerEastNear(Pawn, distance);
		}
		if (ActionField == EastMid)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerEastMid(Pawn, distance);
		}
		if (ActionField == EastFar)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerEastFar(Pawn, distance);
		}
		if (ActionField == EastRemote)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerEastRemote(Pawn, distance);
		}

		if (ActionField == WestNear)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerWestNear(Pawn, distance);
		}
		if (ActionField == WestMid)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerWestMid(Pawn, distance);
		}
		if (ActionField == WestFar)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerWestFar(Pawn, distance);
		}
		if (ActionField == WestRemote)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerWestRemote(Pawn, distance);
		}

		if (ActionField == NorthNear)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerNorthNear(Pawn, distance);
		}
		if (ActionField == NorthMid)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerNorthMid(Pawn, distance);
		}
		if (ActionField == NorthFar)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerNorthFar(Pawn, distance);
		}
		if (ActionField == NorthRemote)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerNorthRemote(Pawn, distance);
		}

		if (ActionField == SouthNear)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerSouthNear(Pawn, distance);
		}
		if (ActionField == SouthMid)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerSouthMid(Pawn, distance);
		}
		if (ActionField == SouthFar)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerSouthFar(Pawn, distance);
		}
		if (ActionField == SouthRemote)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerSouthRemote(Pawn, distance);
		}
		
		FinishExecute(true);
	}
	else
	{
		FinishExecute(false);
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
