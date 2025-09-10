// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "PXGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UPXGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UPXGameplayEffect();

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EffectTags)
	FGameplayTagContainer AssetTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EffectTags)
	FGameplayTagContainer GrantedTags;


	virtual void PostLoad() override;
	
};
