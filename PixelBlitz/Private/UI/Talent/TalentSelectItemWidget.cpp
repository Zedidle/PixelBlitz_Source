// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Talent/TalentSelectItemWidget.h"

#include <string>

#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Subsystems/DataTableSubsystem.h"
#include "UI/Talent/TalentTipWidget.h"
#include "Widgets/Input/SButton.h"

namespace
{
	void FindInternalSButton(const TSharedRef<const SWidget>& InWidget, TWeakPtr<SButton>& Out)
	{
		const FName Type = InWidget->GetType();
		if (Type == FName("SButton") || Type == FName("SCommonButton"))
		{
			Out = StaticCastSharedRef<SButton>(ConstCastSharedRef<SWidget>(InWidget));
			return;
		}

		// GetChildren() 为非 const 接口，此处仅做只读遍历
		const FChildren* Children = ConstCastSharedRef<SWidget>(InWidget)->GetChildren();
		for (int32 i = 0; i < Children->Num(); ++i)
		{
			FindInternalSButton(Children->GetChildAt(i), Out);
			if (Out.IsValid())
			{
				return;
			}
		}
	}
}

void UTalentSelectItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TalentTag = TalentData.TalentTag;

	// 天赋块选择色泽基于 BuffOnWidget 表
	if (UDataTableSubsystem* DataTableSubsystem = UDataTableSubsystem::GetInstance(this))
	{
		if (const FBuffOnWidget* BuffData = DataTableSubsystem->GetBuffOnWidgetDataByTag(TalentData.TalentTag))
		{
			TalentColor = BuffData->Color;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("TalentSelectItemWidget: BuffOnWidget 表未找到 Tag: %s，使用默认颜色"), *TalentData.TalentTag.ToString());
		}
	}

	ApplyTalentBGColor();

	if (TextBlock_TalentName)
	{
		TextBlock_TalentName->SetText(TalentData.TalentName);
	}

	if (TextBlock_Price)
	{
		TextBlock_Price->SetText(FText::AsNumber(TalentData.Price));
	}
	
	if (Image_TalentIcon)
	{
		if (UTexture2D* IconImage = TalentData.Icon.LoadSynchronous())
		{
			Image_TalentIcon->SetBrushFromTexture(IconImage);
		}
	}
	
	if (UPXBasicBuildSaveGame* SG = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(GetWorld()))
	{
		if (SG->ChosenTalents.Contains(TalentData.TalentTag))
		{
			SetSelectedInternal(true, false, false);
		}
	}

	OnHovered().AddUObject(this, &ThisClass::Event_OnHovered);
	OnUnhovered().AddUObject(this, &ThisClass::Event_OnUnhovered);
}

void UTalentSelectItemWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (TalentTipWidget)
	{
		TalentTipWidget->RemoveFromParent();
	}
}


void UTalentSelectItemWidget::Event_OnHovered()
{
	if (TalentTipWidgetClass)
	{
		TalentTipWidget = CreateWidget<UTalentTipWidget>(GetWorld(), TalentTipWidgetClass);
		if (TalentTipWidget)
		{
			TalentTipWidget->SetTalentData(TalentData);
			TalentTipWidget->AddToViewport(100);
		}
	}
}

void UTalentSelectItemWidget::Event_OnUnhovered()
{
	if (TalentTipWidget)
	{
		TalentTipWidget->RemoveFromParent();
	}
}

void UTalentSelectItemWidget::ApplyTalentBGColor()
{
	if (TSharedPtr<SButton> Button = CachedSButton.Pin())
	{
		// 选中态乘上表颜色，未选中保持白色（不改变原样式 tint）；只作用于背景画刷，文字不受影响
		Button->SetBorderBackgroundColor(GetSelected() ? FSlateColor(TalentColor) : FSlateColor(FLinearColor::White));
	}
}

TSharedRef<SWidget> UTalentSelectItemWidget::RebuildWidget()
{
	TSharedRef<SWidget> RootWidget = Super::RebuildWidget();

	CachedSButton.Reset();
	FindInternalSButton(RootWidget, CachedSButton);

	return RootWidget;
}

void UTalentSelectItemWidget::NativeOnSelected(bool bBroadcast)
{
	Super::NativeOnSelected(bBroadcast);

	ApplyTalentBGColor();
}

void UTalentSelectItemWidget::NativeOnDeselected(bool bBroadcast)
{
	Super::NativeOnDeselected(bBroadcast);

	ApplyTalentBGColor();
}

