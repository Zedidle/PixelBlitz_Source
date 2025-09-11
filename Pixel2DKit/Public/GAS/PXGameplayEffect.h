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

	/* 资源Tag
	 * 可以用于提示 Buff 的加入和移除，参考DT_BuffOnWidget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EffectTags)
	FGameplayTagContainer AssetTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EffectTags)
	FGameplayTagContainer GrantedTags;


	virtual void PostLoad() override;
	
};
