// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Buff/BuffFloatingTextWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"


void UBuffFloatingTextWidget::InitializeData(const FString& _BuffName, FLinearColor _TextColor, float _PlaySpeed, bool _In, FVector2D _Translation, FVector2D _RenderScale)
{
	SetBuffName(_BuffName);
	TextColor = _TextColor;
	Translation = _Translation;
	RenderScale = _RenderScale;
	PlaySpeed = _PlaySpeed;
	In = _In;
}

void UBuffFloatingTextWidget::SetBuffName(const FString& _BuffName)
{
	BuffName = _BuffName;
	TextBlock->SetText(FText::FromString(UKismetStringLibrary::Concat_StrStr(">", BuffName)));
}

void UBuffFloatingTextWidget::PlayIn()
{
	if (FloatIn)
	{
		PlayAnimationForward(FloatIn, PlaySpeed);
	}
}

void UBuffFloatingTextWidget::PlayOut()
{
	if (FloatOut)
	{
		PlayAnimationForward(FloatOut, PlaySpeed);
	}
}

void UBuffFloatingTextWidget::DispearOut()
{
	if (Dispear)
	{
		PlayAnimationForward(Dispear, PlaySpeed);
	}
}



void UBuffFloatingTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (In)
	{
		PlayIn();
	}
	else
	{
		PlayOut();
	}

	SetRenderTranslation(Translation);
	if (TextBlock)
	{
		TextBlock->SetRenderScale(RenderScale);
		TextBlock->SetColorAndOpacity(TextColor);
	}
}
