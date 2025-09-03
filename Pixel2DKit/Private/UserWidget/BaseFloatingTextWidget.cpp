// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/BaseFloatingTextWidget.h"
#include "PaperSprite.h"




void UBaseFloatingTextWidget::Init_Implementation(const FText& InText, FLinearColor Color, UPaperSprite* InSpriteToDisplay,
	FVector InitLocation)
{
	Text = InText;
	TextColor = Color;
	SpriteToDisplay = InSpriteToDisplay;
	InitialLocation = InitLocation + FVector(FMath::FRandRange(-10.0f, 10.0f), 0, 10);
	FinalLocation = InitLocation + FVector(FMath::FRandRange(-10.0f, 20.0f), 0, FMath::FRandRange(45.0f, 55.0f));

	
}
