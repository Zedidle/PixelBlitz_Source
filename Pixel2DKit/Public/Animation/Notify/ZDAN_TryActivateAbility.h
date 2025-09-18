// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Notifies/PaperZDAnimNotify.h"
#include "ZDAN_TryActivateAbility.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UZDAN_TryActivateAbility : public UPaperZDAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify | GAS")
	FGameplayTag AbilityTag;
	
	virtual void OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance = nullptr) override;

};
