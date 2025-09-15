// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/CenterWordTipWidget.h"

#include "Components/TextBlock.h"


void UCenterWordTipWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (AnimType == EFloatingTextAnimType::Shaking)
	{
		if (textShake)
		{
			PlayAnimation(textShake, 0.0f, 1, EUMGSequencePlayMode::Forward, PlaybackSpeed);
		}
	}
	else if (AnimType == EFloatingTextAnimType::Floatup)
	{
		if (textFloatUp)
		{
			PlayAnimation(textFloatUp, 0.0f, 1, EUMGSequencePlayMode::Forward, PlaybackSpeed);
		}
	}

	SetRenderTranslation(Translation);
	if (TextTip)
	{
		TextTip->SetRenderScale(Scale);
		TextTip->SetText(Tip);
		TextTip->SetColorAndOpacity(Color);
	}
}

void UCenterWordTipWidget::Init(FText _Tip, FLinearColor _Color, FVector2D _Translation, FVector2D _Scale,
                                float _PlaybackSpeed, EFloatingTextAnimType _AnimType)
{
	Tip = _Tip;
	Color = _Color;
	Translation = _Translation;
	Scale = _Scale;
	PlaybackSpeed = _PlaybackSpeed;
	AnimType = _AnimType;
}
