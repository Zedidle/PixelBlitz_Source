// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PaperSprite.h"
#include "UWBaseFloatingText.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PIXEL2DKIT_API UUWBaseFloatingText : public UUserWidget
{
	GENERATED_BODY()

	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FloatingText", meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	UPaperSprite* SpriteToDisplay = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FloatingText", meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FloatingText", meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	FLinearColor TextColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FloatingText", meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	FVector InitialLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FloatingText")
	FVector FinalLocation;

	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FloatingText")
	void Init(const FText& InText, FLinearColor Color, UPaperSprite* InSpriteToDisplay, FVector InitLocation = FVector::ZeroVector);
	virtual void Init_Implementation(const FText& InText, FLinearColor Color, UPaperSprite* InSpriteToDisplay, FVector InitLocation = FVector::ZeroVector);
};
