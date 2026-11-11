// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "PXCooldownGameplayEffect.generated.h"

/**
 * 通用冷却 GE（纯 C++，不需要 CD_XXX 资产）：
 * 时长由 UPXGameplayAbility::ApplyCooldown 用 SetByCaller("AbilityCD", GetCooldownDuration()) 注入，
 * 冷却标签（Ability.X.CD）同样由 ApplyCooldown 动态写入 DynamicGrantedTags，本类不再做其它事。
 */
UCLASS()
class PIXELBLITZ_API UPXCooldownGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UPXCooldownGameplayEffect();
};
