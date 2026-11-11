#include "GAS/Ability/GA_TalentTriggers.h"

#include "Character/BasePXCharacter.h"
#include "Character/Components/AbilityComponent.h"
#include "Character/Components/BuffComponent.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Fight/Components/StateComponent.h"
#include "PixelBlitz/PixelBlitz.h"
#include "Subsystems/TimerManagerFuncLib.h"

#define LOCTEXT_NAMESPACE "PX"

// ================= 踏风波 =================

UGA_StepWindWave::UGA_StepWindWave()
{
	SetAssetTags(FGameplayTagContainer(TAG("Ability.StepWindWave")));
}

void UGA_StepWindWave::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

	// 与原 Skill_StepWindWave::OnCrossPlatformLanding 一致：
	// 施加 AttributeEffectOnActivated（CurSpeed 爆发移速，持续 0.5s）并显示 buff
	PXCharacter->AbilityComponent->ApplyAttributeEffects(TAG("Ability.StepWindWave"));
	PXCharacter->BuffComponent->AddBuffByTag(TAG("Ability.StepWindWave"));

	K2_EndAbility();
}

// ================= 闪避突袭 =================

UGA_EvadeStrike::UGA_EvadeStrike()
{
	SetAssetTags(FGameplayTagContainer(TAG("Ability.EvadeStrike")));
	ExtraTriggerTimings = { EAbilityTiming::AttackFinish };
}

void UGA_EvadeStrike::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

	if (PXCharacter->AbilityComponent->GetLastActivatedTiming() == EAbilityTiming::SkillEnd)
	{
		// BuffExist 在此检查点恒为 true（Super 每次激活都施加并注册 Tag，且 AttackFinish 每次攻击
		// 结束都会触发本 GA），无法作为门控。改为刷新语义：先移除残留增益（可能未被消耗）再重新施加，
		// 既保证每次 SkillEnd 都获得完整增益，又避免 AddBuffEffect 同 Tag 叠层。
		PXCharacter->BuffComponent->RemoveBuff(TAG("Ability.EvadeStrike"));

		// 重新施加 AttributeEffectOnActivated（CurDamagePlusAfterDash +10，被上面的 RemoveBuff 一并清掉），
		// 再读取并转为攻击增益
		PXCharacter->AbilityComponent->ApplyAttributeEffects(TAG("Ability.EvadeStrike"));

		const float DamagePlusValue = PXCharacter->CachedASC
			? PXCharacter->CachedASC->GetAttributeValue(EPXAttribute::CurDamagePlusAfterDash)
			: 0.f;

		PXCharacter->BuffComponent->AddAttributeEffect(TAG("Ability.EvadeStrike"),
			FAttributeEffect(EPXAttribute::CurAttackValue, 0.0f, DamagePlusValue));
		PXCharacter->BuffComponent->AddBuffByTag(TAG("Ability.EvadeStrike"));
	}
	else
	{
		// 与原 OnAttackFinish 一致：消耗增益
		PXCharacter->BuffComponent->RemoveBuff(TAG("Ability.EvadeStrike"));
	}

	K2_EndAbility();
}

// ================= 醉拳 =================

UGA_SwingFist::UGA_SwingFist()
{
	SetAssetTags(FGameplayTagContainer(TAG("Ability.SwingFist")));
	ExtraTriggerTimings = { EAbilityTiming::AttackSkillEnd };
	// 摇摆相位需跨激活保留：InstancedPerActor 实例授予时创建，EndAbility 不销毁
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_SwingFist::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	MakeSwingFistPower();

	K2_EndAbility();
}

