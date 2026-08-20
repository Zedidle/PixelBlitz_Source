// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notify/ZDAN_OnAttackCombo.h"
#include "PaperZDAnimInstance.h"
#include "PaperZDCharacter.h"
#include "Pixel2DKit.h"
#include "Subsystems/PXAnimSubsystem.h"

void UZDAN_OnAttackCombo::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningInstance)
	UPXAnimSubsystem::SetAnimInstanceProperty(OwningInstance, FName(TEXT("AttackComboIndex")), NextComboIndex);
}
