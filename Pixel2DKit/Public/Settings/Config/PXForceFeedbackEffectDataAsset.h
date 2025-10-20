// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PXForceFeedbackEffectDataAsset.generated.h"


UCLASS(BlueprintType)
class PIXEL2DKIT_API UPXForceFeedbackEffectDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY( EditAnywhere, Category = "ForceFeedbackEffect")
	TSoftObjectPtr<UForceFeedbackEffect> LittleDamage;

	UPROPERTY( EditAnywhere, Category = "ForceFeedbackEffect")
	TSoftObjectPtr<UForceFeedbackEffect> NormalDamage;

	UPROPERTY( EditAnywhere, Category = "ForceFeedbackEffect")
	TSoftObjectPtr<UForceFeedbackEffect> HugeDamage;

	UPROPERTY( EditAnywhere, Category = "ForceFeedbackEffect")
	TSoftObjectPtr<UForceFeedbackEffect> PlayerDie;

	UPROPERTY( EditAnywhere, Category = "ForceFeedbackEffect")
	TSoftObjectPtr<UForceFeedbackEffect> Knockback;

	UPROPERTY( EditAnywhere, Category = "ForceFeedbackEffect")
	TSoftObjectPtr<UForceFeedbackEffect> HugeKnockback;

	UPROPERTY( EditAnywhere, Category = "ForceFeedbackEffect")
	TSoftObjectPtr<UForceFeedbackEffect> AttackHolding;

	UPROPERTY( EditAnywhere, Category = "ForceFeedbackEffect")
	TSoftObjectPtr<UForceFeedbackEffect> Attack;
};
