// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Buff/BuffPermanentFloatingTextWidget.h"

#include "Components/TextBlock.h"

void UBuffPermanentFloatingTextWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	PlayAnimationForward(FloatIn, PlaySpeed);

	SetRenderTranslation(Translation);

	if (TextBlock)
	{
		TextBlock->SetRenderScale(RenderScale);
	}
	
}

void UBuffPermanentFloatingTextWidget::InitializeData(FText _Text, FLinearColor _TextColor, FVector2D _Translation,
                                                      FVector2D _RenderScale, float _PlaySpeed)
{
		Text = _Text;
    	TextColor = _TextColor;
    	Translation = _Translation;
    	RenderScale = _RenderScale;
    	PlaySpeed = _PlaySpeed;
}
