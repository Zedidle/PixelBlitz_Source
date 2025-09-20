// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "UIDefine.generated.h"



UENUM(BlueprintType)
enum class EUILayerType : uint8
{
	Default = 0,
	Dock = 1,
	Window = 2,
	Top = 3,
	Tips = 4,
	Loading = 5,
	GM = 6,
};

USTRUCT(BlueprintType)
struct FUIConfigData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> UI_WidgetClass;

	UPROPERTY(EditAnywhere)
	FGameplayTag LayerTag = FGameplayTag::RequestGameplayTag(FName("UI.Layer.Default"));
};
