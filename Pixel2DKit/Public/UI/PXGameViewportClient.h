// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonGameViewportClient.h"
#include "PXGameViewportClient.generated.h"

class UGameInstance;
class UObject;

UCLASS(BlueprintType)
class PIXEL2DKIT_API UPXGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()
public:
	UPXGameViewportClient();

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;
};
