// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/PXHUDLayout.h"

#include "CommonUIExtensions.h"
#include "Input/CommonUIInputTypes.h"
#include "NativeGameplayTags.h"
#include "Controller/PXPlayerController.h"
#include "Core/PXGameState.h"
#include "UI/UIManager.h"
#include "UI/Foundation/PXActivatableWidget.h"
#include "Utilitys/PXGameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXHUDLayout)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_ACTION_ESCAPE, "UI.Action.Escape");
UPXHUDLayout::UPXHUDLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPXHUDLayout::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterUIActionBinding(FBindUIActionArgs(FUIActionTag::ConvertChecked(TAG_UI_ACTION_ESCAPE), false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleEscapeAction)));
}

void UPXHUDLayout::HandleEscapeAction()
{
	if (ensure(!EscapeWidgetName.IsNone()))
	{
		APXPlayerController* PC = UPXGameplayStatics::GetPlayerController(this);
		if (PC && PC->CanPause())
		{
			// 使用自定义的 UIManager进行界面处理
			if (UUIManager* UIManager = UUIManager::GetSelfInstance(this))
			{
				UIManager->OpenUI(EscapeWidgetName);
			}
		}
	}
}
