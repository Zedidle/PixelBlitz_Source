// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask_EnemyAttack.h"
#include "AIController.h"
#include "Anim/BasePixelAnimInstance.h"
#include "PaperZDAnimationComponent.h"
#include "Enemy/Components/EnemyAIComponent.h"
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

		// 设置行动区间
		if (UPaperZDAnimationComponent* AnimInstanceComp = Pawn->GetComponentByClass<UPaperZDAnimationComponent>())
		{
			if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(AnimInstanceComp->GetAnimInstance()))
			AnimInst->SetActionField(ActionField);
		}
		
		if (ActionField == EActionField::EastNear)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerEastNear(Pawn, distance);
		}
		if (ActionField == EActionField::EastMid)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerEastMid(Pawn, distance);
		}
		if (ActionField == EActionField::EastFar)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerEastFar(Pawn, distance);
		}
		if (ActionField == EActionField::EastFar)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerEastRemote(Pawn, distance);
		}

		if (ActionField == EActionField::WestNear)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerWestNear(Pawn, distance);
		}
		if (ActionField == EActionField::WestMid)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerWestMid(Pawn, distance);
		}
		if (ActionField == EActionField::WestFar)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerWestFar(Pawn, distance);
		}
		if (ActionField == EActionField::WestFar)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerWestRemote(Pawn, distance);
		}

		if (ActionField == EActionField::NorthNear)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerNorthNear(Pawn, distance);
		}
		if (ActionField == EActionField::NorthMid)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerNorthMid(Pawn, distance);
		}
		if (ActionField == EActionField::NorthFar)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerNorthFar(Pawn, distance);
		}
		if (ActionField == EActionField::NorthFar)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerNorthRemote(Pawn, distance);
		}

		if (ActionField == EActionField::SouthNear)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerSouthNear(Pawn, distance);
		}
		if (ActionField == EActionField::SouthMid)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerSouthMid(Pawn, distance);
		}
		if (ActionField == EActionField::SouthFar)
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerSouthFar(Pawn, distance);
		}
		if (ActionField == EActionField::SouthFar)
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
