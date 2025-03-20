// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SlateWrapperTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UserWidgetSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UUserWidgetSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	int Layer = 0;
	UPROPERTY()
	TMap<TSubclassOf<UUserWidget>, UUserWidget*> Widgets;

	
public:
	UPROPERTY(BlueprintReadOnly, Category = "UserWidgetSubsystem")
	UUserWidget* CurrentWidget;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UserWidgetSubsystem", meta = (DeterminesOutputType = "WidgetClass"))
	UUserWidget* GetWidget(TSubclassOf<UUserWidget> WidgetClass);
	
	UFUNCTION(BlueprintCallable, Category = "UserWidgetSubsystem", meta = (DeterminesOutputType = "WidgetClass"))
	UUserWidget* AddWidget(TSubclassOf<UUserWidget> WidgetClass, const ESlateVisibility Visibility = ESlateVisibility::Visible, bool HideCurrentWidget = false);

	UFUNCTION(BlueprintCallable, Category = "UserWidgetSubsystem", meta = (DeterminesOutputType = "WidgetClass"))
	UUserWidget* ShowWidget(TSubclassOf<UUserWidget> WidgetClass, const ESlateVisibility Visibility = ESlateVisibility::Visible, bool HideCurrentWidget = true);
	
	UFUNCTION(BlueprintCallable, Category = "UserWidgetSubsystem")
	void HideWidget(TSubclassOf<UUserWidget> WidgetClass);
	
};