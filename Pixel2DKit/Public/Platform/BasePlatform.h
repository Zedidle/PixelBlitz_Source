// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
// #include "Engine/EngineTypes.h"
#include "BasePlatform.generated.h"

UCLASS()
class PIXEL2DKIT_API ABasePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	FVector StartLocation;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TEnumAsByte<EComponentMobility::Type> EMovable = EComponentMobility::Static;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float FloatSpeedPeroid = 1.0;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float DistanceRandomRatio = 0.1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FVector FloatDirection = FVector(0,0,1);
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float FloatDistance = 0.1;


	
	
};