void UGA_SwingFist::MakeSwingFistPower()
{
	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(GetPawn());
	if (!PXCharacter || !PXCharacter->AbilityComponent || !PXCharacter->BuffComponent) return;

	FEffectGameplayTags& AbilityExtendData = PXCharacter->AbilityComponent->AbilityExtendData;

	// 与原 Skill_SwingFist::MakeSwingFistPower 一致：相位决定本次施加减伤还是增伤
	FGameplayTag CalTag = SwingFistPower
		? TAG("Ability.SwingFist.Set.AttackDamageMinusPercent")
		: TAG("Ability.SwingFist.Set.AttackDamagePlusPercent");
	if (!AbilityExtendData.Contains(CalTag)) return;

	PXCharacter->BuffComponent->RemoveBuff(TAG("Ability.SwingFist"));

	FText BuffNameFormat = LOCTEXT("Buff_SwingFist", "醉拳{0}");

	FAttributeEffect Effect = FAttributeEffect(EPXAttribute::CurAttackValue, AbilityExtendData[CalTag]);
	PXCharacter->BuffComponent->AddAttributeEffect(TAG("Ability.SwingFist"), Effect);

	if (SwingFistPower)
	{
		PXCharacter->BuffComponent->AddBuffOnWidget(TAG("Ability.SwingFist"),
			FText::Format(BuffNameFormat, FText::FromString(TEXT("↓"))).ToString(),
			FLinearColor(0.093059, 0.027321, 0.0, 1), false);
	}
	else
	{
		PXCharacter->BuffComponent->AddBuffOnWidget(TAG("Ability.SwingFist"),
			FText::Format(BuffNameFormat, FText::FromString(TEXT("↑"))).ToString(),
			FLinearColor(1.0, 0.296138, 0.0, 1), false);
	}

	SwingFistPower = !SwingFistPower;
}

// ================= 赏金猎人 =================

UGA_GoldHunter::UGA_GoldHunter()
{
	SetAssetTags(FGameplayTagContainer(TAG("Ability.GoldHunter")));
}

void UGA_GoldHunter::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(GetPawn());
	if (!PXCharacter || !PXCharacter->AbilityComponent)
	{
		K2_EndAbility();
		return;
	}

	// 与原 Skill_GoldHunter::OnPickGold（经 Super::OnPickGold）一致：
	// 施加 AttributeEffectOnActivated（CurSpeed/EP 短时增益，持续 2s）
	PXCharacter->AbilityComponent->ApplyAttributeEffects(TAG("Ability.GoldHunter"));

	K2_EndAbility();
}

// ================= 奇迹行者 =================

UGA_MiracleWalker::UGA_MiracleWalker()
{
	SetAssetTags(FGameplayTagContainer(TAG("Ability.MiracleWalker")));
	// 重触发定时器的生命周期长于单次激活：InstancedPerActor 实例持久
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_MiracleWalker::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	// 对应原 BeginPlay：天赋授予即开始"未攻击计时"
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		if (ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(ActorInfo->AvatarActor.Get()))
		{
			PrepareEffect(PXCharacter);
		}
	}
}

void UGA_MiracleWalker::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(GetPawn()))
	{
		// 与原 OnAttackEffect 一致：攻击即移除待命增益并重置计时
		PrepareEffect(PXCharacter);
	}

	K2_EndAbility();
}

void UGA_MiracleWalker::PrepareEffect(ABasePXCharacter* PXCharacter)
{
	if (!PXCharacter || !PXCharacter->AbilityComponent || !PXCharacter->BuffComponent) return;

	PXCharacter->BuffComponent->RemoveBuff(TAG("Ability.MiracleWalker"));

	FGameplayTag DamagePlusTag = TAG("Ability.MiracleWalker.Set.DamagePlus");
	FGameplayTag IntervalTag = TAG("Ability.MiracleWalker.CD");
	FEffectGameplayTags& ExtendData = PXCharacter->AbilityComponent->AbilityExtendData;
	if (!ExtendData.Contains(DamagePlusTag) || !ExtendData.Contains(IntervalTag)) return;

	const FName TimerName = FName(*(PXCharacter->GetName() + "_MakeMiracleWalker"));
	TWeakObjectPtr<ABasePXCharacter> WeakPX(PXCharacter);

	UTimerManagerFuncLib::SetRetriggerableDelay(PXCharacter->GetWorld(), TimerName,
		[WeakPX, DamagePlusTag]()
		{
			ABasePXCharacter* PXCharacter = WeakPX.Get();
			if (!PXCharacter || !PXCharacter->AbilityComponent || !PXCharacter->BuffComponent) return;

			FEffectGameplayTags& ExtendData = PXCharacter->AbilityComponent->AbilityExtendData;
			if (ExtendData.Contains(DamagePlusTag))
			{
				PXCharacter->BuffComponent->AddAttributeEffect(TAG("Ability.MiracleWalker"),
					FAttributeEffect(EPXAttribute::CurAttackValue, 0.0f, ExtendData[DamagePlusTag]));
				PXCharacter->BuffComponent->AddBuffByTag(TAG("Ability.MiracleWalker"));
			}
		}, ExtendData[IntervalTag]);
}

