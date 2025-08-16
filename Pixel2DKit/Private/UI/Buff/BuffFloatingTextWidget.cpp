// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Buff/BuffFloatingTextWidget.h"

#include "Components/TextBlock.h"


void UBuffFloatingTextWidget::InitializeData(FText _Text, FLinearColor _TextColor, FVector2D _Translation,
                                             FVector2D _RenderScale, float _PlaySpeed, bool _In)
{
	Text = _Text;
	TextColor = _TextColor;
	Translation = _Translation;
	RenderScale = _RenderScale;
	PlaySpeed = _PlaySpeed;
	In = _In;
}


void UBuffFloatingTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (In)
	{
		if (FloatIn)
		{
			PlayAnimationForward(FloatIn, PlaySpeed);
		}
	}
	else
	{
		if (FloatOut)
		{
			PlayAnimationForward(FloatOut, PlaySpeed);
		}
	}

	SetRenderTranslation(Translation);
	TextBlock->SetRenderScale(RenderScale);
	
}