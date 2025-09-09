// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interact_Interface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteract_Interface : public UInterface
{
	GENERATED_BODY()
};


class PIXEL2DKIT_API IInteract_Interface
{
	GENERATED_BODY()
    
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interact_Interface")
	void Interact();
	
	
};