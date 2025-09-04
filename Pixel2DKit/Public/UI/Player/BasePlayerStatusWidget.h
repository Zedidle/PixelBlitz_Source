// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HeartsWidget.h"
#include "GAS/AttributeSet/PXAttributeSet.h"
#include "BasePlayerStatusWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS(Abstract, Blueprintable)
class PIXEL2DKIT_API UBasePlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Player Status", meta = (AllowPrivateAccess = "true"))
	const UPXAttributeSet* PXAttributeSet;

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UHeartsWidget* HeartsWidget;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* Anim_ShowIn;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UProgressBar* ProgressBar_HP;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UProgressBar* ProgressBar_EP;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* TextBlock_HP;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* TextBlock_EP;
	
	
public:

	UFUNCTION(BlueprintCallable, Category = "Player Status")
	void Init(const UPXAttributeSet* _PXAttributeSet);


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetViewEffect(bool Show, FLinearColor Color, float LargeRadius, float VelocitySpeed);


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RefreshLife();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateDark(float ViewWeekEffectPercent);

	UFUNCTION()
	void UpdateHP();

	UFUNCTION()
	void UpdateEP();

	UFUNCTION()
	void OnAttributeChanged(const FGameplayAttribute& Attribute, float  OldValue, float NewValue);
};
