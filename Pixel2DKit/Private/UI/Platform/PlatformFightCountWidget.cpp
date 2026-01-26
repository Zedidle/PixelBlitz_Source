// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Platform/PlatformFightCountWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Subsystems/TimerManagerFuncLib.h"

void UPlatformFightCountWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Show();
}

void UPlatformFightCountWidget::Show()
{
	if (CurNum > 0)
	{
		UTimerManagerFuncLib::CancelDelay(GetWorld(), "UPlatformFightCountWidget::Fade");
		if (AnimShowIn)
		{
			PlayAnimation(AnimShowIn);
		}
	}
}

void UPlatformFightCountWidget::Fade(float Delay)
{
	UTimerManagerFuncLib::SetRetriggerableDelay(GetWorld(), "UPlatformFightCountWidget::Fade",
		[WeakThis = TWeakObjectPtr(this)]
	{
		if (!WeakThis.IsValid()) return;
		if (WeakThis->CurNum > 0)
		{
			if (WeakThis->AnimShowIn)
			{
				WeakThis->PlayAnimation(WeakThis->AnimShowIn, 0.0f, 1, EUMGSequencePlayMode::Reverse);
			}
		}
	}, Delay);
}

void UPlatformFightCountWidget::UpdateCount(int32 Num)
{
	ToNum = Num;
	if (AnimCount)
	{
		PlayAnimation(AnimCount);
	}
}


