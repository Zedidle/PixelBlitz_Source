// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonTipContainerWidget.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UCommonTipContainerWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void AddTip(const FString& Content, float Time);

	UFUNCTION()
	void RemoveTip();

	UFUNCTION()
	void OnCommonTipRemove(UCommonTipWidget* TipWidget);

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional))
	class UScrollBox* ScrollBox_Tips;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Settings")
	TSubclassOf<UCommonTipWidget> CommonTipClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Settings")
	int32 MaxShowCommonTip = 3;

	TArray<UCommonTipWidget*> CacheTipQueue;
};
