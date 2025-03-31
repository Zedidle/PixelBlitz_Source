// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AnimNotify//ZDAN_OnAttackAnimOff.h"
#include "Anim/BasePixelAnimInstance.h"

void UZDAN_OnAttackAnimOff::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance)
{
	if (UBasePixelAnimInstance* AnimInst = Cast<UBasePixelAnimInstance>(OwningInstance))
	{
		AnimInst->SetAttackAnimToggle(false);
	}
}
