// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BaseInteractableItem.h"

#include "Pixel2DKit.h"
#include "Character/BasePXCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/TimerManagerFuncLib.h"
#include "Utilitys/SoundFuncLib.h"


// Sets default values
ABaseInteractableItem::ABaseInteractableItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Guid = FGuid::NewGuid();
	SetActorEnableCollision(false);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
}

// Called when the game starts or when spawned
void ABaseInteractableItem::BeginPlay()
{
	Super::BeginPlay();

	UTimerManagerFuncLib::SetDelay(GetWorld(),[WeakThis=TWeakObjectPtr(this)]
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

	float HighUP = 0;
	if (IsOnGround(HighUP))
	{
		PreOnGround = true;
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
		if (PreOnGround)
		{
			// 地面突然消失，会获得向上的弹力，同时避免因为卡帧掉落平台
			CurSimVelocity.Z = 60;
			PreOnGround = false;
		}
		else
		{
			CurSimVelocity.Z += DeltaTime * SimGravity;
		}
	}
	
	AddActorWorldOffset(CurSimVelocity * DeltaTime);
}

void ABaseInteractableItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	ABasePXCharacter* PxCharacter = Cast<ABasePXCharacter>(OtherActor);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PxCharacter)

	PxCharacter->AddInteractableItem(this);
	OnApproachEffect(PxCharacter);
}

void ABaseInteractableItem::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	ABasePXCharacter* PxCharacter = Cast<ABasePXCharacter>(OtherActor);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PxCharacter)

	PxCharacter->RemoveInteractableItem(this);
	OnLeaveEffect(PxCharacter);
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

void ABaseInteractableItem::OnInteractEffect(AActor* OtherActor)
{
	if (bInteracted) return;
	bInteracted = true;
	
	if (Sound_OnInteract)
	{
		USoundFuncLib::PlaySoundAtLocation(Sound_OnInteract, GetActorLocation());
	}

	BP_OnInteractEffect(OtherActor);
}

void ABaseInteractableItem::OnApproachEffect_Implementation(AActor* OtherActor)
{
	if (bInteracted) return;
	if (Sound_OnApproach)
	{
		USoundFuncLib::PlaySoundAtLocation(Sound_OnApproach, GetActorLocation());
	}
}

void ABaseInteractableItem::OnLeaveEffect_Implementation(AActor* OtherActor)
{
	
}


bool ABaseInteractableItem::IsOnGround_Implementation(float& HighUP)
{
	UWorld* World = GetWorld();
	if (!World) return false;

	FVector StartLocation = GetActorLocation();
	float StartZ = StartLocation.Z;
	FVector EndLocation = StartLocation + FVector(0, 0, -GroundHeightCheck);

	StartLocation.Z += GroundHeightCheck;

	FHitResult Hit;
	TArray<AActor*> ActorsToIgnore;
	bool bHit = UKismetSystemLibrary::LineTraceSingle(World, StartLocation, EndLocation,
		TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, Hit,true);

	if (!bHit) return false;
	
	// 只支持再地面上上浮
	float TargetZ = Hit.Location.Z + GroundHeightFloat;
	if (TargetZ > StartZ)
	{
		HighUP = TargetZ - StartZ;
		return true;
	}

	return false;
}



