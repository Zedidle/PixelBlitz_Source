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

bool UPXASComponent::TryActivateAbilities(const FGameplayTagContainer& GameplayTagContainer, FName CDTagName)
{
	if (HasTag(CDTagName))
	{
		return false;
	}

	TryActivateAbilitiesByTag(GameplayTagContainer);
	return HasTag(CDTagName);
}

FGameplayAbilitySpec* UPXASComponent::GetAbilityByTagName(FName AbilityTagName)
{
	// 遍历所有激活的技能
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability)
		{
			if (Spec.Ability->AbilityTags.HasTagExact(FGameplayTag::RequestGameplayTag(AbilityTagName)))
			{
				return &Spec;
			}
		}
	}
	return nullptr; // 安全返回空
}


bool UPXASComponent::HasTag(FName TagName)
{
	if (!TagCache.Contains(TagName))
	{
		TagCache.Add(TagName, FGameplayTag::RequestGameplayTag(TagName));
	}
	return HasMatchingGameplayTag(TagCache[TagName]);
}
