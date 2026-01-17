// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/TalentComponent.h"
#include "Character/Components/BuffComponent.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Fight/Components/EnergyComponent.h"
#include "Fight/Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/PXASComponent.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "Utilitys/PXGameplayStatics.h"

#define LOCTEXT_NAMESPACE "PX"

class UDataTableSubsystem;

void UTalentComponent::InitTalents()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->BuffComponent)
	UPXBasicBuildSaveGame* BasicBuildSaveGame = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BasicBuildSaveGame)

	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem);
	
	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;

	for (const FGameplayTag& TalentTag: BasicBuildSaveGame->ChosenTalents)
	{
		// 获取对应的Talent结构
		const FTalent& TalentData = DataTableSubsystem->GetTalentDataByTag(TalentTag);
		if (!TalentData.TalentTag.IsValid()) continue;

		for (auto& D: TalentData.Effect_GameplayTag)
		{
			EffectGameplayTags.AddData(D.Key, D.Value);
		}

		for (auto& AbilityClass: TalentData.AbilityClass)
		{
			if (UClass* LoadedClass = AbilityClass.LoadSynchronous())
			{
				FGameplayAbilitySpec Spec(LoadedClass);
				CachedASC->GiveAbility(Spec);
			}
		}
		
		if (TalentData.CommonInit)
		{
			if (ABaseSkill* Skill = SpawnSkill(TalentData.SkillClass))
			{
				Skill->SetActivateTiming(TalentData.Timing);
				SkillsHolding.Add(Skill);
			}
			if (PXCharacter->BuffComponent)
			{
				PXCharacter->BuffComponent->AddBuffByTag(TalentData.TalentTag, true);
			}
		}
		
		if (TalentData.Timing != EAbilityTiming::None)
		{
			FGameplayTagArray& TagArray = AbilitiesTiming.FindOrAdd(TalentData.Timing);
			TagArray.Tags.Add(TalentData.TalentTag);
		}
	}
}

// Sets default values for this component's properties
UTalentComponent::UTalentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTalentComponent::BeginPlay()
{
	Super::BeginPlay();

	PXCharacter = Cast<ABasePXCharacter>(GetOwner());
	OwnerPreLocation = PXCharacter->GetActorLocation();

	if (UPXASComponent* PXASComponent = Cast<UPXASComponent>(PXCharacter->GetAbilitySystemComponent()))
	{
		CachedASC = PXASComponent;
	}
	
	

	// 对 OnPlayerAttack 的绑定，在玩家发起攻击时的事件
	PXCharacter->OnPlayerAttackStart.AddDynamic(this, &UTalentComponent::OnAttackStart);
}

void UTalentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);

	UTimerSubsystemFuncLib::CancelDelay(GetWorld(), "Talent_WarmingUP");
	
	PXCharacter->OnPlayerAttackStart.RemoveDynamic(this, &UTalentComponent::OnAttackStart);
}


// Called every frame
void UTalentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTalentComponent::OnAttackStart(EAttackType Type, FVector Direction)
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnAttackStart();
		}
	}
	
	ActivateAbilityByTiming(EAbilityTiming::AttackStart);
}

void UTalentComponent::RegisterDefenseSkill(ABaseDefenseSkill* Skill)
{
	DefenseSkills.Add(Skill);
	int Index = DefenseSkills.Num() - 1;
	while (Index > 0)
	{
		if (IsValid(DefenseSkills[Index]) && IsValid(DefenseSkills[Index - 1]))
		{
			if (DefenseSkills[Index]->Priority > DefenseSkills[Index - 1]->Priority)
			{
				DefenseSkills.Swap(Index, Index - 1);
				Index --;
				continue;
			}
		}
		break;
	}
}

void UTalentComponent::RemoveDefenseSkill(FGameplayTag Tag)
{
	DefenseSkills.RemoveAllSwap([&Tag](const ABaseDefenseSkill* Skill)
	{
		return Skill && Skill->AbilityTags.HasTag(Tag);
	}, EAllowShrinking::No);
}


