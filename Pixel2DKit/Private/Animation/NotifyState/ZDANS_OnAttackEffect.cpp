// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/NotifyState/ZDANS_OnAttackEffect.h"
#include "PaperZDCharacter.h"
#include "Interfaces/Fight_Interface.h"
#include "Pixel2DKit/Pixel2DKit.h"

void UZDANS_OnAttackEffect::OnNotifyBegin_Implementation(UPaperZDAnimInstance* OwningInstance)
{
	Super::OnNotifyBegin_Implementation(OwningInstance);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningInstance)

	if (APaperZDCharacter* PaperZDCharacter = OwningInstance->GetPaperCharacter())
	{
		if (PaperZDCharacter->Implements<UFight_Interface>())
		{
			IFight_Interface::Execute_OnAttackEffectBegin(PaperZDCharacter);
		}
	}
}

void UZDANS_OnAttackEffect::OnNotifyEnd_Implementation(UPaperZDAnimInstance* OwningInstance)
{
	Super::OnNotifyEnd_Implementation(OwningInstance);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningInstance)

	if (APaperZDCharacter* PaperZDCharacter = OwningInstance->GetPaperCharacter())
	{
		if (PaperZDCharacter->Implements<UFight_Interface>())
		{
			IFight_Interface::Execute_OnAttackEffectEnd(PaperZDCharacter);
		}
	}
}
