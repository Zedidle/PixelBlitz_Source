// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PlatformFightCountWidget.generated.h"

class UWidgetAnimation;

UCLASS()
class PIXEL2DKIT_API UPlatformFightCountWidget : public UUserWidget
{
	GENERATED_BODY()



protected:
	virtual void NativeConstruct() override;


	
public:
	
	UPROPERTY(BlueprintReadWrite)
	int32 CurNum = 99;
	UPROPERTY(BlueprintReadWrite)
	int32 ToNum = 0;
	
	UPROPERTY(BlueprintReadWrite, Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimShowIn;
	
	UPROPERTY(BlueprintReadWrite, Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* AnimCount;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional))
	UTextBlock* TextNum;

	
	UFUNCTION(BlueprintCallable)
	void Show();

	UFUNCTION(BlueprintCallable)
	void Fade(float Delay = 2);

	UFUNCTION(BlueprintCallable)
	void UpdateCount(int32 Num);
	
};