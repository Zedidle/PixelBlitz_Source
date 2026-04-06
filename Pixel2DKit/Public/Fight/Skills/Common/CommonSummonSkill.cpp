// Fill out your copyright notice in the Description page of Project Settings.

#include "CommonSummonSkill.h"
#include "Fight/Summoned/BaseSummoned.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/DataTableSubsystem.h"


// Sets default values
ACommonSummonSkill::ACommonSummonSkill()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACommonSummonSkill::LoadData(FName ID, int Level)
{
	SummonedID = ID;
	SkillLevel = Level;
	
	// 读取召唤物表
	if (UDataTableSubsystem* DTManager = UDataTableSubsystem::GetInstance(GetWorld()))
	{
		if (const FSummoned* Data = DTManager->GetSummonedDataByName(SummonedID))
		{
			SummonedID = Data->SummonedID;
			SummonedData = *Data;
		}
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(SummonTimer, this, &ThisClass::Summon, GetSpawnInterval(), true);
	}
	SetLifeSpan(GetSustainTime());
}

void ACommonSummonSkill::BeginPlay()
{
	Super::BeginPlay();

	ActivateTiming = EAbilityTiming::BeDamaged;
}

void ACommonSummonSkill::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	End();
}

// Called every frame
void ACommonSummonSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACommonSummonSkill::OnDispel(int DispelLevel, float BasicRate)
{
	if (ActivatedSummoneds.IsEmpty()) return;

	int LastIndex = ActivatedSummoneds.Num() - 1;

	if (ActivatedSummoneds.IsValidIndex(LastIndex))
	{
		// ====================== 数值参数（可自由调整平衡）======================
		const float MinChance = 0.1f;    // 最小概率（5%，保底）
		const float MaxChance = 1.0f;    // 最大概率（85%，封顶，避免必中）
		const float PerLevelBonus = 0.25f; // 每差1级，概率提升10%
	
		// 计算等级差：驱散等级 - 召唤技能等级
		int32 LevelDiff = DispelLevel - SkillLevel;
	
		// 核心驱散概率公式（线性增长+钳制，最合理、最平滑）
		float DispelChance = BasicRate + LevelDiff * PerLevelBonus;
	
		// 强制限制概率在 最小~最大 之间（防止概率异常）
		DispelChance = FMath::Clamp(DispelChance, MinChance, MaxChance);

		// 随机判断：随机数 < 概率 → 驱散成功
		float R = FMath::FRand();
		if (R < DispelChance)
		{
			ActivatedSummoneds[LastIndex]->SummonedEnd();
			ActivatedSummoneds.RemoveAt(LastIndex);
		}
	}
}

void ACommonSummonSkill::OnOwnerDie()
{
	End();
}

void ACommonSummonSkill::SetTarget(AActor* _Target)
{
	Target = _Target;
}

void ACommonSummonSkill::Summon()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (!Owner) return;
	if (!SummonedData.SummonedClass) return;
	
	FVector Location = Owner->GetActorLocation() + FMath::VRand() * SummonedData.SpawnRadius;
	
	FTransform Transform;
	Transform.SetLocation(Location);

	// 2. 延迟生成Actor（仅创建实例，未执行BeginPlay等生命周期）
	if (ABaseSummoned* Summoned = World->SpawnActorDeferred<ABaseSummoned>(
		SummonedData.SummonedClass,  // 要生成的Actor类
		Transform,                   // 生成变换（位置/旋转/缩放）
		GetOwner(),                  // 生成参数
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	))
	{
		// 3. 【延迟阶段】Actor已创建，但未FinishSpawning，在此完成所有初始化
		// 注：Spawn时已指定Owner，可省略SetOwner；若有自定义SetOwner逻辑，可保留原代码
		// Summoned->SetOwner(GetOwner()); 
		Summoned->SetTarget(Target.Get());
		Summoned->SetLifeSpan(GetSummonedLifeSpan());
		ActivatedSummoneds.Add(Summoned);

		// 4. 【必须调用】完成Actor生成，触发PostInitializeComponents、BeginPlay等生命周期
		UGameplayStatics::FinishSpawningActor(Summoned, Transform);

		// 5. 生成成功后，执行原定时器逻辑（完全保留原业务）
		if (ActivatedSummoneds.Num() == GetSpawnNum())
		{
			World->GetTimerManager().ClearTimer(SummonTimer);
		}
	}
	
}

void ACommonSummonSkill::End()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(SummonTimer);
	}
	
	for (ABaseSummoned* Summoned : ActivatedSummoneds)
	{
		if (IsValid(Summoned))
		{
			Summoned->SummonedEnd();
		}
	}
}

float ACommonSummonSkill::GetSustainTime()
{
	if (SummonedData.SustainTimeAtLevel.IsValidIndex(SkillLevel - 1))
	{
		return SummonedData.SustainTimeAtLevel[SkillLevel - 1];
	}
	return 5.0f;
}

float ACommonSummonSkill::GetSpawnInterval()
{
	if (SummonedData.SpawnIntervalAtLevel.IsValidIndex(SkillLevel - 1))
	{
		return SummonedData.SpawnIntervalAtLevel[SkillLevel - 1];
	}
	return 1.0f;
}

float ACommonSummonSkill::GetSummonedLifeSpan()
{
	if (SummonedData.SummonedLifeSpanAtLevel.IsValidIndex(SkillLevel - 1))
	{
		return SummonedData.SummonedLifeSpanAtLevel[SkillLevel - 1];
	}
	return 3.0f;
}

int ACommonSummonSkill::GetSpawnNum()
{
	if (SummonedData.SpawnNumsAtLevel.IsValidIndex(SkillLevel - 1))
	{
		return SummonedData.SpawnNumsAtLevel[SkillLevel - 1];
	}
	return 1;
}