void UTalentComponent::LoadTalents()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->BuffComponent)
	UGameInstance* GameInstance = PXCharacter->GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)
	UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem)
	UPXBasicBuildSaveGame* BasicBuildSaveGame = SaveGameSubsystem->GetBasicBuildData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BasicBuildSaveGame)

	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)
	
	const FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	
	if (Loaded) return;
	Loaded = true;

	FGameplayTag TalentTag;

	// 基础移动速度
	TalentTag = TAG("TalentSet.SpeedPlusPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		PXCharacter->MaxWalkSpeed = PXCharacter->MaxWalkSpeed * (1 + EffectGameplayTags[TalentTag]);
		if (UCharacterMovementComponent* MovementComponent = PXCharacter->GetCharacterMovement())
		{
			MovementComponent->MaxWalkSpeed = PXCharacter->MaxWalkSpeed;
		}
	}

	// 基础跳跃高度
	TalentTag = TAG("TalentSet.BasicJumpHeightPlusPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		PXCharacter->JumpZVelocity = PXCharacter->JumpZVelocity * (1 + EffectGameplayTags[TalentTag]);
		if (UCharacterMovementComponent* MovementComponent = PXCharacter->GetCharacterMovement())
		{
			MovementComponent->JumpZVelocity = PXCharacter->JumpZVelocity;
		}
	}

	// 基础冲刺距离
	TalentTag = TAG("TalentSet.DashDistancePlusPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		PXCharacter->BasicDashSpeed = PXCharacter->BasicDashSpeed * (1 + EffectGameplayTags[TalentTag]);
	}


	// 视野
	TalentTag = TAG("TalentSet.SightPlusPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		if (PXCharacter->BuffComponent->Implements<UBuff_Interface>())
		{
			IBuff_Interface::Execute_BuffEffect_Sight(PXCharacter->BuffComponent,
				TAG("Ability.EagleEye"), EffectGameplayTags[TalentTag], 0, 999);
		}
	}
	

	// 最大生命值
	TalentTag = TAG("TalentSet.MaxHPPlus");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		if (PXCharacter->HealthComponent)
		{
			PXCharacter->HealthComponent->ModifyMaxHP(EffectGameplayTags[TalentTag], EStatChange::Increase, true);
		}
	}

	// 最大体力值
	TalentTag = TAG("TalentSet.MaxEPPlus");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		if (UEnergyComponent* EnergyComponent = PXCharacter->EnergyComponent)
		{
			EnergyComponent->SetMaxEP(EffectGameplayTags[TalentTag] + EnergyComponent->GetMaxEP());
		}
	}

	// 奇迹行者
	MakeMiracleWalker();

	// 体型 / 蚁人
	TalentTag = TAG("TalentSet.BodySizePlusPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		PXCharacter->SetScale(1 + EffectGameplayTags[TalentTag]);
	}
	
	// 热身
	TalentTag = TAG("Ability.Warmup.Set.AttackDamagePlusPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		UTimerSubsystemFuncLib::SetDelayLoopSafe(GetWorld(), "Ability.WarmUP",
			this, &UTalentComponent::MoveWarmingUP, 0.2);
	}

	// 不灭
	MakeImmortalPower(true);
	
}

ABaseSkill* UTalentComponent::SpawnSkill(TSubclassOf<ABaseSkill> SkillClass, const FTransform& SpawnTransform)
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;
	if (!SkillClass) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PXCharacter;
	SpawnParams.Instigator = PXCharacter;
	
	if (ABaseSkill* Skill = World->SpawnActor<ABaseSkill>(SkillClass, SpawnTransform, SpawnParams))
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
		Skill->AttachToActor(PXCharacter, AttachmentRules);

		if (ABaseDefenseSkill* DefenseSkill = Cast<ABaseDefenseSkill>(Skill))
		{
			RegisterDefenseSkill(DefenseSkill);
		}
		
		return Skill;
	}
	return nullptr;
}

void UTalentComponent::OnBuffCalDamage()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	if (!PXCharacter->Implements<UBuff_Interface>()) return;
	
	// 清除热身运动
	if (WarmUP_Power > 0)
	{
		IBuff_Interface::Execute_RemoveBuff(PXCharacter, TAG("Ability.Warmup"), true);
		WarmUP_Power = 0 ;
		WarmUP_MoveDistance = 0;
		OwnerPreLocation = PXCharacter->GetActorLocation();
	}

	MakeMiracleWalker();
	MakeImmortalPower(false);

	IBuff_Interface::Execute_RemoveBuff(PXCharacter, TAG("Ability.StaticPower"), true);
	IBuff_Interface::Execute_RemoveBuff(PXCharacter, TAG("Ability.EvadeStrike"), true);
}

void UTalentComponent::OnDying(int& RemReviveTimes)
{
	int MaxReviveTimes = -1;
	for (auto& Skill : SkillsHolding)
	{
		int TmpReviveTimes = -1;
		Skill->OnDying(TmpReviveTimes);
		if (MaxReviveTimes < TmpReviveTimes)
		{
			MaxReviveTimes = TmpReviveTimes;
		}
	}
	RemReviveTimes = MaxReviveTimes;

	ActivateAbilityByTiming(EAbilityTiming::Dying);
}

void UTalentComponent::OnSkillFinish()
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnSkillFinish();
		}
	}
	
	ActivateAbilityByTiming(EAbilityTiming::SkillFinish);
}

