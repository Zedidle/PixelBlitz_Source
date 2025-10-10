// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/ZDAN_TryActivateAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "PaperZDAnimInstance.h"
#include "GAS/PXASComponent.h"
#include "Pixel2DKit/Pixel2DKit.h"

void UZDAN_TryActivateAbility::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const
{
	if (!AbilityTag.IsValid()) return;
	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningInstance);
	AActor* Actor = OwningInstance->GetOwningActor();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Actor);

	if (const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor))
	{
		if (UPXASComponent* ASC = Cast<UPXASComponent>(ASI->GetAbilitySystemComponent()))
		{
			ASC->TryActivateAbilityByTag(AbilityTag);
		}
	}
}
