// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/ComboWeakPointCountWidget.h"

#include "Subsystems/TimerSubsystemFuncLib.h"

void UComboWeakPointCountWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UComboWeakPointCountWidget::Fade(float Delay)
{
	UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), "UComboWeakPointCountWidget::Fade",
		[WeakThis = TWeakObjectPtr(this)]
	{
		if (!WeakThis.IsValid()) return;

		WeakThis->CurNum = 0;
		WeakThis->CurDamagePlusPercent = 0;
			
		if (WeakThis->AnimCount)
		{
			WeakThis->StopAnimation(WeakThis->AnimCount);
		}
		
		if (WeakThis->AnimShowIn)
		{
			WeakThis->PlayAnimation(WeakThis->AnimShowIn, 0.0f, 1, EUMGSequencePlayMode::Reverse);
		}
	}, Delay);
}


void UComboWeakPointCountWidget::UpdateCount(int32 Num, int32 DamagePlusPercent)
{
	CurNum = Num;
	CurDamagePlusPercent = DamagePlusPercent;
	
	if (AnimCount)
	{
		PlayAnimation(AnimCount);
	}
}