// ================= 不灭 =================

UGA_Immortal::UGA_Immortal()
{
	SetAssetTags(FGameplayTagContainer(TAG("Ability.Immortal")));
	// bReady 与准备定时器跨激活保留
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Immortal::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	// 对应原 BeginPlay：授予即进入准备期
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		if (ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(ActorInfo->AvatarActor.Get()))
		{
			BeginPrepare(PXCharacter);
		}
	}
}

void UGA_Immortal::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(GetPawn()))
	{
		// 与原 OnAttackEffect → MakeEffect 一致：就绪则消耗并重新进入准备期
		MakeEffect(PXCharacter);
	}

	K2_EndAbility();
}

void UGA_Immortal::BeginPrepare(ABasePXCharacter* PXCharacter)
{
	bReady = false;
	if (!PXCharacter || !PXCharacter->AbilityComponent || !PXCharacter->BuffComponent || !PXCharacter->StateComponent) return;

	PXCharacter->BuffComponent->RemoveBuffOnWidget(TAG("Ability.Immortal"), true);

	float CooldownSeconds = 0.f;
	if (!PXCharacter->AbilityComponent->FindExtendData(TAG("Ability.Immortal.CD"), CooldownSeconds) || CooldownSeconds <= 0.f) return;

	FGameplayTag DamageTag = TAG("Ability.Immortal.Set.AttackDamagePlusOnMaxHPPercent");
	TWeakObjectPtr<UGA_Immortal> WeakSelf(this);
	TWeakObjectPtr<ABasePXCharacter> WeakPX(PXCharacter);

	UTimerManagerFuncLib::SetDelay(PXCharacter->GetWorld(),
		[WeakSelf, WeakPX, DamageTag]()
		{
			UGA_Immortal* Self = WeakSelf.Get();
			ABasePXCharacter* PXCharacter = WeakPX.Get();
			if (!Self || !PXCharacter || !PXCharacter->AbilityComponent || !PXCharacter->BuffComponent || !PXCharacter->StateComponent) return;

			float DamagePercent = 0.f;
			if (!PXCharacter->AbilityComponent->FindExtendData(DamageTag, DamagePercent)) return;

			// 与原 Skill_Immortal::BeginPrepare 定时器回调一致：按当前最大生命附加攻击增益并进入就绪
			PXCharacter->BuffComponent->AddAttributeEffect(TAG("Ability.Immortal"),
				FAttributeEffect(EPXAttribute::CurAttackValue, 0.0f,
					FMath::RoundToInt(PXCharacter->StateComponent->GetMaxHP() * DamagePercent)));
			PXCharacter->BuffComponent->AddBuffByTag(TAG("Ability.Immortal"));

			Self->bReady = true;
		}, CooldownSeconds);
}

void UGA_Immortal::MakeEffect(ABasePXCharacter* PXCharacter)
{
	if (!PXCharacter || !PXCharacter->AbilityComponent || !PXCharacter->BuffComponent || !PXCharacter->StateComponent) return;

	float HPEffect = 0.f;
	if (!PXCharacter->AbilityComponent->FindExtendData(TAG("Ability.Immortal.Set.MaxHPPlusAfterAttack"), HPEffect)) return;

	// 与原 Skill_Immortal::MakeEffect 一致：仅就绪时消耗
	if (!bReady) return;

	UPXMainSaveGame* MainSaveGame = UPXSaveGameSubSystemFuncLib::GetMainData(PXCharacter->GetWorld());
	if (!MainSaveGame) return;

	MainSaveGame->CharacterInheritAttribute.BasicMaxHP += HPEffect;

	PXCharacter->BuffComponent->RemoveBuff(TAG("Ability.Immortal"), true);
	PXCharacter->BuffComponent->AddAttributeEffect(TAG("Ability.Immortal"), EPXAttribute::BasicMaxHP, 0.0f, 0, HPEffect);
	PXCharacter->StateComponent->IncreaseHP(HPEffect, PXCharacter);

	BeginPrepare(PXCharacter);
}

#undef LOCTEXT_NAMESPACE
