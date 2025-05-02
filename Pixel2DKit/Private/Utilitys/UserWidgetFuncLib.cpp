// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/UserWidgetFuncLib.h"
#include "Subsystems/UserWidgetSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Tests/AutomationCommon.h"

UUserWidget* UUserWidgetFuncLib::GetWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	UWorld* world = GEngine->GetCurrentPlayWorld();
	if (!world) return nullptr;
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(world))
	{
		if (UUserWidgetSubsystem* UWS = GI->GetSubsystem<UUserWidgetSubsystem>())
		{
			return UWS->GetWidget(WidgetClass);
		}
	}
	return nullptr;
}

UUserWidget* UUserWidgetFuncLib::AddWidget(TSubclassOf<UUserWidget> WidgetClass, const ESlateVisibility Visibility, bool HideCurrentWidget)
{
	UWorld* world = GEngine->GetCurrentPlayWorld();
	if (!world) return nullptr;
	
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(world))
	{
		if (UUserWidgetSubsystem* UWS = GI->GetSubsystem<UUserWidgetSubsystem>())
		{
			return UWS->AddWidget(WidgetClass, Visibility, HideCurrentWidget);
		}
		
	}
	return nullptr;
}

UUserWidget* UUserWidgetFuncLib::ShowWidget(TSubclassOf<UUserWidget> WidgetClass, const ESlateVisibility Visibility, bool HideCurrentWidget)
{
	UWorld* world = GEngine->GetCurrentPlayWorld();
	if (!world) return nullptr;
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(world))
	{
		if (UUserWidgetSubsystem* UWS = GI->GetSubsystem<UUserWidgetSubsystem>())
		{
			return UWS->ShowWidget(WidgetClass, Visibility);
		}
	}
	return nullptr;
}

void UUserWidgetFuncLib::HideWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	UWorld* world = GEngine->GetCurrentPlayWorld();
	if (!world) return;
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(world))
	{
		if (UUserWidgetSubsystem* UWS = GI->GetSubsystem<UUserWidgetSubsystem>())
		{
			UWS->HideWidget(WidgetClass);
		}
	}
}


