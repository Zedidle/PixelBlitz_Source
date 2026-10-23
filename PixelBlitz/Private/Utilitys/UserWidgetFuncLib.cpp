// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/UserWidgetFuncLib.h"
#include "UI/UIManager.h"


UUserWidget* UUserWidgetFuncLib::GetCurWidget()
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!World) return nullptr;

	UUIManager* UIManager = UUIManager::GetSelfInstance(World);
	if (!UIManager) return nullptr;

	return UIManager->GetCurWidget();
}

UUserWidget* UUserWidgetFuncLib::GetWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!World) return nullptr;

	UUIManager* UIManager = UUIManager::GetSelfInstance(World);
	if (!UIManager) return nullptr;

	return UIManager->GetWidget(WidgetClass);
}

UUserWidget* UUserWidgetFuncLib::AddWidget(TSubclassOf<UUserWidget> WidgetClass, const ESlateVisibility Visibility, bool HideCurrentWidget, int PlusZOrder)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!World) return nullptr;

	UUIManager* UIManager = UUIManager::GetSelfInstance(World);
	if (!UIManager) return nullptr;
	
	return UIManager->AddWidget(WidgetClass, Visibility, HideCurrentWidget, PlusZOrder);
}

UUserWidget* UUserWidgetFuncLib::ShowWidget(TSubclassOf<UUserWidget> WidgetClass, const ESlateVisibility Visibility, bool HideCurrentWidget)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!World) return nullptr;
	
	UUIManager* UIManager = UUIManager::GetSelfInstance(World);
	if (!UIManager) return nullptr;

	return UIManager->ShowWidget(WidgetClass, Visibility);
}

void UUserWidgetFuncLib::HideWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!World) return;

	UUIManager* UIManager = UUIManager::GetSelfInstance(World);
	if (!UIManager) return;

	UIManager->HideWidget(WidgetClass);

}


