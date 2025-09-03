// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UI/Common/BaseFloatingTextWidget.h"
#include "UserWidgetSettings.generated.h"

class UAchievementCompleteWidget;

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "UserWidget Settings"))
class PIXEL2DKIT_API UUserWidgetSettings : public UDeveloperSettings
{
	GENERATED_BODY()


public:

	UPROPERTY(Config, EditAnywhere, Category = "Common | UserWidget")
	TSubclassOf<UBaseFloatingTextWidget> FloatingTextDefaultClass;

	UPROPERTY(Config, EditAnywhere, Category = "Common | UserWidget")
	TSubclassOf<UUserWidget> CenterWordTipClass;
	
	UPROPERTY(Config, EditAnywhere, Category = "Achievement | UserWidget")
	TSubclassOf<UAchievementCompleteWidget> AchievementCompleteWidgetClass;
};
