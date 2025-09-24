// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utilitys/CommonFuncLib.h"
#include "CenterWordTipWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

UCLASS(Abstract, Blueprintable, BlueprintType)
class PIXEL2DKIT_API UCenterWordTipWidget : public UUserWidget
{
	GENERATED_BODY()

	FText Tip;
	FLinearColor Color;
	FVector2D Translation;
	FVector2D Scale;
	float PlaybackSpeed;
	EFloatingTextAnimType AnimType;

public:

	virtual void NativeConstruct() override;
	

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* TextTip;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* textShake;
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* textFloatUp;
	
	UFUNCTION(BlueprintCallable, Category = "Common | CenterWordTipWidget")
	void Init(FText _Tip, FLinearColor _Color, FVector2D _Translation, FVector2D _Scale,
		float _PlaybackSpeed, EFloatingTextAnimType _AnimType);
};