void UTalentComponent::OnKillEnemy()
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnKillEnemy();
		}
	}

	ActivateAbilityByTiming(EAbilityTiming::KillEnemy);
}

void UTalentComponent::ActivateAbilityByTiming(EAbilityTiming Timing)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	
	FGameplayTagArray TagArray = AbilitiesTiming.FindRef(Timing);
	for (auto& Tag : TagArray.Tags)
	{
		CachedASC->TryActivateAbilityByTag(Tag);
	}
}


int UTalentComponent::GetAttackDamagePlus()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(PXCharacter, 0)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(PXCharacter->HealthComponent, 0)
	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;

	int LocalPlus = 0;
	FGameplayTag Tag = TAG("TalentSet.Wushu.AttackDamagePlusOnCurHPPercent");
	if (EffectGameplayTags.Contains(Tag))
	{
		LocalPlus += FMath::RoundToInt(EffectGameplayTags[Tag] * PXCharacter->HealthComponent->GetCurrentHP()) ;
	}

	// …… 其它技能
	return LocalPlus;
}

bool UTalentComponent::GetIsAttacking()
{
	return false;
}

bool UTalentComponent::GetIsDefending()
{
	return false;
}

void UTalentComponent::OnAttackHit_Implementation(AActor* Receiver)
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnAttackHit(Receiver);
		}
	}

	ActivateAbilityByTiming(EAbilityTiming::AttackHit);
}

void UTalentComponent::OnSkillHit_Implementation()
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnSkillHit();
		}
	}

	ActivateAbilityByTiming(EAbilityTiming::SkillHit);
}

void UTalentComponent::OnBeAttacked_Implementation(AActor* Maker, int InDamage, int& OutDamage, bool bForce)
{
	if (InDamage <= 0)
	{
		OutDamage = 0;
		return;
	}
	
	if (!Maker)
	{
		OutDamage = InDamage;
		return;
	}

	ActivateAbilityByTiming(EAbilityTiming::BeAttacked);
	
	int RemDamage = InDamage;

	for (auto& Skill : DefenseSkills)
	{
		if (!Skill) continue;
	
		bool stop;
		int _OutDamage;
		Skill->OnBeAttacked(Maker, RemDamage, _OutDamage, stop);

		// 某些防御性技能不只是削减伤害
		if (!bForce)
		{
			RemDamage = _OutDamage;
		}
		if (stop) break;
	}
	
	OutDamage = RemDamage;
}

void UTalentComponent::OnAttackWeakPoint_Implementation(AActor* Receiver)
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnHitWeakPoint(Receiver);
		}
	}
	
	ActivateAbilityByTiming(EAbilityTiming::AttackWeakPoint);
}

void UTalentComponent::OnAttackEffect_Implementation()
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnAttackEffect();
		}
	}
	
	ActivateAbilityByTiming(EAbilityTiming::AttackEffect);
}

void UTalentComponent::OnPickGold_Implementation()
{
	for (auto& Skill : SkillsHolding)
	{
		if (Skill.IsValid())
		{
			Skill->OnPickGold();
		}
	}

	ActivateAbilityByTiming(EAbilityTiming::PickGold);
}

void UTalentComponent::MoveWarmingUP()
{
	if (WarmUP_Power >= 4) return;
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);

	WarmUP_MoveDistance += FVector::Distance(PXCharacter->GetActorLocation(), OwnerPreLocation);
	OwnerPreLocation = PXCharacter->GetActorLocation();

	FGameplayTag MoveDistancePerLevelTag = TAG("TalentSet.Warmup.MoveDistancePerLevel");
	FGameplayTag AttackDamagePlusPercentTag = TAG("TalentSet.Warmup.AttackDamagePlusPercent");

	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	
	if (!EffectGameplayTags.Contains(MoveDistancePerLevelTag) ||
		!EffectGameplayTags.Contains(AttackDamagePlusPercentTag)
	) return;

	if (WarmUP_MoveDistance < EffectGameplayTags[MoveDistancePerLevelTag]) return;

	WarmUP_MoveDistance -= EffectGameplayTags[MoveDistancePerLevelTag];
	WarmUP_Power ++;

	if (!PXCharacter->Implements<UBuff_Interface>()) return;
	
	float PlusPowerPercent = WarmUP_Power * EffectGameplayTags[AttackDamagePlusPercentTag];
	FGameplayTag WarmUP_Tag = TAG("Ability.Warmup");
	
	FText BuffNameFormat = LOCTEXT("Buff_Warmup", "热身{0}");
	
	IBuff_Interface::Execute_BuffEffect_Attack(PXCharacter, WarmUP_Tag, PlusPowerPercent, 0, 999);
	IBuff_Interface::Execute_AddBuffOnWidget(PXCharacter, WarmUP_Tag, FText::Format(BuffNameFormat, WarmUP_Power).ToString(),
		FLinearColor( 0.25 * WarmUP_Power, 0.1, 0.1, 1.0), false);
}

