// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Tips/CommonTipContainerWidget.h"
#include "UI/Tips/CommonTipWidget.h"
#include "UI/UIEventCenter.h"
#include "Components/ScrollBox.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Subsystems/TimerSubsystemFuncLib.h"


void UCommonTipContainerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UUIEventCenter* UIEventCenter = UUIEventCenter::Get(this))
	{
		UIEventCenter->OnCommonTipAnimOutEnd.AddDynamic(this, &UCommonTipContainerWidget::OnCommonTipRemoveAll);
	}
}

void UCommonTipContainerWidget::NativeDestruct()
{
	if (UUIEventCenter* UIEventCenter = UUIEventCenter::Get(this))
	{
		UIEventCenter->OnCommonTipAnimOutEnd.RemoveDynamic(this, &UCommonTipContainerWidget::OnCommonTipRemoveAll);
	}
	
	Super::NativeDestruct();
}

void UCommonTipContainerWidget::AddTip(const FString& Content, float Time)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ScrollBox_Tips)

	APlayerController* OwningPlayer = GetOwningPlayer();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningPlayer)

	UCommonTipWidget* NewCommonTipWidget = CreateWidget<UCommonTipWidget>(OwningPlayer, CommonTipClass);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(NewCommonTipWidget)
	
	NewCommonTipWidget->SetContent(Content, Time);
	ScrollBox_Tips->AddChild(NewCommonTipWidget);

	while(CacheTipQueue.Num() >= MaxShowCommonTip)
	{
		RemoveTip();
	}

	CacheTipQueue.Add(NewCommonTipWidget);
}

void UCommonTipContainerWidget::RemoveTip()
{
	if(CacheTipQueue.IsValidIndex(0))
	{
		UCommonTipWidget* CommonTipWidget = CacheTipQueue[0];
		if(IsValid(CommonTipWidget))
		{
			CommonTipWidget->RemoveFromParent();
		}

		CacheTipQueue.RemoveAt(0);
	}
}

void UCommonTipContainerWidget::OnCommonTipRemoveAll(float NewEndTime)
{
	if (CurEndTime > NewEndTime) return;

	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);

	CurEndTime = NewEndTime;

	UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), "UCommonTipContainerWidget::OnCommonTipRemoveAll",
	[WeakThis = TWeakObjectPtr(this)]
	{
		if (!WeakThis.IsValid()) return;

		if (WeakThis->CacheTipQueue.IsEmpty()) return;

		for (auto& Tip : WeakThis->CacheTipQueue)
		{
			if (!Tip) continue;

			Tip->RemoveFromParent();
		}

		WeakThis->CacheTipQueue.Empty();
		
	}, NewEndTime - World->TimeSeconds);
}

