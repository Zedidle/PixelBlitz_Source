// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "PixelPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API APixelPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

	public:
	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { checkf(false, TEXT("%s need implement GetAbilitySystemComponent interface"), *GetName()); return nullptr; }
	
};
