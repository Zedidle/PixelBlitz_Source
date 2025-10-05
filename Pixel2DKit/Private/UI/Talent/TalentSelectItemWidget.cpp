// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Talent/TalentSelectItemWidget.h"

#include "CommonTextBlock.h"
#include "DelayAction.h"
#include "MeshAttributes.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBoxSlot.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "UI/Talent/TalentTipWidget.h"

void UTalentSelectItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Text_TalentName)
	{
		Text_TalentName->SetText(FText::AsCultureInvariant(*FString::Printf(TEXT("%d> %s"), TalentData.Price, *TalentData.TalentName.ToString())));
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
}


void UTalentSelectItemWidget::Event_OnHovered()
{
	if (TalentTipWidgetClass)
	{
		TalentTipWidget = CreateWidget<UTalentTipWidget>(GetWorld(), TalentTipWidgetClass);
		if (TalentTipWidget)
		{
			TalentTipWidget->SetTalentData(TalentData);
			if (GEngine && GEngine->GameViewport)
			{
			
				FGeometry Geometry = GetCachedGeometry();
				FVector2D AbsolutePosition = Geometry.GetAbsolutePosition();
				FVector2D ViewportPos, PixelPos;
				USlateBlueprintLibrary::AbsoluteToViewport(this, AbsolutePosition, PixelPos, ViewportPos);
				
				FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);
				float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
				FVector2D RealViewportSize = ViewportSize / ViewportScale;

				ViewportPos.X += TipOffset.X / ViewportScale;
				ViewportPos.Y = FMath::Min(RealViewportSize.Y + TipOffset.Y / ViewportScale, RealViewportSize.Y);
				
				TalentTipWidget->SetPositionInViewport(ViewportPos);
			}

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

