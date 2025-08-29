// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/PXASComponent.h"
#include "AbilitySystemStats.h"
#include "GAS/PXGameplayEffect.h"

FGameplayEffectSpecHandle UPXASComponent::MakeOutgoingSpec(TSubclassOf<UGameplayEffect> GameplayEffectClass,
                                                           float Level, FGameplayEffectContextHandle Context) const
{
	// SCOPE_CYCLE_COUNTER(STAT_GetOutgoingSpec);
	if (Context.IsValid() == false)
	{
		Context = MakeEffectContext();
	}

	if (GameplayEffectClass)
	{
		UGameplayEffect* GameplayEffect = GameplayEffectClass->GetDefaultObject<UGameplayEffect>();
		FGameplayEffectSpec* NewSpec = new FGameplayEffectSpec(GameplayEffect, Context, Level);
		if (UPXGameplayEffect* GE = Cast<UPXGameplayEffect>(GameplayEffect))
		{
			NewSpec->AppendDynamicAssetTags(GE->AssetTags);
			NewSpec->DynamicGrantedTags.AppendTags(GE->GrantedTags);
		}
		return FGameplayEffectSpecHandle(NewSpec);
	}

	return FGameplayEffectSpecHandle(nullptr);
}
