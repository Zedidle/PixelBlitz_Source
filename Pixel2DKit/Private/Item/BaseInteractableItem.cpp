// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BaseInteractableItem.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "Utilitys/SoundFuncLib.h"


// Sets default values
ABaseInteractableItem::ABaseInteractableItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Guid = FGuid::NewGuid();
	SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void ABaseInteractableItem::BeginPlay()
{
	Super::BeginPlay();

	UTimerSubsystemFuncLib::SetDelay(GetWorld(),[WeakThis=TWeakObjectPtr(this)]
	{
		if (!WeakThis.IsValid()) return;
		WeakThis->SetActorEnableCollision(true);
	}, 1);
}

// Called every frame
void ABaseInteractableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bSimPhysical)
	{
		CurSimVelocity = { 0,0,0};
		SetActorTickEnabled(false);
		return;
	}

	float HighUP;
	if (IsOnGround(HighUP))
	{
		if (CurSimVelocity.Z < -5)
		{
			// 反弹
			CurSimVelocity.Z *= -Elastic;
			CurSimVelocity.X *= Elastic;
			CurSimVelocity.Y *= Elastic;
		}
		else
		{
			AddActorWorldOffset(FVector(0,0,HighUP));
			return;
		}
	}
	else
	{
		CurSimVelocity.Z += DeltaTime * SimGravity;
	}
	
	AddActorWorldOffset(CurSimVelocity * DeltaTime);
}


void ABaseInteractableItem::SetVelocityOnSpawn(float RandomRotateAngle, float Speed)
{
	if (!bSimPhysical) return;

	Speed = FMath::FRandRange(0.8, 1.2) * Speed;
	
	FVector NewVelocity = {0, 0, Speed};
	
	float Roll = RandomRotateAngle * FMath::FRandRange(-1.f, 1.f);
	float Pitch = RandomRotateAngle * FMath::FRandRange(-1.f, 1.f);

	CurSimVelocity = FRotator(Pitch, 0, Roll).RotateVector(NewVelocity);
}

void ABaseInteractableItem::SetSimPhysical(bool V)
{
	bSimPhysical = V;
	if (bSimPhysical)
	{
		SetActorTickEnabled(true);
	}
}

void ABaseInteractableItem::OnInteractEffect_Implementation(AActor* OtherActor)
{
	if (Sound_OnInteract)
	{
		USoundFuncLib::PlaySoundAtLocation(Sound_OnInteract, GetActorLocation());
	}
	
}

bool ABaseInteractableItem::IsOnGround_Implementation(float& HighUP)
{
	UWorld* World = GetWorld();
	if (!World) return false;

	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + FVector(0, 0, -GroundHeightCheck);

	FHitResult Hit;
	TArray<AActor*> ActorsToIgnore;
	bool bHit = UKismetSystemLibrary::LineTraceSingle(World, StartLocation, EndLocation,
		TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, Hit,true);

	HighUP = Hit.Location.Z - EndLocation.Z;
	
	return bHit;
}



