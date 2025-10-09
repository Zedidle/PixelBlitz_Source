// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PXSettingScreen.h"

#include "Input/CommonUIInputTypes.h"
#include "Player/PXLocalPlayer.h"
#include "Settings/PXGameSettingRegistry.h"
#include "UI/UIManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXSettingScreen)

class UGameSettingRegistry;

void UPXSettingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// BackHandle = RegisterUIActionBinding(FBindUIActionArgs(BackInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction)));
	ApplyHandle = RegisterUIActionBinding(FBindUIActionArgs(ApplyInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleApplyAction)));
	CancelChangesHandle = RegisterUIActionBinding(FBindUIActionArgs(CancelChangesInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCancelChangesAction)));
}

UGameSettingRegistry* UPXSettingScreen::CreateRegistry()
{
	UPXGameSettingRegistry* NewRegistry = NewObject<UPXGameSettingRegistry>();

	if (UPXLocalPlayer* LocalPlayer = CastChecked<UPXLocalPlayer>(GetOwningLocalPlayer()))
	{
		NewRegistry->Initialize(LocalPlayer);
	}

	return NewRegistry;
}

bool UPXSettingScreen::NativeOnHandleBackAction()
{
	if (AttemptToPopNavigation())
	{
		return false;
	}

	ApplyChanges();
	
	UUIManager* UIManager = UUIManager::GetSelfInstance(this);
	UIManager->CloseUI(UIName);
	return true;
}

void UPXSettingScreen::HandleApplyAction()
{
	ApplyChanges();
}

void UPXSettingScreen::HandleCancelChangesAction()
{
	CancelChanges();
}

void UPXSettingScreen::OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty)
{
	if (bSettingsDirty)
	{
		if (!GetActionBindings().Contains(ApplyHandle))
		{
			AddActionBinding(ApplyHandle);
		}
		if (!GetActionBindings().Contains(CancelChangesHandle))
		{
			AddActionBinding(CancelChangesHandle);
		}
	}
	else
	{
		RemoveActionBinding(ApplyHandle);
		RemoveActionBinding(CancelChangesHandle);
	}
}
