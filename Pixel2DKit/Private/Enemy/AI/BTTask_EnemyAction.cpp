// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/AI/BTTask_EnemyAction.h"
#include "AIController.h"
#include "Animation/BasePXAnimInstance.h"
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
		FGameplayTag ActionField = EnemyAIComponent->GetActionFieldByPlayer();

// 		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow,
// FString::Printf(TEXT("UBTTask_EnemyAction Field %s  %s, %d"), *Pawn->GetName(), *ActionField.ToString() ,__LINE__));
// 		
		
		// 设置行动区间
		if (UPaperZDAnimationComponent* AnimInstanceComp = Pawn->GetComponentByClass<UPaperZDAnimationComponent>())
		{
			if (UBasePXAnimInstance* AnimInst = Cast<UBasePXAnimInstance>(AnimInstanceComp->GetAnimInstance()))
			AnimInst->SetActionField(ActionField);
		}

		if (ActionField.GetTagName() == "ActionField.East.Near")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerEastNear(Pawn, distance);
		}
		if (ActionField.GetTagName() == "ActionField.East.Mid")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerEastMid(Pawn, distance);
		}
		if (ActionField.GetTagName() == "ActionField.East.Far")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerEastFar(Pawn, distance);
		}
		if (ActionField.GetTagName() == "ActionField.East.Remote")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerEastRemote(Pawn, distance);
		}

		if (ActionField.GetTagName() == "ActionField.West.Near")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerWestNear(Pawn, distance);
		}
		if (ActionField.GetTagName() == "ActionField.West.Mid")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerWestMid(Pawn, distance);
		}
		if (ActionField.GetTagName() == "ActionField.West.Far")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerWestFar(Pawn, distance);
		}
		if (ActionField.GetTagName() == "ActionField.West.Remote")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerWestRemote(Pawn, distance);
		}

		if (ActionField.GetTagName() == "ActionField.North.Near")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerNorthNear(Pawn, distance);
		}
		if (ActionField.GetTagName() == "ActionField.North.Mid")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerNorthMid(Pawn, distance);
		}
		if (ActionField.GetTagName() == "ActionField.North.Far")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerNorthFar(Pawn, distance);
		}
		if (ActionField.GetTagName() == "ActionField.North.Remote")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerNorthRemote(Pawn, distance);
		}

		if (ActionField.GetTagName() == "ActionField.South.Near")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerSouthNear(Pawn, distance);
		}
		if (ActionField.GetTagName() == "ActionField.South.Mid")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerSouthMid(Pawn, distance);
		}
		if (ActionField.GetTagName() == "ActionField.South.Far")
		{
			IEnemyAI_Interface::Execute_ActionAtPlayerSouthFar(Pawn, distance);
		}
		if (ActionField.GetTagName() == "ActionField.South.Remote")
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
