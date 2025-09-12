// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Buff/BuffStateWidget.h"

#include "InteractiveToolManager.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/KismetStringLibrary.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Components/VerticalBox.h"
#include "UI/Buff/BuffPermanentFloatingTextWidget.h"


void UBuffStateWidget::BuffIn_Implementation(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor)
{
	UKismetStringLibrary::Concat_StrStr(">", BuffName);
	if (Tag2Widget.Contains(Tag))
	{
		Tag2Widget[Tag]->RemoveFromParent();
	}

	// BuffFloatingText 相关
	if (BuffFloatingText_WidgetClass)
	{
		UBuffFloatingTextWidget* BuffFloatingTextWidget = Cast<UBuffFloatingTextWidget>(CreateWidget(GetWorld(), BuffFloatingText_WidgetClass));
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffFloatingTextWidget);
		
		BuffFloatingTextWidget->InitializeData(
		FText::FromString(UKismetStringLibrary::Concat_StrStr(">", BuffName)),
			TextColor,
			FVector2D::ZeroVector,
			FVector2D(1.0f, 1.0f),
			1.0f,
			true
		);
		Tag2Widget.Add(Tag, BuffFloatingTextWidget);

		if (VerticalBox_BuffIn)
		{
			if (UVerticalBoxSlot* VerticalBoxSlot = VerticalBox_BuffIn->AddChildToVerticalBox(BuffFloatingTextWidget))
			{
				VerticalBoxSlot->SetHorizontalAlignment(HAlign_Right);
			}
		}
	}
}

void UBuffStateWidget::BuffOut_Implementation(FGameplayTag Tag)
{
	if (!Tag2Widget.Contains(Tag)) return;

	UBuffFloatingTextWidget* BuffFloatingTextWidget = Cast<UBuffFloatingTextWidget>(Tag2Widget[Tag]);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffFloatingTextWidget);

	BuffFloatingTextWidget->RemoveFromParent();
	Tag2Widget.Remove(Tag);

	BuffFloatingTextWidget->In = false;

	if (VerticalBox_BuffOut)
	{
		if (UVerticalBoxSlot* VerticalBoxSlot = VerticalBox_BuffOut->AddChildToVerticalBox(BuffFloatingTextWidget))
		{
			VerticalBoxSlot->SetHorizontalAlignment(HAlign_Right);
		}
	}
}


void UBuffStateWidget::BuffPermanent_Implementation(FGameplayTag Tag, const FString& BuffName, FLinearColor TextColor)
{
	if (Tag2Widget.Contains(Tag)) return;

	if (BuffFloatingTextPermanent_WidgetClass)
	{
		if (UBuffPermanentFloatingTextWidget* BuffFloatingTextWidget = Cast<UBuffPermanentFloatingTextWidget>(
	CreateWidget(GetWorld(), BuffFloatingTextPermanent_WidgetClass)))
		{
			BuffFloatingTextWidget->InitializeData(
			FText::FromString(UKismetStringLibrary::Concat_StrStr(BuffName,"*")),
				TextColor,
				FVector2D::ZeroVector,
				FVector2D(1.0f, 1.0f),
				1.0f
			);
			Tag2Widget.Add(Tag, BuffFloatingTextWidget);

			if (VerticalBox_BuffPermanent)
			{
				VerticalBox_BuffPermanent->AddChildToVerticalBox(BuffFloatingTextWidget)->SetHorizontalAlignment(HAlign_Left);
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
}

void UBuffStateWidget::BuffDispear_Implementation(FGameplayTag Tag)
{
	if (!Tag2Widget.Contains(Tag)) return;

	UBuffFloatingTextWidget* BuffFloatingTextWidget = Cast<UBuffFloatingTextWidget>(Tag2Widget[Tag]);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BuffFloatingTextWidget);
	BuffFloatingTextWidget->DispearOut();
}
