// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "BasePlatform.generated.h"

UENUM(BlueprintType)
enum class EPlatformMoveType : uint8
{
	None,
	LandToPass UMETA(DisplayName = "触碰传递"),
	Float UMETA(DisplayName = "浮动"),
};


UCLASS()
class PIXEL2DKIT_API ABasePlatform : public AActor
{
	GENERATED_BODY()

	FVector CurOffset = FVector::ZeroVector;
	FVector PreOffset = FVector::ZeroVector;
	float Period = 1.0f;
	bool bPassed = false;
	float CurPassTime = 0.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	EPlatformMoveType PlatformMoveType = EPlatformMoveType::Float;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	bool bHasSetStartLocation = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	bool bHasSetEndLocation = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	FVector StartLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	FVector EndLocation = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform", meta = (AllowPrivateAccess = "true"))
	FVector TargetOffset = FVector(0,0,10);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform", meta=(ExposeOnSpawn, AllowPrivateAccess = "true"))
	float MovePeriod = 4.0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform | Float", meta=(ExposeOnSpawn, AllowPrivateAccess = "true"))
	float FloatDistanceRandRatio = 0.1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform | LandToPass", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* PassFloatCurve;
	

	UFUNCTION(CallInEditor, Category = "Platform")
	void RecordStartLocation();
	UFUNCTION(CallInEditor, Category = "Platform")
	void RecordEndLocation();
	
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
	void SetMobility(EComponentMobility::Type V);

	
	void Tick_DefaultFloat();
	void Tick_LandToPass(float DeltaTime);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Platform | LandToPass")
	void OnPlayerLand();
};

inline void ABasePlatform::Tick_DefaultFloat()
{
	if (PlatformMoveType != EPlatformMoveType::Float) return;
	if (GetRootComponent()->Mobility != EComponentMobility::Type::Movable) return;
	
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	PreOffset = CurOffset;
	CurOffset = TargetOffset * FMath::Sin(World->TimeSeconds / Period );
	FHitResult* Hit = nullptr;
	AddActorWorldOffset(CurOffset - PreOffset, false, Hit);
}
