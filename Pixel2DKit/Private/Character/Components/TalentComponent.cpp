// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/TalentComponent.h"

#include "Character/Components/BuffComponent.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Fight/Components/EnergyComponent.h"
#include "Fight/Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/DataTableSettings.h"
#include "Settings/SkillSettings.h"
#include "Utilitys/CommonFuncLib.h"


void UTalentComponent::InitTalents()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	UPXBasicBuildSaveGame* BasicBuildSaveGame = UPXSaveGameSubSystemFuncLib::GetBasicBuildData(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(BasicBuildSaveGame)

	const UDataTableSettings& DataTableSettings = UDataTableSettings::Get();
	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	
	for (const FGameplayTag& TalentTag: BasicBuildSaveGame->ChosenTalents)
	{
		// 获取对应的Talent结构
		const FTalent* Data = DataTableSettings.GetTalentDataByTag(TalentTag);
		if (!Data) continue;

		for (auto& D: Data->Effect_GameplayTag)
		{
			EffectGameplayTags.AddData(D.Key, D.Value);
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

	InitTalents();

	// 对 OnPlayerAttack 的绑定，在玩家发起攻击时的事件
	PXCharacter->OnPlayerAttackStart.AddDynamic(this, &UTalentComponent::Event_OnPlayerAttackStart);
}

void UTalentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);
	
	PXCharacter->OnPlayerAttackStart.RemoveDynamic(this, &UTalentComponent::Event_OnPlayerAttackStart);
	
	if (MiracleWalker_TimerHandle.IsValid())
	{
		PXCharacter->GetGameInstance()->GetTimerManager().ClearTimer(MiracleWalker_TimerHandle);
	}

	if (WarmUP_TimerHandle.IsValid())
	{
		PXCharacter->GetGameInstance()->GetTimerManager().ClearTimer(WarmUP_TimerHandle);
	}

	if (ImmortalPower_TimerHandle.IsValid())
	{
		PXCharacter->GetGameInstance()->GetTimerManager().ClearTimer(ImmortalPower_TimerHandle);
	}
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
	for (int32 i = DefenseSkills.Num() - 1; i >= 0; --i)
	{
		if (DefenseSkills[i] && DefenseSkills[i]->AbilityTags.HasTag(Tag))
		{
			DefenseSkills.RemoveAt(i);
		}
	}
}

