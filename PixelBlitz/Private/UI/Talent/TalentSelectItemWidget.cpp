// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Talent/TalentSelectItemWidget.h"

#include <string>

#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "UI/Talent/TalentTipWidget.h"

void UTalentSelectItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TalentTag = TalentData.TalentTag;

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

