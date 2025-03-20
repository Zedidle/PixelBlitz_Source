// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BasePixelGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UBasePixelGameInstance : public UGameInstance
{
	GENERATED_BODY()




	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeneralSetting")
	uint8 GeneralSetting_Language;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VideoSetting")
	int VideoSetting_CameraMode;
	
};