void UTalentComponent::OnBeAttacked(AActor* Maker, int InDamage, int& OutDamage)
{
	if (!Maker)
	{
		OutDamage = InDamage;
		return;
	}
	
	int PreDamage = InDamage;
	for (int Index = 0; Index < DefenseSkills.Num(); ++Index)
	{
		if (!DefenseSkills[Index]) continue;
		
		bool stop;
		int _OutDamage;
		DefenseSkills[Index]->OnBeAttacked(Maker, PreDamage, _OutDamage, stop);
		PreDamage = _OutDamage;
		
		if (stop) break;
	}
	
	OutDamage = PreDamage;
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
	const USkillSettings* SkillSettings = GetDefault<USkillSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SkillSettings)
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)
	
	const FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	
	if (Loaded) return;
	Loaded = true;

	FGameplayTag TalentTag;

	// 起死回生 / 被怪物击杀的复活次数
	TalentTag = FGameplayTag::RequestGameplayTag("TalentSet.ReviveTimesPlus");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		PXCharacter->ReviveTimes += FMath::RoundToInt(EffectGameplayTags[TalentTag]);
		PXCharacter->BuffComponent->AddBuffByTag(FGameplayTag::RequestGameplayTag("Buff.Talent.Revive"));
	}

	// 基础移动速度
	TalentTag = FGameplayTag::RequestGameplayTag("TalentSet.SpeedPlusPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		PXCharacter->BasicMoveSpeed = PXCharacter->BasicMoveSpeed * (1 + EffectGameplayTags[TalentTag]);
		if (UCharacterMovementComponent* MovementComponent = PXCharacter->GetCharacterMovement())
		{
			MovementComponent->MaxWalkSpeed = PXCharacter->BasicMoveSpeed;
		}
	}

	// 基础跳跃高度
	TalentTag = FGameplayTag::RequestGameplayTag("TalentSet.BasicJumpHeightPlusPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		PXCharacter->BasicJumpZ_Velocity = PXCharacter->BasicJumpZ_Velocity * (1 + EffectGameplayTags[TalentTag]);
		if (UCharacterMovementComponent* MovementComponent = PXCharacter->GetCharacterMovement())
		{
			MovementComponent->MaxWalkSpeed = PXCharacter->BasicJumpZ_Velocity;
		}
	}

	// 基础冲刺距离
	TalentTag = FGameplayTag::RequestGameplayTag("TalentSet.DashDistancePlusPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		PXCharacter->BasicDashSpeed = PXCharacter->BasicDashSpeed * (1 + EffectGameplayTags[TalentTag]);
	}


	// 视野
	TalentTag = FGameplayTag::RequestGameplayTag("TalentSet.SightPlusPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		if (PXCharacter->BuffComponent->Implements<UBuff_Interface>())
		{
			FGameplayTag EagleEyeTag = FGameplayTag::RequestGameplayTag("Buff.Talent.EagleEye");
			IBuff_Interface::Execute_BuffEffect_Sight(PXCharacter->BuffComponent,
				EagleEyeTag, EffectGameplayTags[TalentTag], 0, 999);
			PXCharacter->BuffComponent->AddBuffByTag(EagleEyeTag);
		}
	}

	// 减速抵抗
	TalentTag = FGameplayTag::RequestGameplayTag("Buff.Talent.SlowDownResistance");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		PXCharacter->BuffComponent->AddBuffByTag(TalentTag);
	}

	// 最大生命值
	TalentTag = FGameplayTag::RequestGameplayTag("TalentSet.MaxHPPlus");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		if (PXCharacter->HealthComponent)
		{
			PXCharacter->HealthComponent->ModifyMaxHP(EffectGameplayTags[TalentTag], EStatChange::Increase, true);
		}
	}

	// 最大体力值
	TalentTag = FGameplayTag::RequestGameplayTag("TalentSet.MaxEPPlus");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		if (UEnergyComponent* EnergyComponent = PXCharacter->EnergyComponent)
		{
			EnergyComponent->SetMaxEP(EffectGameplayTags[TalentTag] + EnergyComponent->GetMaxEP());
		}
	}

	// 月光蛊
	TalentTag = FGameplayTag::RequestGameplayTag("TalentSet.Moonlight.BasicDamage");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		if (SkillSettings->MoonLightInsect)
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(FVector(0, 0, 50));
			SpawnSkill(SkillSettings->MoonLightInsect, SpawnTransform);
		}
	}

	// 魔法护盾
	// FGameplayTag::RequestGameplayTag("Talent.MagicShield");
	if (BasicBuildSaveGame->ChosenTalents.Contains(FGameplayTag::RequestGameplayTag("Talent.MagicShield")))
	{
		if (SkillSettings->MagicShield)
		{
			SpawnSkill(SkillSettings->MagicShield);
		}
	}

	// 雷公助我
	if (BasicBuildSaveGame->ChosenTalents.Contains(FGameplayTag::RequestGameplayTag("Talent.Thor")))
	{
		if (SkillSettings->Thor)
		{
			if (SpawnSkill(SkillSettings->Thor))
			{
				PXCharacter->BuffComponent->AddBuffByTag(FGameplayTag::RequestGameplayTag("Buff.Talent.Thor"));
			}
		}
	}

	// 光合作用
	if (BasicBuildSaveGame->ChosenTalents.Contains(FGameplayTag::RequestGameplayTag("Talent.SunHeal")))
	{
		if (SkillSettings->SunHeal)
		{
			if (SpawnSkill(SkillSettings->SunHeal))
			{
				PXCharacter->BuffComponent->AddBuffByTag(FGameplayTag::RequestGameplayTag("Buff.Talent.SunHeal"));
			}
		}
	}

	// 奇迹行者
	MakeMiracleWalker();

	// 体型 / 蚁人
	TalentTag = FGameplayTag::RequestGameplayTag("TalentSet.BodySizePlusPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		PXCharacter->SetScale(1 + EffectGameplayTags[TalentTag]);
		PXCharacter->BuffComponent->AddBuffByTag(FGameplayTag::RequestGameplayTag("Buff.Talent.AntMan"));
	}

	// 武术
	TalentTag = FGameplayTag::RequestGameplayTag("TalentSet.Wushu.AttackDamagePlusOnCurHPPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		PXCharacter->BuffComponent->AddBuffByTag(FGameplayTag::RequestGameplayTag("Buff.Talent.WuShu"));
	}

	// 热身
	TalentTag = FGameplayTag::RequestGameplayTag("TalentSet.Warmup.AttackDamagePlusPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		GameInstance->GetTimerManager().SetTimer(WarmUP_TimerHandle, this,
			&UTalentComponent::MoveWarmingUP, 0.2, true);
	}

	// 摇摆拳
	TalentTag = FGameplayTag::RequestGameplayTag("Buff.Talent.SwingFist");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		MakeSwingFistPower();
	}

	// 淘金者
	TalentTag = FGameplayTag::RequestGameplayTag("Buff.Talent.GoldRush");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		if (SkillSettings->GoldRush && SpawnSkill(SkillSettings->GoldRush))
		{
			PXCharacter->BuffComponent->AddBuffByTag(TalentTag);
		}
	}

	// 赏金猎人
	TalentTag = FGameplayTag::RequestGameplayTag("TalentSet.GoldPickup.SpeedupPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		PXCharacter->BuffComponent->AddBuffByTag(FGameplayTag::RequestGameplayTag("Buff.Talent.GoldHunter"));
	}

	// 静功
	TalentTag = FGameplayTag::RequestGameplayTag("TalentSet.StaticPower.AttackDamagePlusPercent");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		if (SkillSettings->StaticPower)
		{
			SpawnSkill(SkillSettings->StaticPower);
		}
	}

	// 不灭
	MakeImmortalPower(true);

	// 飞沙走石
	TalentTag = FGameplayTag::RequestGameplayTag("TalentSet.NoSandSpawnInterval");
	if (EffectGameplayTags.Contains(TalentTag))
	{
		if (SkillSettings->NoSand && SpawnSkill(SkillSettings->NoSand))
		{
			PXCharacter->BuffComponent->AddBuffByTag(TalentTag);
		}
	}
	
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
		IBuff_Interface::Execute_RemoveBuff(PXCharacter, FGameplayTag::RequestGameplayTag("Buff.Talent.Warmup"), true);
		WarmUP_Power = 0 ;
		WarmUP_MoveDistance = 0;
		OwnerPreLocation = PXCharacter->GetActorLocation();
	}

	MakeMiracleWalker();
	MakeImmortalPower(false);

	IBuff_Interface::Execute_RemoveBuff(PXCharacter, FGameplayTag::RequestGameplayTag("Buff.Talent.StaticPower"), true);
	IBuff_Interface::Execute_RemoveBuff(PXCharacter, FGameplayTag::RequestGameplayTag("Buff.Talent.DodgeStrike"), true);
	
}

