// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/PXCooldownGameplayEffect.h"

UPXCooldownGameplayEffect::UPXCooldownGameplayEffect()
{
	// 有持续时间：具体秒数由 ApplyCooldown 的 SetByCaller("AbilityCD") 注入
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FSetByCallerFloat DurationByCaller;
	DurationByCaller.DataTag = FGameplayTag::RequestGameplayTag(FName("AbilityCD"), false);
	DurationMagnitude = FGameplayEffectModifierMagnitude(DurationByCaller);

	// 非周期；冷却标签由 ApplyCooldown 动态授予，这里不需要额外配置
	Period = FScalableFloat(0.f);
	bExecutePeriodicEffectOnApplication = false;
}
