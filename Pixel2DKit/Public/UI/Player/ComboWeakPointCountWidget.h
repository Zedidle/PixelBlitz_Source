// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComboWeakPointCountWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UComboWeakPointCountWidget : public UUserWidget
{
	GENERATED_BODY()


	float ComboSustainTime = 5;

	
protected:
	virtual void NativeConstruct() override;


	
public:

	
	UPROPERTY(BlueprintReadWrite)
	int32 CurNum = 1;

	UPROPERTY(BlueprintReadWrite)
	int32 CurDamagePlusPercent = 0;

	UPROPERTY(BlueprintReadWrite, Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimShowIn;
	
	UPROPERTY(BlueprintReadWrite, Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimCount;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional))
	UTextBlock* TextNum;

	UFUNCTION(BlueprintCallable)
	void Fade(float Delay = 2);

	UFUNCTION(BlueprintCallable)
	void UpdateCount(int32 Num, int32 DamagePlusPercent);

	
	
};
