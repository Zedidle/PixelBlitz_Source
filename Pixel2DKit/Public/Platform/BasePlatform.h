// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "BasePlatform.generated.h"

UCLASS()
class PIXEL2DKIT_API ABasePlatform : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	bool bDefaultFloat = true;
	
	FVector TargetOffset = FVector::ZeroVector;
	FVector CurOffset = FVector::ZeroVector;
	FVector PreOffset = FVector::ZeroVector;
	float Period = 1.0f;
	
public:	
	// Sets default values for this actor's properties
	ABasePlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Platform")
	void SetDefaultFloat(bool V);

	UFUNCTION(BlueprintCallable, Category = "Platform")
	void SetMobility(EComponentMobility::Type V);
	
	UPROPERTY(BlueprintReadOnly)
	FVector StartLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform", meta=(ExposeOnSpawn))
	float FloatPeriod = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform")
	float DistanceRandomRatio = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform")
	FVector FloatDirection = FVector(0,0,1);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform", meta=(ExposeOnSpawn))
	float FloatDistance = 0.1;

	void Tick_DefaultFloat();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Platform")
	void OnPlayerLand();
};

inline void ABasePlatform::Tick_DefaultFloat()
{
	if (!bDefaultFloat) return;
	if (GetRootComponent()->Mobility != EComponentMobility::Type::Movable) return;
	
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	PreOffset = CurOffset;
	CurOffset = TargetOffset * FMath::Sin(World->TimeSeconds / Period );
	FHitResult* OutSweepHitResult = nullptr;
	AddActorWorldOffset(CurOffset - PreOffset, false, OutSweepHitResult, ETeleportType::None);
}
