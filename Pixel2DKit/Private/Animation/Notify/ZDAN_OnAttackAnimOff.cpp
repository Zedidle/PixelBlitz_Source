// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify//ZDAN_OnAttackAnimOff.h"
#include "Animation/BasePixelAnimInstance.h"
#include "Subsystems/PixelAnimSubsystem.h"

void UZDAN_OnAttackAnimOff::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance)
{
	UPixelAnimSubsystem::SetAnimInstanceProperty(OwningInstance, FName(TEXT("bAttackAnimToggle")), false);
}
