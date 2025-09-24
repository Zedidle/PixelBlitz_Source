// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Tips/CommonTipWidget.h"


#include "UI/UIEventCenter.h"
#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"


void UCommonTipWidget::SetContent(const FString& InContent, float InTime)
{
	if(Text_Content)
	{
		Text_Content->SetText(FText::FromString(InContent));
	}

	CacheShowTipTime = InTime;

	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}

	World->GetTimerManager().SetTimer(ShowTipTimerHandle, this, &UCommonTipWidget::RemoveSelf, CacheShowTipTime, false, CacheShowTipTime);
	PlayAnimIn();
}

void UCommonTipWidget::OnTipPlayAnimOutEnd()
{
	if (UUIEventCenter* UIEventCenter = UUIEventCenter::Get(this))
	{
		UIEventCenter->OnCommonTipRemove.Broadcast(this);
	}
}

void UCommonTipWidget::RemoveSelf()
{
	PlayAnimOut();
	
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}

	if(ShowTipTimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(ShowTipTimerHandle);
	}

	if(Anim_out == nullptr)
	{
		return;
	}

	float AnimOutPlayTime = Anim_out->GetEndTime() - Anim_out->GetStartTime();
	World->GetTimerManager().SetTimer(ShowTipTimerHandle, this, &UCommonTipWidget::OnTipPlayAnimOutEnd, AnimOutPlayTime, false, AnimOutPlayTime);
}

void UCommonTipWidget::PlayAnimIn()
{
	if(Anim_in)
	{
		if(IsAnimationPlaying(Anim_out))
		{
			StopAnimation(Anim_out);
		}

		PlayAnimation(Anim_in);
	}
}

void UCommonTipWidget::PlayAnimOut()
{
	if(Anim_out)
	{
		if(IsAnimationPlaying(Anim_in))
		{
			StopAnimation(Anim_in);
		}

		PlayAnimation(Anim_out);
	}
}

