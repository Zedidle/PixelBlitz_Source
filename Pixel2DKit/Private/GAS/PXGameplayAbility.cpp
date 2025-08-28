// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/PXGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "GAS/PXGameplayEffect.h"
#include "opensubdiv/far/error.h"
#include "Pixel2DKit/Pixel2DKit.h"

class UPXGameplayEffect;

float UPXGameplayAbility::GetCooldownDuration_Implementation() const
{
	return CooldownDuration.GetValueAtLevel(GetAbilityLevel());
}

void UPXGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CooldownGE);
	
	// 创建 GE 规范（Spec）
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
    
	// 动态注入参数
	SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags); // 添加技能专属标签
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(FName("AbilityCD")), // 自定义 SetByCaller 标签
		GetCooldownDuration()
	);

	if (UPXGameplayEffect* PXGE = Cast<UPXGameplayEffect>(CooldownGE))
	{
		if (FGameplayEffectSpec* Spec = SpecHandle.Data.Get())
		{
			Spec->AppendDynamicAssetTags(PXGE->AssetTags);
			Spec->DynamicGrantedTags.AppendTags(PXGE->GrantedTags);
		}
	}
	
	// 应用至所有者
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}

const FGameplayTagContainer* UPXGameplayAbility::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // 清空临时容器（避免 CDO 残留旧标签）
    
	// 合并父类标签与技能专属标签
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags) MutableTags->AppendTags(*ParentTags);
	MutableTags->AppendTags(CooldownTags); 
	return MutableTags;
}

