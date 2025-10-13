// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Buff/BuffStateWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/KismetStringLibrary.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "UI/Buff/BuffPermanentFloatingTextWidget.h"


void UBuffStateWidget::BuffIn_Implementation(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor)
{
	if (UBuffFloatingTextWidget* Widget = Tag2Widget_In.FindRef(Tag))
	{
		if (Widget->BuffName != BuffName)
		{
			Widget->SetBuffName(BuffName);
			Widget->PlayIn();
		}
		return;
	}
	
	// BuffFloatingText 相关
	if (BuffFloatingText_WidgetClass)
	{
		TObjectPtr<UBuffFloatingTextWidget> BuffFloatingTextWidget = CreateWidget<UBuffFloatingTextWidget>(this, BuffFloatingText_WidgetClass);
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffFloatingTextWidget);
		
		BuffFloatingTextWidget->InitializeData( BuffName, TextColor,1.0f,true);

		if (VerticalBox_BuffIn)
		{
			if (UVerticalBoxSlot* VerticalBoxSlot = VerticalBox_BuffIn->AddChildToVerticalBox(BuffFloatingTextWidget))
			{
				VerticalBoxSlot->SetHorizontalAlignment(HAlign_Right);
				Tag2Widget_In.Add(Tag, BuffFloatingTextWidget);
			}
		}
	}
}

void UBuffStateWidget::BuffOut_Implementation(FGameplayTag Tag)
{
	UBuffFloatingTextWidget* Widget = Tag2Widget_In.FindRef(Tag);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Widget);

	Widget->RemoveFromParent();
	Tag2Widget_In.Remove(Tag);

	Widget->In = false;

	if (VerticalBox_BuffOut)
	{
		if (UVerticalBoxSlot* VerticalBoxSlot = VerticalBox_BuffOut->AddChildToVerticalBox(Widget))
		{
			VerticalBoxSlot->SetHorizontalAlignment(HAlign_Right);
			Tag2Widget_Out.Add(Tag, Widget);
		}
	}
}


void UBuffStateWidget::BuffPermanent_Implementation(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor)
{
	if (Tag2Widget_Permanent.Contains(Tag)) return;

	if (BuffFloatingTextPermanent_WidgetClass)
	{
		if (UBuffPermanentFloatingTextWidget* Widget = CreateWidget<UBuffPermanentFloatingTextWidget>(this, BuffFloatingTextPermanent_WidgetClass))
		{
			Widget->InitializeData(BuffName,TextColor);

			if (VerticalBox_BuffPermanent)
			{
				VerticalBox_BuffPermanent->AddChildToVerticalBox(Widget)->SetHorizontalAlignment(HAlign_Left);
				Tag2Widget_Permanent.Add(Tag, Widget);
			}
		}
	}
}



void UBuffStateWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(VerticalBox_BuffIn)
	VerticalBox_BuffIn->ClearChildren();

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(VerticalBox_BuffOut)
	VerticalBox_BuffOut->ClearChildren();

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(VerticalBox_BuffPermanent)
	VerticalBox_BuffPermanent->ClearChildren();

	
}

void UBuffStateWidget::NativeDestruct()
{
	Super::NativeDestruct();
	for (auto& Pair : Tag2Widget_In)
	{
		Pair.Value->RemoveFromParent();
	}
	for (auto& Pair : Tag2Widget_Out)
	{
		Pair.Value->RemoveFromParent();
	}
	for (auto& Pair : Tag2Widget_Permanent)
	{
		Pair.Value->RemoveFromParent();
	}
	Tag2Widget_In.Empty();
	Tag2Widget_Out.Empty();
	Tag2Widget_Permanent.Empty();
}

void UBuffStateWidget::BuffExpire_Implementation(FGameplayTag Tag)
{
	UBuffFloatingTextWidget* BuffFloatingTextWidget = Tag2Widget_In.FindRef(Tag);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffFloatingTextWidget);
	
	BuffFloatingTextWidget->DispearOut();
}
