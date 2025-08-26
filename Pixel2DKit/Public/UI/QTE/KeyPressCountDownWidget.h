// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KeyPressCountDownWidget.generated.h"


class UWidgetAnimation;


UCLASS(Abstract, Blueprintable)
class PIXEL2DKIT_API UKeyPressCountDownWidget : public UUserWidget
{
	GENERATED_BODY()


	bool Effected = false;
	float SustainTime;
	float Scale;


protected:

	virtual void NativeConstruct() override;






	
	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	UWidgetAnimation* TipKeyPress;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	UWidgetAnimation* Pressed;
	
public:

	UFUNCTION()
	void InitializeData(float SustainTime, float Scale);
	
	UFUNCTION(BlueprintCallable)	
	void OnPressed(bool& Effect);
	
};
