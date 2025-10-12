// Fill out your copyright notice in the Description page of Project Settings.


#include "Platform/BasePlatform.h"

#include "Pixel2DKit/Pixel2DKit.h"
#include "Subsystems/TimerSubsystemFuncLib.h"

void ABasePlatform::RecordStartLocation()
{
	StartLocation = GetActorLocation();
	bHasSetStartLocation = true;
}

void ABasePlatform::RecordEndLocation()
{
	EndLocation = GetActorLocation();
	bHasSetEndLocation = true;
}

// Sets default values
ABasePlatform::ABasePlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add(FName("Obstacle"));

	if (bHasSetStartLocation && bHasSetEndLocation)
	{
		if (PlatformMoveType == EPlatformMoveType::Float)
		{
			TargetOffset = (EndLocation - StartLocation) / 2 * (1.0f + FMath::RandRange(FloatDistanceRandRatio * -1.0f, FloatDistanceRandRatio));;
			SetActorLocation(StartLocation + TargetOffset);
			SetMobility(EComponentMobility::Movable);
			Period =  MovePeriod / (2 * UE_PI);
		}
		else if (PlatformMoveType == EPlatformMoveType::LandToPass)
		{
			TargetOffset = EndLocation - StartLocation;
			SetActorLocation(StartLocation);
		}
	}
	else if (PlatformMoveType == EPlatformMoveType::Float)
	{
		SetMobility(EComponentMobility::Movable);
	}
	else
	{
		SetMobility(EComponentMobility::Static);
	}
}

// Called every frame
void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Tick_DefaultFloat();
	Tick_LandToPass(DeltaTime);
}

void ABasePlatform::SetMobility(EComponentMobility::Type V)
{
	GetRootComponent()->SetMobility(V);
	SetActorTickEnabled(V == EComponentMobility::Movable);		
}



void ABasePlatform::OnPlayerLand_Implementation()
{
	if (PlatformMoveType != EPlatformMoveType::LandToPass) return;
	if (!bHasSetStartLocation || !bHasSetEndLocation)  return;
	
	if (bPassed) return;
	bPassed = true;
	
	SetMobility(EComponentMobility::Movable);

	UTimerSubsystemFuncLib::SetDelay(this, [WeakThis = TWeakObjectPtr(this)]
	{
		if (!WeakThis.IsValid()) return;
		WeakThis->SetMobility(EComponentMobility::Static);
	}, MovePeriod);
}

void ABasePlatform::Tick_LandToPass(float DeltaTime)
{
	if (!bPassed) return;
	
	CurPassTime += DeltaTime;

	PreOffset = CurOffset;
	if (IsValid(PassFloatCurve))
	{
		CurOffset = TargetOffset * PassFloatCurve->GetFloatValue(CurPassTime / MovePeriod);
	}
	else
	{
		CurOffset = TargetOffset * CurPassTime / MovePeriod;
	}	

	FHitResult* Hit = nullptr;
	AddActorWorldOffset(CurOffset - PreOffset, false, Hit);
}
