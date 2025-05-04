// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/ZDAN_OnRemoteAttackEnd.h"

#include "PaperZDCharacter.h"
#include "Interfaces/Fight_Interface.h"
#include "Pixel2DKit/Pixel2DKit.h"

void UZDAN_OnRemoteAttackEnd::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance)
{
	Super::OnReceiveNotify_Implementation(OwningInstance);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningInstance)

	if (APaperZDCharacter* PaperZDCharacter = OwningInstance->GetPaperCharacter())
	{
		if (PaperZDCharacter->Implements<UFight_Interface>())
		{
			IFight_Interface::Execute_OnRemoteAttackEffect(PaperZDCharacter);
		}
	}
}
