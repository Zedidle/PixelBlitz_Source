// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/TalentComponent.h"

#include "Character/Components/BuffComponent.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Fight/Components/EnergyComponent.h"
#include "Fight/Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/TimerSubsystemFuncLib.h"

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
			SpawnSkill(TalentData.SkillClass);
			PXCharacter->BuffComponent->AddBuffByTag(TalentData.BuffTagOnWidget);
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
	
	
	InitTalents();

	// 对 OnPlayerAttack 的绑定，在玩家发起攻击时的事件
	PXCharacter->OnPlayerAttackStart.AddDynamic(this, &UTalentComponent::Event_OnPlayerAttackStart);
}

void UTalentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);

	UTimerSubsystemFuncLib::CancelDelay(GetWorld(), "Talent_WarmingUP");
	
	PXCharacter->OnPlayerAttackStart.RemoveDynamic(this, &UTalentComponent::Event_OnPlayerAttackStart);
}


// Called every frame
void UTalentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTalentComponent::Event_OnPlayerAttackStart()
{
	MakeSwingFistPower();
	
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
	}, false);
}

void UTalentComponent::OnBeAttacked(AActor* Maker, int InDamage, int& OutDamage, bool bForce)
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

	
	int RemDamage = InDamage;

	for (int Index = 0; Index < DefenseSkills.Num(); ++Index)
	{
		if (!DefenseSkills[Index]) continue;
	
		bool stop;
		int _OutDamage;
		DefenseSkills[Index]->OnBeAttacked(Maker, RemDamage, _OutDamage, stop);

		// 某些防御性技能不只是削减伤害
		if (!bForce)
		{
			RemDamage = _OutDamage;
		}
		if (stop) break;
	}
	
	OutDamage = RemDamage;
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

	// 起死回生 / 被怪物击杀的复活次数
	TalentTag = TAG("TalentSet.ReviveTimesPlus");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		PXCharacter->HereReviveTimes += FMath::RoundToInt(EffectGameplayTags[TalentTag]);
	}

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
				TAG("Buff.Talent.EagleEye"), EffectGameplayTags[TalentTag], 0, 999);
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
	TalentTag = TAG("TalentSet.Warmup.AttackDamagePlusPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		UTimerSubsystemFuncLib::SetDelayLoopSafe(GetWorld(), "Talent_WarmingUP",
			this, &UTalentComponent::MoveWarmingUP, 0.2);
	}

	// 摇摆拳
	TalentTag = TAG("Buff.Talent.SwingFist");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		MakeSwingFistPower();
	}

	// 不灭
	MakeImmortalPower(true);


	
}

ABaseSkill* UTalentComponent::SpawnSkill(TSubclassOf<ABaseSkill> Skill_Class, const FTransform& SpawnTransform)
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;
	if (!Skill_Class) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PXCharacter;
	SpawnParams.Instigator = PXCharacter;
	
	if (ABaseSkill* Skill = World->SpawnActor<ABaseSkill>(Skill_Class, SpawnTransform, SpawnParams))
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
	
	// 清楚热身运动
	if (WarmUP_Power > 0)
	{
		IBuff_Interface::Execute_RemoveBuff(PXCharacter, TAG("Buff.Talent.Warmup"), true);
		WarmUP_Power = 0 ;
		WarmUP_MoveDistance = 0;
		OwnerPreLocation = PXCharacter->GetActorLocation();
	}

	MakeMiracleWalker();
	MakeImmortalPower(false);

	IBuff_Interface::Execute_RemoveBuff(PXCharacter, TAG("Buff.Talent.StaticPower"), true);
	IBuff_Interface::Execute_RemoveBuff(PXCharacter, TAG("Buff.Talent.DodgeStrike"), true);
	
}

void UTalentComponent::OnDashEnd()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	if (!PXCharacter->Implements<UBuff_Interface>()) return;

	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	// 闪避突袭
	FGameplayTag DamagePlus_AfterDashTag = TAG("TalentSet.DamagePlus_AfterDash");
	if (EffectGameplayTags.Contains(DamagePlus_AfterDashTag))
	{
		FGameplayTag Tag = TAG("Buff.Talent.DodgeStrike");
		IBuff_Interface::Execute_BuffEffect_Attack(PXCharacter, Tag, 0.0f,
			EffectGameplayTags[DamagePlus_AfterDashTag], 999.0f
		);

		if (PXCharacter->BuffComponent)
		{
			PXCharacter->BuffComponent->AddBuffByTag(Tag);
		}
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
	FGameplayTag WarmUP_Tag = TAG("Buff.Talent.Warmup");
	
	FText BuffNameFormat = LOCTEXT("Buff_Warmup", "热身{0}");
	
	IBuff_Interface::Execute_BuffEffect_Attack(PXCharacter, WarmUP_Tag, PlusPowerPercent, 0, 999);
	IBuff_Interface::Execute_AddBuff(PXCharacter, WarmUP_Tag, FText::Format(BuffNameFormat, WarmUP_Power).ToString(),
		FLinearColor( 0.25 * WarmUP_Power, 0.1, 0.1, 1.0), false);
}

void UTalentComponent::MakeSwingFistPower()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	
	FGameplayTag DecreaseTag = TAG("TalentSet.SwingPunch.AttackDamage_DecreasePercent");
	FGameplayTag IncreaseTag = TAG("TalentSet.SwingPunch.AttackDamage_IncreasePercent");

	if (!EffectGameplayTags.Contains(DecreaseTag) || !EffectGameplayTags.Contains(IncreaseTag)) return;



	if (!PXCharacter->Implements<UBuff_Interface>()) return;

	SwingFistPower = !SwingFistPower;
	
	FGameplayTag SwingFistTag = TAG("Buff.Talent.SwingFist");
	IBuff_Interface::Execute_RemoveBuff(PXCharacter, SwingFistTag, true);

	FText BuffNameFormat = LOCTEXT("Buff_SwingFist", "摇摆拳{0}");
	if (SwingFistPower)
	{
		IBuff_Interface::Execute_BuffEffect_Attack(PXCharacter, SwingFistTag, EffectGameplayTags[IncreaseTag], 0, 999);
		IBuff_Interface::Execute_AddBuff(PXCharacter, SwingFistTag,  FText::Format(BuffNameFormat, FText::FromString(TEXT("↓"))).ToString(),
			FLinearColor(0.093059, 0.027321, 0.0, 1), false);
	}
	else
	{
		IBuff_Interface::Execute_BuffEffect_Attack(PXCharacter, SwingFistTag, EffectGameplayTags[DecreaseTag], 0, 999);
		IBuff_Interface::Execute_AddBuff(PXCharacter, SwingFistTag,  FText::Format(BuffNameFormat, FText::FromString(TEXT("↑"))).ToString(),
			FLinearColor(1.0, 0.296138, 0.0, 1), false);
	}
}

void UTalentComponent::MakeMiracleWalker()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	if (!PXCharacter->Implements<UBuff_Interface>()) return;
	
	FGameplayTag MiracleWalkerTag = TAG("Buff.Talent.MiracleWalker");
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
	FGameplayTag ImmortalPowerTag = TAG("Buff.Talent.ImmortalPower");

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
