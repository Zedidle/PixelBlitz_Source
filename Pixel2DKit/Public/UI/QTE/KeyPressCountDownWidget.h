// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KeyPressCountDownWidget.generated.h"



class UWidgetAnimation;
class UInputAction;

UCLASS(Abstract, Blueprintable)
class PIXEL2DKIT_API UKeyPressCountDownWidget : public UUserWidget
{
	GENERATED_BODY()

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool Effected = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SustainTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Scale;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UInputAction* Action;
	
protected:

	virtual void NativeConstruct() override;






	
	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	UWidgetAnimation* TipKeyPress;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional))
	UWidgetAnimation* Pressed;
	
public:

	UFUNCTION()
	void InitializeData(float _SustainTime, float _Scale, UInputAction* _Action);
	
	UFUNCTION(BlueprintCallable)	
	void OnPressed(bool& Effect);
	
};