void UTalentComponent::MakeMiracleWalker()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	if (!PXCharacter->Implements<UBuff_Interface>()) return;
	
	FGameplayTag MiracleWalkerTag = TAG("Ability.MiracleWalker");
	IBuff_Interface::Execute_RemoveBuff(PXCharacter, MiracleWalkerTag, true);

	FGameplayTag DamagePlusTag = TAG("TalentSet.MiracleWalker.DamagePlus");
	FGameplayTag IntervalTag = TAG("TalentSet.MiracleWalker.Interval");


	if (!EffectGameplayTags.Contains(DamagePlusTag) || !EffectGameplayTags.Contains(IntervalTag)) return;

	FName TimerName = FName(GetReadableName() + "_MakeMiracleWalker");
	UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), TimerName,
		[WeakThis = TWeakObjectPtr<ThisClass>(this), DamagePlusTag, MiracleWalkerTag]
		{
			if (!WeakThis.IsValid()) return;
			if (!WeakThis->PXCharacter) return;

			FEffectGameplayTags& EffectGameplayTags = WeakThis->PXCharacter->EffectGameplayTags;
			if (EffectGameplayTags.Contains(DamagePlusTag))
			{
				IBuff_Interface::Execute_BuffEffect_Attack(WeakThis->PXCharacter, MiracleWalkerTag, 0.0,
					EffectGameplayTags[DamagePlusTag], 999
				);
				if (WeakThis->PXCharacter->BuffComponent)
				{
					WeakThis->PXCharacter->BuffComponent->AddBuffByTag(MiracleWalkerTag);
				}
			}
		}, EffectGameplayTags[IntervalTag]);
}

void UTalentComponent::MakeImmortalPower(bool First)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->BuffComponent)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->HealthComponent)
	if (!PXCharacter->Implements<UBuff_Interface>()) return;
	
	FGameplayTag AttackDamagePlusOnMaxHPPercentTag = TAG("TalentSet.Immortal.AttackDamagePlusOnMaxHPPercent");
	FGameplayTag IntervalTag = TAG("TalentSet.Immortal.Interval");
	FGameplayTag MaxHPPlusAfterAttackTag = TAG("TalentSet.Immortal.MaxHPPlusAfterAttack");
	
	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	
	if (!EffectGameplayTags.Contains(AttackDamagePlusOnMaxHPPercentTag) || 
		!EffectGameplayTags.Contains(IntervalTag) ||
		!EffectGameplayTags.Contains(MaxHPPlusAfterAttackTag)
	) return;
	FGameplayTag ImmortalPowerTag = TAG("Ability.Immortal");

	if (First || PXCharacter->BuffComponent->BuffExist(ImmortalPowerTag))
	{
		UTimerSubsystemFuncLib::SetDelay(GetWorld(),
	[WeakThis = TWeakObjectPtr(this), AttackDamagePlusOnMaxHPPercentTag, ImmortalPowerTag]
		{
			if (!WeakThis.IsValid()) return;
			if (!WeakThis->PXCharacter) return;
			if (!WeakThis->PXCharacter->HealthComponent) return;
		
			FEffectGameplayTags& EffectGameplayTags = WeakThis->PXCharacter->EffectGameplayTags;
			if (!EffectGameplayTags.Contains(AttackDamagePlusOnMaxHPPercentTag)) return;
												
			IBuff_Interface::Execute_BuffEffect_Attack(WeakThis->PXCharacter, ImmortalPowerTag, 0.0f,
				FMath::RoundToInt(WeakThis->PXCharacter->HealthComponent->GetMaxHP() * EffectGameplayTags[AttackDamagePlusOnMaxHPPercentTag]),
				999.f
			);
			if (WeakThis->PXCharacter->BuffComponent)
			{
				WeakThis->PXCharacter->BuffComponent->AddBuffByTag(ImmortalPowerTag);
			}
			UGameInstance* GameInstance = WeakThis->PXCharacter->GetGameInstance();
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);
			UPXMainSaveGame* MainSaveGame= UPXSaveGameSubSystemFuncLib::GetMainData(GameInstance->GetWorld());
			MainSaveGame->CharacterInheritAttribute.MaxHP ++;
		}, EffectGameplayTags[IntervalTag]);
	}

	if (!First && PXCharacter->BuffComponent->BuffExist(ImmortalPowerTag))
	{
		IBuff_Interface::Execute_RemoveBuff(PXCharacter, ImmortalPowerTag, true);
		PXCharacter->HealthComponent->ModifyMaxHP(1, EStatChange::Increase, true);
	}
}



#undef LOCTEXT_NAMESPACE
