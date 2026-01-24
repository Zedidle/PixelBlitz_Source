// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/PXASComponent.h"
#include "AbilitySystemStats.h"
#include "AudioMixerBlueprintLibrary.h"
#include "NativeGameplayTags.h"
#include "Pixel2DKit.h"
#include "GAS/PXGameplayEffect.h"
#include "GAS/AttributeSet/PXAttributeSet.h"
#include "Utilitys/DebugFuncLab.h"

class UPXAttributeSet;

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

bool UPXASComponent::HasAbility(const FGameplayTag& Tag)
{
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{		
		if (Spec.Ability && Spec.Ability->GetAssetTags().HasTagExact(Tag))
		{
			return true;
		}
	}
	return false;
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
	if (!HasAbility(Tag)) return false;
	if (IsAbilityInCD(Tag)) return false;

	bool bSuccess = false;
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{		
		if (Spec.Ability && Spec.Ability->GetAssetTags().HasTagExact(Tag))
		{
			bSuccess |= TryActivateAbility(Spec.Handle, bAllowRemoteActivation);
		}
	}
	return bSuccess && IsAbilityInCD(Tag);
}

FGameplayAbilitySpec* UPXASComponent::GetAbilityByTagName(FName AbilityTagName)
{
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability)
		{
			if (Spec.Ability->GetAssetTags().HasTagExact(TAG(AbilityTagName)))
			{
				return &Spec;
			}
		}
	}
	return nullptr;
}


bool UPXASComponent::HasTag(FName TagName)
{
	if (TagName.IsNone()) return false;
	
	if (!CacheTags.Contains(TagName))
	{
		CacheTags.Add(TagName, TAG(TagName));
	}
	return HasMatchingGameplayTag(CacheTags[TagName]);
}

bool UPXASComponent::IsAbilityInCD(const FGameplayTag& AbilityTag)
{
	FName CDTagName = FName(AbilityTag.ToString() + ".CD");
	return HasTag(CDTagName);
}

float UPXASComponent::GetRemainingCD(const FGameplayTag& CooldownTag)
{
	// 获取冷却相关的 GameplayEffect
	TArray<FActiveGameplayEffectHandle> ActiveEffects = GetActiveEffects(
		FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(CooldownTag))
	);

	for (const FActiveGameplayEffectHandle& Handle : ActiveEffects)
	{
		if (const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect(Handle))
		{
			return ActiveEffect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
		}
	}
	
	return 0.0f;
}

void UPXASComponent::RemoveAbilityCD(const FGameplayTag& AbilityTag)
{
	if (!AbilityTag.IsValid()) return;
    
	// 1. 构建查询：查找所有拥有指定冷却标签的GameplayEffect
	FGameplayEffectQuery Query;
	Query.OwningTagQuery = FGameplayTagQuery::MakeQuery_MatchAnyTags(FGameplayTagContainer(AbilityTag));
    
	// 2. 获取所有匹配的活跃GE句柄
	TArray<FActiveGameplayEffectHandle> ActiveEffectHandles = GetActiveEffects(Query);
    
	// 3. 遍历并移除它们
	for (const FActiveGameplayEffectHandle& Handle : ActiveEffectHandles)
	{
		if (Handle.IsValid())
		{
			// 移除该GameplayEffect，冷却立即结束
			RemoveActiveGameplayEffect(Handle, 1); // 第二个参数“1”表示立即移除
		}
	}
    
	// 可选：移除可能残留的“松散标签”，确保状态完全一致
	RemoveLooseGameplayTag(AbilityTag);
}

// void UPXASComponent::SetPXAttributeValue(EPXAttribute AttributeName, float Value)
// {
// 	if (const UPXAttributeSet* AttributeSet = GetSet<UPXAttributeSet>())
// 	{
// 		UPXAttributeSet* MutableSet = const_cast<UPXAttributeSet*>(AttributeSet);
// 		FGameplayAttribute Attribute = AttributeSet->GetAttribute(AttributeName);
// 		if (FGameplayAttributeData* Data = Attribute.GetGameplayAttributeData(MutableSet))
// 		{
// 			Data->SetBaseValue(Value);
//
// 			// Data->SetCurrentValue(Value);
// 			
// 			// 现在你可以安全地修改 AttackData 了
// 			// Data->SetBaseValue(100.0f);
//
// 			// 或者使用你的自定义公式计算后，将结果设置给 CurrentValue
// 			// Data->GetCurrentValue();
// 			// Data->GetBaseValue();
// 		}
//
// 		// SetNumericAttributeBase(AttributeSet->GetAttributeByName(AttributeName), Value);
//
// 		// if (const FGameplayAttributeData* HealthData = Attribute.GetGameplayAttributeData(AttributeSet))
// 		// {
// 		// 	HealthData->SetCurrentValue(100.0f); // 修改当前值
// 		// }
// 	}
// }
