// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/GA_Leverage.h"

#include "Character/BasePXCharacter.h"
#include "Character/Components/AbilityComponent.h"
#include "GAS/PXCooldownGameplayEffect.h"
#include "PixelBlitz/PixelBlitz.h"
#include "Subsystems/ComboSubsystem.h"
#include "Settings/Config/PXResourceDataAsset.h"

UGA_Leverage::UGA_Leverage()
{
	// UAbilityComponent::ActivateAbilityByTiming 通过 GetAssetTags() 精确匹配天赋 Tag 后触发本技能
	// 触发时机为 BeAttackedInvulnerable（完美闪避），由天赋表 Timing 列配置
	SetAssetTags(FGameplayTagContainer(TAG("Ability.Leverage")));

	// 冷却：通用 C++ 冷却 GE + 动态授予 Ability.Leverage.CD，无需 BGA 子类与 CD_XXX 资产
	CooldownGameplayEffectClass = UPXCooldownGameplayEffect::StaticClass();
	CooldownTags.AddTag(TAG("Ability.Leverage.CD"));
	CooldownDuration = FScalableFloat(0.5f);	// 兜底值，天赋 ExtendData 可覆盖
}

float UGA_Leverage::GetCooldownDuration_Implementation() const
{
	// 天赋数值走 AbilityComponent 的 ExtendData：Ability.Leverage.CD = 冷却秒数
	if (const ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(GetPawn()))
	{
		if (UAbilityComponent* AbilityComponent = PXCharacter->AbilityComponent)
		{
			float CooldownSeconds = 0.f;
			if (AbilityComponent->FindExtendData(TAG("Ability.Leverage.CD"), CooldownSeconds) && CooldownSeconds > 0.f)
			{
				return CooldownSeconds;
			}
		}
	}

	return Super::GetCooldownDuration_Implementation();
}

void UGA_Leverage::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(GetPawn());
	if (!PXCharacter || !PXCharacter->Implements<UFight_Interface>() ||
		!IFight_Interface::Execute_IsAlive(PXCharacter))
	{
		K2_EndAbility();
		return;
	}

	// 完美闪避：额外累积 1 层弱点连击（内部已处理封顶、音阶音效、UI 刷新、衰减重排与 BGM 闪避）
	if (UComboSubsystem* ComboSubsystem = UComboSubsystem::GetInstance(PXCharacter))
	{
		ComboSubsystem->AddComboWeakPoint(1);
	}

	K2_EndAbility();
}
