// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Tips/CommonTipWidget.h"
#include "UI/UIEventCenter.h"
#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Subsystems/TimerSubsystemFuncLib.h"


void UCommonTipWidget::SetContent(const FString& InContent, float InTime)
{
	if(Text_Content)
	{
		Text_Content->SetText(FText::FromString(InContent));
	}

	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	PlayAnimIn();
	
	UTimerSubsystemFuncLib::SetDelay(World, [WeakThis = TWeakObjectPtr<ThisClass>(this)]
	{
		if (!WeakThis.IsValid()) return;
		WeakThis->PlayAnimOut();
	}, InTime);

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Anim_out)
	float AnimOutPlayTime = Anim_out->GetEndTime() - Anim_out->GetStartTime();
	
	if (UUIEventCenter* UIEventCenter = UUIEventCenter::Get(this))
	{
		float NewEndTime = World->TimeSeconds + AnimOutPlayTime + InTime;
		UIEventCenter->OnCommonTipAnimOutEnd.Broadcast( NewEndTime );
	}
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

