// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/PXUserWidget.h"
#include "CommonTipWidget.generated.h"


UCLASS()
class PIXEL2DKIT_API UCommonTipWidget : public UPXUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetContent(const FString& InContent, float InTime);

	UFUNCTION()
	void OnTipPlayAnimOutEnd();

	UFUNCTION()
	void RemoveSelf();

	UFUNCTION()
	void PlayAnimIn();

	UFUNCTION()
	void PlayAnimOut();

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional))
	UTextBlock* Text_Content;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	UWidgetAnimation* Anim_in;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	UWidgetAnimation* Anim_out;

	FTimerHandle ShowTipTimerHandle;

	float CacheShowTipTime;
};
