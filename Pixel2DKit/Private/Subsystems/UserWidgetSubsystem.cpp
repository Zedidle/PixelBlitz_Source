// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/UserWidgetSubsystem.h"
#include "Blueprint/UserWidget.h"


UUserWidget* UUserWidgetSubsystem::GetWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (!WidgetClass) return nullptr;

	if (Widgets.Contains(WidgetClass))
	{
		return Widgets[WidgetClass];
	}
	return nullptr;
}


UUserWidget* UUserWidgetSubsystem::AddWidget(TSubclassOf<UUserWidget> WidgetClass, const ESlateVisibility Visibility, bool HideCurrentWidget)
{
	if (!WidgetClass) return nullptr;
	
	if (Widgets.Contains(WidgetClass))
	{
		return ShowWidget(WidgetClass, Visibility, HideCurrentWidget);
	}

	if (UUserWidget* w = CreateWidget(GetWorld(), WidgetClass))
	{
		CurrentWidget = w;
		CurrentWidget->AddToViewport(Layer ++);
		Widgets.Add(WidgetClass, CurrentWidget);
		return CurrentWidget;
	}
	
	return nullptr;
}

UUserWidget* UUserWidgetSubsystem::ShowWidget(TSubclassOf<UUserWidget> WidgetClass, const ESlateVisibility Visibility, bool HideCurrentWidget)
{
	if (!WidgetClass) return nullptr;

	if (HideCurrentWidget)
	{
		CurrentWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if (Widgets.Contains(WidgetClass))
	{
		CurrentWidget = Widgets[WidgetClass];
		CurrentWidget->SetVisibility(Visibility);
		return CurrentWidget;
	}
	return nullptr;
}

void UUserWidgetSubsystem::HideWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (!WidgetClass) return;

	if (Widgets.Contains(WidgetClass))
	{
		Widgets[WidgetClass]->SetVisibility(ESlateVisibility::Collapsed);
	}
}

