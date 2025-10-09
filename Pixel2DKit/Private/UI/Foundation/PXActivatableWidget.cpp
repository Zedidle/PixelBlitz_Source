// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/PXActivatableWidget.h"

#include "UI/UIManager.h"

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
			UUIManager* UIManager = UUIManager::GetSelfInstance(this);
			UIManager->CloseUI(UIName);
		}
		return true;
	}
	return false;
}
