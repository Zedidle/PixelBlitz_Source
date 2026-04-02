// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSummonSkill.h"

#include "Fight/Summoned/BaseSummoned.h"


// Sets default values
ABaseSummonSkill::ABaseSummonSkill()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseSummonSkill::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(SummonTimer, this, &ThisClass::Summon, SpawnInterval, true);
	}
	
	// 后续可能SetData? 
	SetLifeSpan(GetSustainTime());
}

void ABaseSummonSkill::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	End();
}

// Called every frame
void ABaseSummonSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseSummonSkill::OnDispel(int localnum)
{
	if (Summoneds.IsEmpty()) return;

	int LastIndex = Summoneds.Num() - 1;
	for (int i = LastIndex; i >= LastIndex - localnum; i--)
	{
		if (Summoneds.IsValidIndex(i))
		{
			if (IsValid(Summoneds[i]))
			{
				Summoneds[i]->SummonedEnd();
				Summoneds.RemoveAt(i);
			}
		}
	}

	
}

void ABaseSummonSkill::OnOwnerDie()
{
	End();
}

void ABaseSummonSkill::SetTarget(AActor* _Target)
{
	Target = _Target;
}

void ABaseSummonSkill::Summon()
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	FVector Location = GetActorLocation() + FMath::VRand() * SpawnRadius;
	
	FTransform Transform;
	Transform.SetLocation(Location);
	
	if (ABaseSummoned* Summoned = World->SpawnActor<ABaseSummoned>(SummonedClass, Transform))
	{
		Summoned->SetTarget(Target.Get());
		Summoneds.Add(Summoned);
		
		if (Summoneds.Num() == GetSpawnNum())
		{
			if (GetWorld())
			{
				GetWorld()->GetTimerManager().ClearTimer(SummonTimer);
			}
		}
	}
	
}

void ABaseSummonSkill::End()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(SummonTimer);
	}
	for (ABaseSummoned* Summoned : Summoneds)
	{
		Summoned->SummonedEnd();
	}
}

float ABaseSummonSkill::GetSustainTime()
{
	if (SustainTimesAtLevel.IsValidIndex(SkillLevel - 1))
	{
		return SustainTimesAtLevel[SkillLevel - 1];
	}
	return 2.0f;
}

int ABaseSummonSkill::GetSpawnNum()
{
	if (SpawnNumsAtLevel.IsValidIndex(SkillLevel - 1))
	{
		return SpawnNumsAtLevel[SkillLevel - 1];
	}
	return 1;
}

