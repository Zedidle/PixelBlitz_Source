// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PXSettingSaveGame.generated.h"

struct FEnhancedActionKeyMapping;


UCLASS()
class PIXEL2DKIT_API UPXSettingSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

#pragma region VideoSetting
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting | Video")
	TSoftObjectPtr<UTexture> VideoSetting_PPVColorGradingLUT;
	
#pragma endregion
	



	
};
