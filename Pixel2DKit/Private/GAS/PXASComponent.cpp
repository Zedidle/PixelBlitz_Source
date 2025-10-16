// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/PXASComponent.h"
#include "AbilitySystemStats.h"
#include "AudioMixerBlueprintLibrary.h"
#include "NativeGameplayTags.h"
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

bool UPXASComponent::TryActivateAbilityByTagName(FName TagName, bool bAllowRemoteActivation)
{
	if (TagName.IsNone()) return false;
	return TryActivateAbilityByTag(FGameplayTag::RequestGameplayTag(TagName), bAllowRemoteActivation);
}

bool UPXASComponent::TryActivateAbilityByTag(const FGameplayTag& Tag, bool bAllowRemoteActivation)
{
	FName CDTagName = FName(Tag.ToString() + ".CD");
	if (HasTag(CDTagName)) return false;

	bool bSuccess = false;
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{		
		if (Spec.Ability && Spec.Ability->GetAssetTags().HasTagExact(Tag))
		{
			bSuccess |= TryActivateAbility(Spec.Handle, bAllowRemoteActivation);
		}
	}
	return bSuccess && HasTag(CDTagName);
}

FGameplayAbilitySpec* UPXASComponent::GetAbilityByTagName(FName AbilityTagName)
{

	FGameplayTag Tag = FGameplayTag::RequestGameplayTag(AbilityTagName);

	// 遍历所有激活的技能
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability)
		{
			if (Spec.Ability->GetAssetTags().HasTagExact(Tag))
			{
				return &Spec;
			}
		}
	}
	return nullptr; // 安全返回空
}


bool UPXASComponent::HasTag(FName TagName)
{
	if (TagName.IsNone()) return false;
	
	if (!TagCache.Contains(TagName))
	{
		TagCache.Add(TagName, FGameplayTag::RequestGameplayTag(TagName));
	}
	return HasMatchingGameplayTag(TagCache[TagName]);
}
