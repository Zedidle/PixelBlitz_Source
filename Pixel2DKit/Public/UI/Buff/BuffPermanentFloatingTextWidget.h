// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuffPermanentFloatingTextWidget.generated.h"


class UTextBlock;

UCLASS(Abstract, Blueprintable)
class PIXEL2DKIT_API UBuffPermanentFloatingTextWidget : public UUserWidget
{
	GENERATED_BODY()
protected:

	virtual void NativeConstruct() override;


	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	UWidgetAnimation* FloatIn;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TextBlock;

	
public:

	UPROPERTY(BlueprintReadOnly)
	FString BuffName;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor TextColor;

	UPROPERTY(BlueprintReadOnly)
	FVector2D Translation;

	UPROPERTY(BlueprintReadOnly)
	FVector2D RenderScale;

	UPROPERTY(BlueprintReadOnly)
	float PlaySpeed;

	UFUNCTION()
	void InitializeData(const FString& _BuffName, FLinearColor _TextColor,
		FVector2D _Translation = FVector2D::ZeroVector, FVector2D _RenderScale = FVector2D(1.0f, 1.0f), float _PlaySpeed = 1.0f);

	UFUNCTION()
	void SetBuffName(const FString& _BuffName);
};
