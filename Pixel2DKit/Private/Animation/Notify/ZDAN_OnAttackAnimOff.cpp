// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify//ZDAN_OnAttackAnimOff.h"
#include "Animation/BasePixelAnimInstance.h"
#include "Character/BasePixelCharacter.h"
#include "Subsystems/PixelAnimSubsystem.h"

void UZDAN_OnAttackAnimOff::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance)
{
	Super::OnReceiveNotify_Implementation(OwningInstance);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningInstance)

	if (APaperZDCharacter* PaperZDCharacter = OwningInstance->GetPaperCharacter())
	{
		if (ABasePixelCharacter* PixelCharacter = Cast<ABasePixelCharacter>(PaperZDCharacter))
		{
			PixelCharacter->SetAttackAnimToggle(false);
		}
	}
}
