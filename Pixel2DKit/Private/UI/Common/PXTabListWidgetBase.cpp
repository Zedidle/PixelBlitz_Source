// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/PXTabListWidgetBase.h"

#include "CommonAnimatedSwitcher.h"
#include "CommonButtonBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXTabListWidgetBase)

void UPXTabListWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UPXTabListWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	SetupTabs();
}

void UPXTabListWidgetBase::NativeDestruct()
{
	for (FPXTabDescriptor& TabInfo : PreregisteredTabInfoArray)
	{
		if (TabInfo.CreatedTabContentWidget)
		{
			TabInfo.CreatedTabContentWidget->RemoveFromParent();
			TabInfo.CreatedTabContentWidget = nullptr;
		}
	}

	Super::NativeDestruct();
}

bool UPXTabListWidgetBase::GetPreregisteredTabInfo(const FName TabNameId, FPXTabDescriptor& OutTabInfo)
{
	const FPXTabDescriptor* const FoundTabInfo = PreregisteredTabInfoArray.FindByPredicate([&](FPXTabDescriptor& TabInfo) -> bool
	{
		return TabInfo.TabId == TabNameId;
	});

	if (!FoundTabInfo)
	{
		return false;
	}

	OutTabInfo = *FoundTabInfo;
	return true;
}

void UPXTabListWidgetBase::SetTabHiddenState(FName TabNameId, bool bHidden)
{
	for (FPXTabDescriptor& TabInfo : PreregisteredTabInfoArray)
	{
		if (TabInfo.TabId == TabNameId)
		{
			TabInfo.bHidden = bHidden;
			break;
		}
	}
}

bool UPXTabListWidgetBase::RegisterDynamicTab(const FPXTabDescriptor& TabDescriptor)
{
	// If it's hidden we just ignore it.
	if (TabDescriptor.bHidden)
	{
		return true;
	}
	
	PendingTabLabelInfoMap.Add(TabDescriptor.TabId, TabDescriptor);

	return RegisterTab(TabDescriptor.TabId, TabDescriptor.TabButtonType, TabDescriptor.CreatedTabContentWidget);
}

void UPXTabListWidgetBase::HandlePreLinkedSwitcherChanged()
{
	for (const FPXTabDescriptor& TabInfo : PreregisteredTabInfoArray)
	{
		// Remove tab content widget from linked switcher, as it is being disassociated.
		if (TabInfo.CreatedTabContentWidget)
		{
			TabInfo.CreatedTabContentWidget->RemoveFromParent();
		}
	}

	Super::HandlePreLinkedSwitcherChanged();
}

void UPXTabListWidgetBase::HandlePostLinkedSwitcherChanged()
{
	if (!IsDesignTime() && GetCachedWidget().IsValid())
	{
		// Don't bother making tabs if we're in the designer or haven't been constructed yet
		SetupTabs();
	}

	Super::HandlePostLinkedSwitcherChanged();
}

void UPXTabListWidgetBase::HandleTabCreation_Implementation(FName TabId, UCommonButtonBase* TabButton)
{
	FPXTabDescriptor* TabInfoPtr = nullptr;
	
	FPXTabDescriptor TabInfo;
	if (GetPreregisteredTabInfo(TabId, TabInfo))
	{
		TabInfoPtr = &TabInfo;
	}
	else
	{
		TabInfoPtr = PendingTabLabelInfoMap.Find(TabId);
	}
	
	if (TabButton->GetClass()->ImplementsInterface(UPXTabButtonInterface::StaticClass()))
	{
		if (ensureMsgf(TabInfoPtr, TEXT("A tab button was created with id %s but no label info was specified. RegisterDynamicTab should be used over RegisterTab to provide label info."), *TabId.ToString()))
		{
			IPXTabButtonInterface::Execute_SetTabLabelInfo(TabButton, *TabInfoPtr);
		}
	}

	PendingTabLabelInfoMap.Remove(TabId);
}

bool UPXTabListWidgetBase::IsFirstTabActive() const
{
	if (PreregisteredTabInfoArray.Num() > 0)
	{
		return GetActiveTab() == PreregisteredTabInfoArray[0].TabId;
	}

	return false;
}

bool UPXTabListWidgetBase::IsLastTabActive() const
{
	if (PreregisteredTabInfoArray.Num() > 0)
	{
		return GetActiveTab() == PreregisteredTabInfoArray.Last().TabId;
	}

	return false;
}

bool UPXTabListWidgetBase::IsTabVisible(FName TabId)
{
	if (const UCommonButtonBase* Button = GetTabButtonBaseByID(TabId))
	{
		const ESlateVisibility TabVisibility = Button->GetVisibility();
		return (TabVisibility == ESlateVisibility::Visible
			|| TabVisibility == ESlateVisibility::HitTestInvisible
			|| TabVisibility == ESlateVisibility::SelfHitTestInvisible);
	}

	return false;
}

int32 UPXTabListWidgetBase::GetVisibleTabCount()
{
	int32 Result = 0;
	const int32 TabCount = GetTabCount();
	for ( int32 Index = 0; Index < TabCount; Index++ )
	{
		if (IsTabVisible(GetTabIdAtIndex( Index )))
		{
			Result++;
		}
	}

	return Result;
}

void UPXTabListWidgetBase::SetupTabs()
{
	for (FPXTabDescriptor& TabInfo : PreregisteredTabInfoArray)
	{
		if (TabInfo.bHidden)
		{
			continue;
		}

		// If the tab content hasn't been created already, create it.
		if (!TabInfo.CreatedTabContentWidget && TabInfo.TabContentType)
		{
			TabInfo.CreatedTabContentWidget = CreateWidget<UCommonUserWidget>(GetOwningPlayer(), TabInfo.TabContentType);
			OnTabContentCreatedNative.Broadcast(TabInfo.TabId, Cast<UCommonUserWidget>(TabInfo.CreatedTabContentWidget));
			OnTabContentCreated.Broadcast(TabInfo.TabId, Cast<UCommonUserWidget>(TabInfo.CreatedTabContentWidget));
		}

		if (UCommonAnimatedSwitcher* CurrentLinkedSwitcher = GetLinkedSwitcher())
		{
			// Add the tab content to the newly linked switcher.
			if (!CurrentLinkedSwitcher->HasChild(TabInfo.CreatedTabContentWidget))
			{
				CurrentLinkedSwitcher->AddChild(TabInfo.CreatedTabContentWidget);
			}
		}

		// If the tab is not already registered, register it.
		if (GetTabButtonBaseByID(TabInfo.TabId) == nullptr)
		{
			RegisterTab(TabInfo.TabId, TabInfo.TabButtonType, TabInfo.CreatedTabContentWidget);
		}
	}
}

