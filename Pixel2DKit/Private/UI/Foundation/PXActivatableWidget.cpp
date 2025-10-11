// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/PXActivatableWidget.h"
#include "Editor/WidgetCompilerLog.h"
#include "UI/UIManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXActivatableWidget)

#define LOCTEXT_NAMESPACE "PX"

void UPXActivatableWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPXActivatableWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

bool UPXActivatableWidget::NativeOnHandleBackAction()
{
	if (bIsBackHandler)
	{
		if (!BP_OnHandleBackAction())
		{
			Close();
		}
		return true;
	}
	return false;
}
void UPXActivatableWidget::Close()
{
	UUIManager* UIManager = UUIManager::GetSelfInstance(this);
	if (UIManager)
	{
		UIManager->CloseUI(UIName);
	}
}

TOptional<FUIInputConfig> UPXActivatableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case EPXWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case EPXWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case EPXWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	case EPXWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}


#undef LOCTEXT_NAMESPACE
