#pragma once

#include "CoreMinimal.h"
#include "GAS/PXGameplayAbility.h"
#include "GA_StepStrike.generated.h"

/**
 * 踏击：跨平台跳跃落地后施加增益（下一次攻击伤害提升），攻击结束/技能结束时消耗。
 * 单 GA 双事件：主触发 = CrossPlatformLanding（Talent.csv Timing 列）；
 * 额外触发 = AttackFinish / AttackSkillEnd（ExtraTriggerTimings，InitTalents 注册）。
 * 两条路径共用 AssetTag "Ability.StepStrike"，在 ActivateAbility 内读
 * AbilityComponent->LastActivatedTiming 区分本次触发事件。
 * 替代原 ABaseSkill Actor 型天赋技能 Skill_StepStrike。
 */
UCLASS()
class PIXELBLITZ_API UGA_StepStrike : public UPXGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_StepStrike();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** 落地：施加天赋表 AttributeEffectOnActivated（CurAttackValue +20%）并显示 buff 图标 */
	void ApplyOnLanding();

	/** 攻击/技能结束：仅在增益存在时移除（与原 ClearStoredBuff 行为一致） */
	void ConsumeOnAttackEnd();
};
