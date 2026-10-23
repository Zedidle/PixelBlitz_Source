// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Buff/BuffPermanentFloatingTextWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"

void UBuffPermanentFloatingTextWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	PlayAnimationForward(FloatIn, PlaySpeed);

	SetRenderTranslation(Translation);

	if (TextBlock)
	{
		TextBlock->SetRenderScale(RenderScale);
		TextBlock->SetColorAndOpacity(TextColor);
	}
	
}

void UBuffPermanentFloatingTextWidget::InitializeData(const FString& _BuffName, FLinearColor _TextColor, FVector2D _Translation,
                                                      FVector2D _RenderScale, float _PlaySpeed)
{
		SetBuffName(_BuffName);
    	TextColor = _TextColor;
    	Translation = _Translation;
    	RenderScale = _RenderScale;
    	PlaySpeed = _PlaySpeed;
}

void UBuffPermanentFloatingTextWidget::SetBuffName(const FString& _BuffName)
{
	BuffName = _BuffName;
	TextBlock->SetText(FText::FromString(UKismetStringLibrary::Concat_StrStr(BuffName,"*")));
	
}
