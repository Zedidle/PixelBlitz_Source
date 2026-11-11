// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/PXGameplayAbility.h"
#include "GA_Leverage.generated.h"

/**
 * 借力打力：完美闪避后（Timing = BeAttackedInvulnerable）触发，
 * 额外累积 1 层弱点连击；由 ComboSubsystem 统一刷新 UI / 音效 / 衰减计时 / BGM 闪避。
 */
UCLASS()
class PIXELBLITZ_API UGA_Leverage : public UPXGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Leverage();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** 冷却时长：读天赋 ExtendData（Ability.Leverage.CD，单位秒），未配置时用 CooldownDuration 默认值 */
	virtual float GetCooldownDuration_Implementation() const override;
};
