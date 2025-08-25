// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuffFloatingTextWidget.generated.h"

class UWidgetAnimation;
class UTextBlock;


UCLASS(Abstract, Blueprintable)
class PIXEL2DKIT_API UBuffFloatingTextWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;


	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* FloatIn;
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* FloatOut;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TextBlock;

	
public:

	UPROPERTY(BlueprintReadOnly)
	FText Text;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor TextColor = FLinearColor::White;

	UPROPERTY(BlueprintReadOnly)
	FVector2D Translation = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FVector2D RenderScale = FVector2D(1.0f, 1.0f);

	UPROPERTY(BlueprintReadOnly)
	float PlaySpeed = 1.0f;

	UPROPERTY(BlueprintReadOnly)
	bool In = false;
	
	
	UFUNCTION()
	void InitializeData(FText _Text, FLinearColor _TextColor, FVector2D _Translation, FVector2D _RenderScale, float _PlaySpeed, bool _In);

	UFUNCTION()
	void PlayOut();
	
};
