// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseSummoned.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utilitys/SpaceFuncLib.h"


// Sets default values
ABaseSummoned::ABaseSummoned()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseSummoned::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseSummoned::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Tick_KeepFaceTarget();
}

void ABaseSummoned::Tick_KeepFaceTarget()
{
	if (Target.IsValid())
	{
		if (bNeedKeepFaceTarget)
		{
			FaceTargetEffect();
		}
	}
}

AActor* ABaseSummoned::GetTarget()
{
	return Target.Get();
}

void ABaseSummoned::SetTarget(AActor* _Target)
{
	Target = _Target;
}

int ABaseSummoned::GetDamage()
{
	if (DamagesAtLevel.IsValidIndex(SummonedLevel - 1))
	{
		return DamagesAtLevel[SummonedLevel - 1];
	}
	return 1;
}

void ABaseSummoned::FaceTargetEffect_Implementation()
{
	bool bTargetAtRight = USpaceFuncLib::ActorAtActorRight(Target.Get(), this);
	float RotationZ = UKismetMathLibrary::SelectFloat(180, 0, bTargetAtRight);
	SetActorRotation(FRotator(0, RotationZ, 0));
}

void ABaseSummoned::SummonedEnd_Implementation()
{
	
}

