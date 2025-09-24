// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Tips/CommonTipContainerWidget.h"
#include "UI/Tips/CommonTipWidget.h"
#include "UI/UIEventCenter.h"
#include "Components/ScrollBox.h"


void UCommonTipContainerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UUIEventCenter* UIEventCenter = UUIEventCenter::Get(this))
	{
		UIEventCenter->OnCommonTipRemove.AddDynamic(this, &UCommonTipContainerWidget::OnCommonTipRemove);
	}
}

void UCommonTipContainerWidget::NativeDestruct()
{
	if (UUIEventCenter* UIEventCenter = UUIEventCenter::Get(this))
	{
		UIEventCenter->OnCommonTipRemove.RemoveDynamic(this, &UCommonTipContainerWidget::OnCommonTipRemove);
	}
	
	Super::NativeDestruct();
}

void UCommonTipContainerWidget::AddTip(const FString& Content, float Time)
{
	if(ScrollBox_Tips == nullptr)
	{
		return;
	}

	APlayerController* OwningPlayer = GetOwningPlayer();
	if(OwningPlayer == nullptr)
	{
		return;
	}

	UCommonTipWidget* NewCommonTipWidget = CreateWidget<UCommonTipWidget>(OwningPlayer, CommonTipClass);
	if(NewCommonTipWidget == nullptr)
	{
		return;
	}

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
			UWorld* World = GetWorld();
			if(World == nullptr)
			{
				return;
			}

			if(CommonTipWidget->ShowTipTimerHandle.IsValid())
			{
				World->GetTimerManager().ClearTimer(CommonTipWidget->ShowTipTimerHandle);
			}
			
			CommonTipWidget->RemoveFromParent();
		}

		CacheTipQueue.RemoveAt(0);
	}
}

void UCommonTipContainerWidget::OnCommonTipRemove(UCommonTipWidget* TipWidget)
{
	if(CacheTipQueue.Find(TipWidget))
	{
		if(IsValid(TipWidget))
		{
			UWorld* World = GetWorld();
			if(World == nullptr)
			{
				return;
			}

			if(TipWidget->ShowTipTimerHandle.IsValid())
			{
				World->GetTimerManager().ClearTimer(TipWidget->ShowTipTimerHandle);
			}
			
			TipWidget->RemoveFromParent();
		}

		CacheTipQueue.Remove(TipWidget);
	}
}
