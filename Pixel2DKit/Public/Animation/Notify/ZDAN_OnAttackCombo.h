// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDAnimNotify.h"
#include "ZDAN_OnAttackCombo.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UZDAN_OnAttackCombo : public UPaperZDAnimNotify
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 NextComboIndex;
	
	void OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance = nullptr) const;
};
