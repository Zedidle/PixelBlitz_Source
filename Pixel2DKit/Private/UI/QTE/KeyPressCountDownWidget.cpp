// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QTE/KeyPressCountDownWidget.h"

#include "Pixel2DKit/Pixel2DKit.h"

void UKeyPressCountDownWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TipKeyPress)
	{
		PlayAnimationForward(TipKeyPress, 1 / SustainTime, false);
	}

	
}

void UKeyPressCountDownWidget::InitializeData(float _SustainTime, float _Scale)
{
	SustainTime = _SustainTime;
	Scale = _Scale;
}

void UKeyPressCountDownWidget::OnPressed(bool& Effect)
{
	if (Effected)
	{
		Effect = false;
		return;
	}

	Effected = true;
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(TipKeyPress)

	StopAnimation(TipKeyPress);
	PlayAnimation(Pressed, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
	Effect = true;
}
