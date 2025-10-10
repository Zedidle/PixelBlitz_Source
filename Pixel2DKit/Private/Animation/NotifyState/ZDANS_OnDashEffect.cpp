// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyState/ZDANS_OnDashEffect.h"

#include "PaperZDAnimInstance.h"
#include "PaperZDCharacter.h"
#include "Interfaces/Fight_Interface.h"
#include "Pixel2DKit/Pixel2DKit.h"

void UZDANS_OnDashEffect::OnNotifyBegin_Implementation(UPaperZDAnimInstance* OwningInstance) const
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningInstance)

	if (APaperZDCharacter* PaperZDCharacter = OwningInstance->GetPaperCharacter())
	{
		if (PaperZDCharacter->Implements<UFight_Interface>())
		{
			IFight_Interface::Execute_OnDashEffectBegin(PaperZDCharacter);
		}
	}
}

void UZDANS_OnDashEffect::OnNotifyEnd_Implementation(UPaperZDAnimInstance* OwningInstance) const
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningInstance)

	if (APaperZDCharacter* PaperZDCharacter = OwningInstance->GetPaperCharacter())
	{
		if (PaperZDCharacter->Implements<UFight_Interface>())
		{
			IFight_Interface::Execute_OnDashEffectEnd(PaperZDCharacter);
		}
	}
}
