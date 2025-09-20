// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PXUIRoot.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Pixel2DKit/Pixel2DKit.h"


void UPXUIRoot::NativeConstruct()
{
	Super::NativeConstruct();
	for (auto& WidgetStack : WidgetStacksToRegister)
	{
		if (WidgetStack.WidgetStack)
		{
			RegisterLayer(WidgetStack.Tag, WidgetStack.WidgetStack);
		}
	}
}

void UPXUIRoot::NativeDestruct()
{
	Super::NativeDestruct();


}

void UPXUIRoot::RootRemoveChild(UCommonActivatableWidget* ActivatableWidget, const FGameplayTag& LayerTag)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ActivatableWidget)
	if(UCommonActivatableWidgetContainerBase* Layer = GetLayerWidget(LayerTag))
	{
		Layer->RemoveWidget(*ActivatableWidget);
	}
}

UCommonActivatableWidgetContainerBase* UPXUIRoot::GetLayerWidget(FGameplayTag LayerName)
{
	return Layers.FindRef(LayerName);
}

void UPXUIRoot::RegisterLayer(FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget)
{
	if(!IsDesignTime())
	{
		LayerWidget->SetTransitionDuration(0.f);

		Layers.Add(LayerTag, LayerWidget);
	}
}

void UPXUIRoot::SetCanvas(UUserWidget* UIWidget)
{
	const auto& SlotCanvas = UWidgetLayoutLibrary::SlotAsCanvasSlot(UIWidget);
	FAnchors SlotAnchor = SlotCanvas->GetAnchors();
	SlotAnchor.Minimum = FVector2D(0, 0);
	SlotAnchor.Maximum = FVector2D(1, 1);
	SlotCanvas->SetAnchors(SlotAnchor);

	FMargin Offset = SlotCanvas->GetOffsets();
	Offset.Left = 0;
	Offset.Top = 0;
	Offset.Right = 0;
	Offset.Bottom = 0;
	SlotCanvas->SetOffsets(Offset);
}
