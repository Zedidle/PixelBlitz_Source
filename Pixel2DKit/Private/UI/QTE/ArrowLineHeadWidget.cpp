// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QTE/ArrowLineHeadWidget.h"

void UArrowLineHeadWidget::Init(FLinearColor _Color, FVector2D _Size)
{
	Color = _Color;
	Size = _Size;
}

void UArrowLineHeadWidget::SetAngle_Implementation(float Angle)
{
	IUI_Interface::SetAngle_Implementation(Angle);
}
