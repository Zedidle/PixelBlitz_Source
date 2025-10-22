// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/ZDAN_ActionMove.h"

#include "PaperZDAnimInstance.h"
#include "Pixel2DKit.h"
#include "Enemy/BaseEnemy.h"


void UZDAN_ActionMove::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningInstance)
	
	if (APaperZDCharacter* PaperZDCharacter = OwningInstance->GetPaperCharacter())
	{
		if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(PaperZDCharacter))
		{
			
			FVector MoveVector;
			if (EActionMoveDirectionType::FromSelf == ActionMoveDirectionType)
			{
				MoveVector = FRotator(0, DirectionYaw, 0).RotateVector(Enemy->GetHorizontalDirectionToPlayer() * Distance);
				Enemy->SetActionMove(MoveVector, CurveName, SustainTime, bFloat, bInterrupt);
			}
			else if (EActionMoveDirectionType::FromTarget == ActionMoveDirectionType)
			{
				AActor* Target = IFight_Interface::Execute_GetTarget(Enemy);
				CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Target)
				MoveVector = FRotator(0, DirectionYaw, 0).RotateVector(- Enemy->GetHorizontalDirectionToPlayer() * Distance);
				MoveVector = Target->GetActorLocation() + MoveVector - Enemy->GetActorLocation();
				
				Enemy->SetActionMove(MoveVector, CurveName, SustainTime, bFloat, bInterrupt);
			}

		}
	}
}
