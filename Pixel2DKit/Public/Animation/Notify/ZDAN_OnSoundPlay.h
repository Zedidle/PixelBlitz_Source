// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Notifies/PaperZDAnimNotify.h"
#include "ZDAN_OnSoundPlay.generated.h"

class USoundBase;

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UZDAN_OnSoundPlay : public UPaperZDAnimNotify
{
	GENERATED_BODY()

	
	
public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	USoundBase* Sound;
	
	virtual void OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance = nullptr) override;
};
