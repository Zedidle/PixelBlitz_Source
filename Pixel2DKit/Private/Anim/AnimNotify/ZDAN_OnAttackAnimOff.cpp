// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AnimNotify//ZDAN_OnAttackAnimOff.h"
#include "Anim/BasePixelAnimInstance.h"
#include "Subsystems/PixelAnimSubsystem.h"

void UZDAN_OnAttackAnimOff::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance)
{
	UPixelAnimSubsystem::SetAnimInstanceProperty(OwningInstance, FName(TEXT("bAttackAnimToggle")), false);
}
