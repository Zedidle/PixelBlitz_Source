// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UserWidgetFuncLib.generated.h"

/**
 * 
 */
UCLASS(Abstract, MinimalAPI)
class UUserWidgetFuncLib : public UObject
{
	GENERATED_BODY()

	public:
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "UWS_GetWidget" , Category="UserWidgetFuncLib" , meta=(DeterminesOutputType = "WidgetClass"))
	static UUserWidget* GetWidget(TSubclassOf<UUserWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, DisplayName = "UWS_AddWidget" , Category="UserWidgetFuncLib" , meta=(DeterminesOutputType = "WidgetClass"))
	static UUserWidget* AddWidget(TSubclassOf<UUserWidget> WidgetClass, const ESlateVisibility Visibility, bool HideCurrentWidget = false);

	UFUNCTION(BlueprintCallable, DisplayName = "UWS_ShowWidget" , Category="UserWidgetFuncLib" , meta=(DeterminesOutputType = "WidgetClass"))
	static UUserWidget* ShowWidget(TSubclassOf<UUserWidget> WidgetClass, const ESlateVisibility Visibility, bool HideCurrentWidget = true);
	
	UFUNCTION(BlueprintCallable, DisplayName = "UWS_HideWidget" , Category="UserWidgetFuncLib" )
	static void HideWidget(TSubclassOf<UUserWidget> WidgetClass);
};
