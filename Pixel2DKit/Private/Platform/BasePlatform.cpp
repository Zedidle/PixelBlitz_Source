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

	SetMobility(bDefaultFloat ? EComponentMobility::Type::Movable : EComponentMobility::Type::Static);
}

// Called every frame
void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Tick_DefaultFloat();
}

void ABasePlatform::SetDefaultFloat(bool V)
{
	bDefaultFloat = V;
}

void ABasePlatform::SetMobility(EComponentMobility::Type V)
{
	GetRootComponent()->SetMobility(V);
	SetActorTickEnabled(V == EComponentMobility::Movable);		
}

void ABasePlatform::OnPlayerLand_Implementation()
{
}

