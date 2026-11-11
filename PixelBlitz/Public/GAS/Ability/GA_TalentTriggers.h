#pragma once

#include "CoreMinimal.h"
#include "GAS/PXGameplayAbility.h"
#include "GA_TalentTriggers.generated.h"

/**
 * 天赋触发型 GA 集合：批量替代 ABaseSkill Actor 型天赋技能
 * （Skill_StepWindWave / Skill_EvadeStrike / Skill_SwingFist / Skill_GoldHunter /
 *   Skill_MiracleWalker / Skill_Immortal，见 Talent.csv 对应行）。
 * 共同约定（与 GA_StepStrike / GA_Leverage 一致）：
 *  - UAbilityComponent::ActivateAbilityByTiming 通过 GetAssetTags() HasTagExact 匹配天赋 Tag 后触发；
 *  - 同一天赋多事件时用 ExtraTriggerTimings 声明，GA 内读 GetLastActivatedTiming() 分流；
 *  - 增益状态统一放 BuffComponent（按天赋 Tag 增删），GA 只做无状态触发壳；
 *  - 需要跨激活保留的少量状态（摇摆相位/就绪标记/重触发定时器）放 InstancedPerActor 实例成员
 *    （实例授予时创建，EndAbility 不销毁）。
 */

// 踏风波：跨平台跳跃落地后爆发短暂大量移速（Timing = CrossPlatformLanding）
UCLASS()
class PIXELBLITZ_API UGA_StepWindWave : public UPXGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_StepWindWave();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};

// 闪避突袭：技能结束后提升下次攻击伤害（主 Timing = SkillEnd），攻击结束消耗（Extra = AttackFinish）
// 注意：原 Actor 版 CSV Timing 误填为无效枚举值 "SkillFinish"（枚举里是 SkillEnd），
// 导致该天赋实际从未生效过；GA 版已按天赋描述修正为 SkillEnd。
UCLASS()
class PIXELBLITZ_API UGA_EvadeStrike : public UPXGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_EvadeStrike();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};

// 醉拳：攻击伤害摇摆 -20%/+50% 交替（Timing = AttackFinish + Extra = AttackSkillEnd，两者行为一致）
UCLASS()
class PIXELBLITZ_API UGA_SwingFist : public UPXGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_SwingFist();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 摇摆相位（InstancedPerActor 实例持久，对应原 SwingFistPower 初始值 false：首次为 +50%）
	bool SwingFistPower = false;

	void MakeSwingFistPower();
};

// 赏金猎人：拾取金币时短时提速并恢复能量（Timing = PickGold）
UCLASS()
class PIXELBLITZ_API UGA_GoldHunter : public UPXGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_GoldHunter();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};

// 奇迹行者：一定时间内未攻击则叠加增伤，攻击时移除并重置计时（Timing = AttackEffect）
UCLASS()
class PIXELBLITZ_API UGA_MiracleWalker : public UPXGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_MiracleWalker();

protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 重置"未攻击计时"并移除待命增益（对应原 PrepareEffect；OnGiveAbility 与每次攻击时调用）
	void PrepareEffect(ABasePXCharacter* PXCharacter);
};

// 不灭：周期性叠加最大生命值，攻击时消耗（Timing = AttackEffect）
UCLASS()
class PIXELBLITZ_API UGA_Immortal : public UPXGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Immortal();

protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 就绪标记（InstancedPerActor 实例持久，对应原 bReady）
	bool bReady = false;

	void BeginPrepare(ABasePXCharacter* PXCharacter);
	void MakeEffect(ABasePXCharacter* PXCharacter);
};
