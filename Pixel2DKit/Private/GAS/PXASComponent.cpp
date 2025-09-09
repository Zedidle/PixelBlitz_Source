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

TWeakObjectPtr<UGameplayAbility> UPXASComponent::GetAbilityByTag(FGameplayTag AbilityTag)
{
	// 遍历所有激活的技能
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability)
		{
			if (Spec.Ability->AbilityTags.HasTagExact(AbilityTag))
			{
				return Spec.Ability;
			}
		}
		
		// 检查技能Tag匹配
		// if (Spec.Ability && Spec.DynamicAbilityTags.HasTagExact(AbilityTag))
		// {
		// 	// 返回安全弱指针
		// 	return TWeakObjectPtr<UGameplayAbility>(Spec.Ability);
		// }
	}
	return nullptr; // 安全返回空
}


bool UPXASComponent::HasTag(FName TagName)
{
	return HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TagName));
}
