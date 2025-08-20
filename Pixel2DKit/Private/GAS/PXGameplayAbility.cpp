// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/PXGameplayAbility.h"

#include "GAS/PXGameplayEffect.h"

class UPXGameplayEffect;

void UPXGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	
	if (!CooldownGameplayEffectClass) return;
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE && HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CooldownGE->GetClass(), GetAbilityLevel(Handle, ActorInfo));

		if (UPXGameplayEffect* PXGE = Cast<UPXGameplayEffect>(CooldownGE))
		{
			if (FGameplayEffectSpec* Spec = SpecHandle.Data.Get())
			{
				Spec->AppendDynamicAssetTags(PXGE->AssetTags);
				Spec->DynamicGrantedTags.AppendTags(PXGE->GrantedTags);
			}
		}
		
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetStackCount(1);
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
	}
	
}
