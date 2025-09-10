// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QTE/ArrowLineHeadAntiWidget.h"

void UArrowLineHeadAntiWidget::Init(FLinearColor _Color, FVector2D _Size)
{
	Color = _Color;
	Size = _Size;
}

void UArrowLineHeadAntiWidget::SetAngle_Implementation(float Angle)
{
	IUI_Interface::SetAngle_Implementation(Angle);
}