void UTalentComponent::OnDashEnd()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	if (!PXCharacter->Implements<UBuff_Interface>()) return;

	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	// 闪避突袭
	FGameplayTag DamagePlus_AfterDashTag = FGameplayTag::RequestGameplayTag("TalentSet.DamagePlus_AfterDash");
	if (EffectGameplayTags.Contains(DamagePlus_AfterDashTag))
	{
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Buff.Talent.DodgeStrike");
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
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag("TalentSet.Wushu.AttackDamagePlusOnCurHPPercent");
	if (EffectGameplayTags.Contains(Tag))
	{
		LocalPlus += FMath::RoundToInt(EffectGameplayTags[Tag] * PXCharacter->HealthComponent->GetMaxHP()) ;
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

	FGameplayTag MoveDistancePerLevelTag = FGameplayTag::RequestGameplayTag("TalentSet.Warmup.MoveDistancePerLevel");
	FGameplayTag AttackDamagePlusPercentTag = FGameplayTag::RequestGameplayTag("TalentSet.Warmup.AttackDamagePlusPercent");

	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	
	if (!EffectGameplayTags.Contains(MoveDistancePerLevelTag) ||
		!EffectGameplayTags.Contains(AttackDamagePlusPercentTag)
	) return;

	if (WarmUP_MoveDistance < EffectGameplayTags[MoveDistancePerLevelTag]) return;

	WarmUP_MoveDistance -= EffectGameplayTags[MoveDistancePerLevelTag];
	WarmUP_Power ++;

	if (!PXCharacter->Implements<UBuff_Interface>()) return;
	
	float PlusPowerPercent = WarmUP_Power * EffectGameplayTags[AttackDamagePlusPercentTag];
	FGameplayTag WarmUP_Tag = FGameplayTag::RequestGameplayTag("Buff.Talent.Warmup");
	
	FString BuffName = ULocalizationFuncLib::GetBuffText("Buff_Warmup") + FString::Printf(TEXT("%d"), WarmUP_Power);
	IBuff_Interface::Execute_BuffEffect_Attack(PXCharacter, WarmUP_Tag, PlusPowerPercent, 0, 999);
	IBuff_Interface::Execute_AddBuff(PXCharacter, WarmUP_Tag, BuffName,
		FLinearColor( 0.25 * WarmUP_Power, 0.1, 0.1, 1.0), false);
}

void UTalentComponent::MakeSwingFistPower()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	
	FGameplayTag DecreaseTag = FGameplayTag::RequestGameplayTag("TalentSet.SwingPunch.AttackDamage_DecreasePercent");
	FGameplayTag IncreaseTag = FGameplayTag::RequestGameplayTag("TalentSet.SwingPunch.AttackDamage_IncreasePercent");

	if (!EffectGameplayTags.Contains(DecreaseTag) || !EffectGameplayTags.Contains(IncreaseTag)) return;



	if (!PXCharacter->Implements<UBuff_Interface>()) return;

	SwingFistPower = !SwingFistPower;
	
	FGameplayTag SwingFistTag = FGameplayTag::RequestGameplayTag("Buff.Talent.SwingFist");
	IBuff_Interface::Execute_RemoveBuff(PXCharacter, SwingFistTag, true);
	
	FString BuffName = ULocalizationFuncLib::GetBuffText("Buff_SwingFist");
	if (SwingFistPower)
	{
		IBuff_Interface::Execute_BuffEffect_Attack(PXCharacter, SwingFistTag, EffectGameplayTags[IncreaseTag], 0, 999);
		IBuff_Interface::Execute_AddBuff(PXCharacter, SwingFistTag,  BuffName + "↓",
			FLinearColor(0.093059, 0.027321, 0.0, 1), false);
	}
	else
	{
		IBuff_Interface::Execute_BuffEffect_Attack(PXCharacter, SwingFistTag, EffectGameplayTags[DecreaseTag], 0, 999);
		IBuff_Interface::Execute_AddBuff(PXCharacter, SwingFistTag,  BuffName + "↑",
			FLinearColor(1.0, 0.296138, 0.0, 1), false);
	}
}

