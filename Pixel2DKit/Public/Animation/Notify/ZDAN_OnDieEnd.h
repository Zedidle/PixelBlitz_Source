// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Notifies/PaperZDAnimNotify.h"
#include "ZDAN_OnDieEnd.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UZDAN_OnDieEnd : public UPaperZDAnimNotify
{
	GENERATED_BODY()
public:
	void OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance = nullptr) const;
};
