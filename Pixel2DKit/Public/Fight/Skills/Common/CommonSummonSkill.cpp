// Fill out your copyright notice in the Description page of Project Settings.

#include "CommonSummonSkill.h"
#include "Fight/Summoned/BaseSummoned.h"
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

void ACommonSummonSkill::OnDispel(int localnum)
{
	if (ActivatedSummoneds.IsEmpty()) return;

	int LastIndex = ActivatedSummoneds.Num() - 1;
	for (int i = LastIndex; i >= LastIndex - localnum; i--)
	{
		if (ActivatedSummoneds.IsValidIndex(i))
		{
			if (IsValid(ActivatedSummoneds[i]))
			{
				ActivatedSummoneds[i]->SummonedEnd();
				ActivatedSummoneds.RemoveAt(i);
			}
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

	if (!SummonedData.SummonedClass) return;
	
	FVector Location = GetActorLocation() + FMath::VRand() * SummonedData.SpawnRadius;
	
	FTransform Transform;
	Transform.SetLocation(Location);

	if (ABaseSummoned* Summoned = World->SpawnActor<ABaseSummoned>(SummonedData.SummonedClass, Transform))
	{
		Summoned->SetTarget(Target.Get());
		ActivatedSummoneds.Add(Summoned);
		
		if (ActivatedSummoneds.Num() == GetSpawnNum())
		{
			if (GetWorld())
			{
				GetWorld()->GetTimerManager().ClearTimer(SummonTimer);
			}
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
		Summoned->SummonedEnd();
	}
}

float ACommonSummonSkill::GetSpawnInterval()
{
	if (SummonedData.SpawnIntervalAtLevel.IsValidIndex(SkillLevel - 1))
	{
		return SummonedData.SpawnIntervalAtLevel[SkillLevel - 1];
	}
	return 1.0f;
}

float ACommonSummonSkill::GetSustainTime()
{
	if (SummonedData.SustainTimesAtLevel.IsValidIndex(SkillLevel - 1))
	{
		return SummonedData.SustainTimesAtLevel[SkillLevel - 1];
	}
	return 2.0f;
}

int ACommonSummonSkill::GetSpawnNum()
{
	if (SummonedData.SpawnNumsAtLevel.IsValidIndex(SkillLevel - 1))
	{
		return SummonedData.SpawnNumsAtLevel[SkillLevel - 1];
	}
	return 1;
}