void UTalentComponent::MakeMiracleWalker()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	if (!PXCharacter->Implements<UBuff_Interface>()) return;

	IBuff_Interface::Execute_RemoveBuff(PXCharacter, FGameplayTag::RequestGameplayTag("Buff.Talent.MiracleWalker"), true);

	FGameplayTag DamagePlusTag = FGameplayTag::RequestGameplayTag("TalentSet.MiracleWalker.DamagePlus");
	FGameplayTag IntervalTag = FGameplayTag::RequestGameplayTag("TalentSet.MiracleWalker.Interval");


	if (!EffectGameplayTags.Contains(DamagePlusTag) || !EffectGameplayTags.Contains(IntervalTag)) return;

	if (MiracleWalker_TimerHandle.IsValid())
	{
		PXCharacter->GetGameInstance()->GetTimerManager().ClearTimer(MiracleWalker_TimerHandle);
	}
	
	PXCharacter->GetGameInstance()->GetTimerManager().SetTimer(MiracleWalker_TimerHandle, [this, DamagePlusTag]
	{
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
		FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
		if (EffectGameplayTags.Contains(DamagePlusTag))
		{
			FGameplayTag MiracleWalkerTag = FGameplayTag::RequestGameplayTag("Buff.Talent.MiracleWalker");
			IBuff_Interface::Execute_BuffEffect_Attack(PXCharacter, MiracleWalkerTag, 0.0,
				EffectGameplayTags[DamagePlusTag], 999
			);
			if (PXCharacter->BuffComponent)
			{
				PXCharacter->BuffComponent->AddBuffByTag(MiracleWalkerTag);
			}
		}
	}, EffectGameplayTags[IntervalTag], false);
}

