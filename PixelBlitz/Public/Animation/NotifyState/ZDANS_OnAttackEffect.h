// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Notifies/PaperZDAnimNotifyState.h"
#include "ZDANS_OnAttackEffect.generated.h"

/**
 * 
 */
UCLASS()
class PIXELBLITZ_API UZDANS_OnAttackEffect : public UPaperZDAnimNotifyState
{
	GENERATED_BODY()
public:
	void OnNotifyBegin_Implementation(UPaperZDAnimInstance* OwningInstance) const;
	void OnNotifyEnd_Implementation(UPaperZDAnimInstance* OwningInstance) const;
};
