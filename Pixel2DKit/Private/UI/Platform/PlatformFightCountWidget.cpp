// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Platform/PlatformFightCountWidget.h"
#include "Animation/WidgetAnimation.h"

void UPlatformFightCountWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Show();
}

void UPlatformFightCountWidget::Show()
{
	if (CurNum > 0)
	{
		bIn = true;
		if (AnimShowIn)
		{
			PlayAnimation(AnimShowIn);
		}
	}
}

void UPlatformFightCountWidget::Fade()
{
	if (bIn) return;
	if (CurNum > 0)
	{
		if (AnimShowIn)
		{
			PlayAnimation(AnimShowIn, 0.0f, 1, EUMGSequencePlayMode::Reverse);
		}
	}
}

void UPlatformFightCountWidget::UpdateCount(int32 Num)
{
	ToNum = Num;
	if (AnimCount)
	{
		PlayAnimation(AnimCount);
	}
}