void UTalentComponent::MakeImmortalPower(bool First)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->BuffComponent)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter->HealthComponent)
	if (!PXCharacter->Implements<UBuff_Interface>()) return;
	
	FGameplayTag AttackDamagePlusOnMaxHPPercentTag = FGameplayTag::RequestGameplayTag("TalentSet.Immortal.AttackDamagePlusOnMaxHPPercent");
	FGameplayTag IntervalTag = FGameplayTag::RequestGameplayTag("TalentSet.Immortal.Interval");
	FGameplayTag MaxHPPlusAfterAttackTag = FGameplayTag::RequestGameplayTag("TalentSet.Immortal.MaxHPPlusAfterAttack");
	
	FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
	
	if (!EffectGameplayTags.Contains(AttackDamagePlusOnMaxHPPercentTag) || 
		!EffectGameplayTags.Contains(IntervalTag) ||
		!EffectGameplayTags.Contains(MaxHPPlusAfterAttackTag)
	) return;

	FGameplayTag ImmortalPowerTag = FGameplayTag::RequestGameplayTag("Buff.Talent.ImmortalPower");
	if (!First)
	{
		if (PXCharacter->BuffComponent->BuffExist(ImmortalPowerTag))
		{
			IBuff_Interface::Execute_RemoveBuff(PXCharacter, ImmortalPowerTag, true);
			PXCharacter->HealthComponent->ModifyMaxHP(1, EStatChange::Increase, true);
		}
	}

	PXCharacter->GetGameInstance()->GetTimerManager().SetTimer(
		ImmortalPower_TimerHandle, [this, AttackDamagePlusOnMaxHPPercentTag, ImmortalPowerTag]
	{
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)
		FEffectGameplayTags& EffectGameplayTags = PXCharacter->EffectGameplayTags;
		if (!EffectGameplayTags.Contains(AttackDamagePlusOnMaxHPPercentTag)) return;
			
		IBuff_Interface::Execute_BuffEffect_Attack(PXCharacter, ImmortalPowerTag, 0.0f,
			FMath::RoundToInt(PXCharacter->HealthComponent->GetCurrentHP() + EffectGameplayTags[AttackDamagePlusOnMaxHPPercentTag]),
			0.0f
		);
		if (PXCharacter->BuffComponent)
		{
			PXCharacter->BuffComponent->AddBuffByTag(ImmortalPowerTag);
		}

		UGameInstance* GameInstance = PXCharacter->GetGameInstance();
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);
		UPXSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UPXSaveGameSubsystem>();
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem);
		UPXMainSaveGame* MainSaveGame = SaveGameSubsystem->GetMainData();
		MainSaveGame->CharacterInheritAttribute.MaxHP ++;
			
	}, EffectGameplayTags[IntervalTag], false);
	 
}




