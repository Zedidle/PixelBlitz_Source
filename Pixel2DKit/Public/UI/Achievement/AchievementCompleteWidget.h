// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/AchievementSubsystem.h"
#include "AchievementCompleteWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class PIXEL2DKIT_API UAchievementCompleteWidget : public UUserWidget
{
	GENERATED_BODY()


protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	FText Name;

public:

	UFUNCTION(BlueprintCallable)
	void SetData(const FAchievement& AchievementData);
	
};
