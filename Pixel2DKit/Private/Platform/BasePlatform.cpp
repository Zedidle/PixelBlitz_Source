// Fill out your copyright notice in the Description page of Project Settings.


#include "Platform/BasePlatform.h"

#include "Pixel2DKit/Pixel2DKit.h"

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
	StartLocation = GetActorLocation();
	Tags.Add(FName("Obstacle"));

	// 初始随机
	FloatDistance = FloatDistance * (1.0f + FMath::RandRange(DistanceRandomRatio * -1.0f, DistanceRandomRatio));

	GetRootComponent()->SetMobility(EMovable);
}

// Called every frame
void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	if (EMovable == EComponentMobility::Movable)
	{
		FVector DeltaLocation = FloatDirection * FloatDistance * FMath::Sin(World->GetDeltaSeconds() * FloatSpeedPeriod);
		FHitResult* OutSweepHitResult = nullptr;
		AddActorWorldOffset(DeltaLocation, false, OutSweepHitResult, ETeleportType::None);
	}
}

