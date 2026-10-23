// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify//ZDAN_OnAttackAnimOff.h"
#include "Animation/BasePXAnimInstance.h"
#include "Character/BasePXCharacter.h"
#include "Enemy/BaseEnemy.h"

void UZDAN_OnAttackAnimOff::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningInstance)

	if (APaperZDCharacter* PaperZDCharacter = OwningInstance->GetPaperCharacter())
	{
		if (ABasePXCharacter* PixelCharacter = Cast<ABasePXCharacter>(PaperZDCharacter))
		{
			PixelCharacter->SetAttackAnimToggle(false);
		}
		else if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(PaperZDCharacter))
		{
			Enemy->SetAttackAnimToggle(false);
		}
	}
}
