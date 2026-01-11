// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UI/Player/ComboWeakPointCountWidget.h"
#include "PXWidgetsDataAsset.generated.h"

class UAchievementCompleteWidget;
class UBaseFloatingTextWidget;
/**
 * 
 */
UCLASS(BlueprintType)
class PIXEL2DKIT_API UPXWidgetsDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY( EditAnywhere, Category = "Common")
	TSubclassOf<UBaseFloatingTextWidget> FloatingTextDefaultClass;

	UPROPERTY( EditAnywhere, Category = "Common")
	TSubclassOf<UUserWidget> CenterWordTipClass;
	
	UPROPERTY( EditAnywhere, Category = "Achievement")
	TSubclassOf<UAchievementCompleteWidget> AchievementCompleteWidgetClass;
	
	UPROPERTY( EditAnywhere, Category = "Player")
	TSubclassOf<UUserWidget> NearDeathWidgetClass;

	UPROPERTY( EditAnywhere, Category = "ComboSubsystem")
	TSubclassOf<UUserWidget> ComboWeakPointCountWidget;
};
