#include "GAS/Ability/GA_StepStrike.h"

#include "Character/BasePXCharacter.h"
#include "Character/Components/AbilityComponent.h"
#include "Character/Components/BuffComponent.h"
#include "PixelBlitz/PixelBlitz.h"

UGA_StepStrike::UGA_StepStrike()
{
	// UAbilityComponent::ActivateAbilityByTiming 通过 GetAssetTags() HasTagExact 精确匹配后触发
	SetAssetTags(FGameplayTagContainer(TAG("Ability.StepStrike")));

	// 额外触发时机：由 AbilityComponent::InitTalents 把本 GA 的 AssetTag 注册进对应触发表
	ExtraTriggerTimings = { EAbilityTiming::AttackFinish, EAbilityTiming::AttackSkillEnd };
}

void UGA_StepStrike::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(GetPawn());
	if (!PXCharacter || !PXCharacter->AbilityComponent || !PXCharacter->BuffComponent)
	{
		K2_EndAbility();
		return;
	}

	if (PXCharacter->AbilityComponent->GetLastActivatedTiming() == EAbilityTiming::CrossPlatformLanding)
	{
		ApplyOnLanding();
	}
	else
	{
		ConsumeOnAttackEnd();
	}

	K2_EndAbility();
}

void UGA_StepStrike::ApplyOnLanding()
{
	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(GetPawn());
	if (!PXCharacter || !PXCharacter->AbilityComponent || !PXCharacter->BuffComponent) return;

	// 与原 Skill_StepStrike::OnCrossPlatformLanding 的 Super 行为一致：
	// 施加天赋表 AttributeEffectOnActivated（CurAttackValue +20%，持续 9999s，即驻留到被消耗）
	PXCharacter->AbilityComponent->ApplyAttributeEffects(TAG("Ability.StepStrike"));

	// buff 图标显示（BuffOnWidget 表，Tag = Ability.StepStrike）
	PXCharacter->BuffComponent->AddBuffByTag(TAG("Ability.StepStrike"));
}

void UGA_StepStrike::ConsumeOnAttackEnd()
{
	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(GetPawn());
	if (!PXCharacter || !PXCharacter->BuffComponent) return;

	// 与原 Skill_StepStrike::OnAttackFinish / OnAttackSkillEnd（ClearStoredBuff）行为一致
	if (PXCharacter->BuffComponent->BuffExist(TAG("Ability.StepStrike")))
	{
		PXCharacter->BuffComponent->RemoveBuff(TAG("Ability.StepStrike"));
	}
}
