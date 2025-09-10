// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/UI_Interface.h"
#include "ArrowLineBodyWidget.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UArrowLineBodyWidget : public UUserWidget, public IUI_Interface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Line", meta = (AllowPrivateAccess = "true", ExposeOnSpawn))
	FLinearColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow Line", meta = (AllowPrivateAccess = "true", ExposeOnSpawn))
	FVector2D Size = {40, 60};
	
public:
	UFUNCTION()
	void Init(FLinearColor _Color, FVector2D _Size);

#pragma region UI_Interface

	virtual void SetAngle_Implementation(float Angle) override;
	
#pragma endregion
	
};
