// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Notifies/PaperZDAnimNotify.h"
#include "ZDAN_OnDefendingHitEffect.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UZDAN_OnDefendingHitEffect : public UPaperZDAnimNotify
{
	GENERATED_BODY()

public:
	void OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance = nullptr) const;
};
