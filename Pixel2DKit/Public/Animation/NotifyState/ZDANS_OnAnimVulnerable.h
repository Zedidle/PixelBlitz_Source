// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Notifies/PaperZDAnimNotifyState.h"
#include "ZDANS_OnAnimVulnerable.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UZDANS_OnAnimVulnerable : public UPaperZDAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void OnNotifyBegin_Implementation(UPaperZDAnimInstance* OwningInstance) override;
	virtual void OnNotifyEnd_Implementation(UPaperZDAnimInstance* OwningInstance) override;
};
