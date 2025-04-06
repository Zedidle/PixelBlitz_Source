// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UserWidget/UWBaseFloatingText.h"
#include "UserWidgetSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "UserWidget Settings"))
class PIXEL2DKIT_API UUserWidgetSettings : public UDeveloperSettings
{
	GENERATED_BODY()


public:

	UPROPERTY(Config, EditAnywhere, Category = "Common | UserWidget")
	TSubclassOf<UUWBaseFloatingText> FloatingText_Default;

	
	
};
