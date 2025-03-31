// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary/SpaceFuncLib.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "GameFramework/Character.h"

bool USpaceFuncLib::ActorAtActorRight(AActor* A, AActor* B, const int PlayerIndex)
{
	if (!A || !B) return false;
	
	APlayerController* Controller = UGameplayStatics::GetPlayerController(A->GetWorld(), PlayerIndex);
	if (!Controller) return false;

	FVector2d v1, v2;
	UGameplayStatics::ProjectWorldToScreen(Controller, A->GetActorLocation(), v1, false);
	UGameplayStatics::ProjectWorldToScreen(Controller, B->GetActorLocation(), v2, false);

	return v1.X > v2.X;
}

bool USpaceFuncLib::ActorAtActorFront(AActor* A, AActor* B, const int PlayerIndex)
{
	if (!A || !B) return false;

	FVector V = (A->GetActorLocation() - B->GetActorLocation()).GetSafeNormal2D();
	if (V.Dot(GetDirection2DFromPlayerViewPoint(PlayerIndex)) > 0)
	{
		return false;
	}

	return true;
}

bool USpaceFuncLib::ActorAtActorUP(AActor* A, AActor* B)
{
	if (!A || !B) return false;
	return A->GetActorLocation().Z > B->GetActorLocation().Z;
}

EWorldDirection USpaceFuncLib::ActorAtActorWorldDirection(AActor* A, AActor* B, const float OffsetRotation)
{
	if (!A || !B) return East;

	FVector BasicDir = FRotator(0, OffsetRotation, 0).RotateVector(FVector(1,0,0));
	FVector dir = (A->GetActorLocation() - B->GetActorLocation()).GetSafeNormal2D();
	
	float d_east = dir.Dot(BasicDir);
	float d_west = dir.Dot(-BasicDir);

	if (d_east >= FMath::Cos(45.0f)) return East;
	if (d_west >= FMath::Cos(45.0f)) return West;
	if (dir.Cross(BasicDir).Z < 0) return South;

	return North;
}


float USpaceFuncLib::GetDistanceX(AActor* A, AActor* B, const int PlayerIndex)
{
	if (!A || !B) return 0;

	FVector V = A->GetActorLocation() - B->GetActorLocation();

	// 投影到包含视角因素的X方向的距离；
	return V.ProjectOnTo(FRotator(0, 90, 0)
			.RotateVector(GetDirection2DFromPlayerViewPoint(PlayerIndex))).Size();
}

float USpaceFuncLib::GetDistanceY(AActor* A, AActor* B, const int PlayerIndex)
{
	if (!A || !B) return 0;

	// 投影到包含视角因素的Y方向的距离；
	FVector V = A->GetActorLocation() - B->GetActorLocation();
	return V.ProjectOnTo(GetDirection2DFromPlayerViewPoint(PlayerIndex)).Size();
}

bool USpaceFuncLib::CheckCliff(const FVector& StartLocation, const float CliffHeight)
{
	const UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!IsValid(World)) return false;

	const FVector EndLocation = StartLocation - FVector(0, 0, CliffHeight);

	FHitResult OutHit;
	TArray<AActor*> ActorsToIgnore;
	bool hit = UKismetSystemLibrary::LineTraceSingle(World, StartLocation, EndLocation,
		ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
				EDrawDebugTrace::None, OutHit, true,
				FLinearColor::Red, FLinearColor::Green, 1.0f);

	return !hit;
}

bool USpaceFuncLib::CheckCliffProcess(const FVector& StartLocation, const FVector& EndLocation, const float CliffHeight)
{
	float LerpPercent = 1.0f;
	while ( LerpPercent > 0.05 )
	{
		if (CheckCliff(StartLocation + (EndLocation - StartLocation) * LerpPercent, CliffHeight))
		{
			return true;
		}
		LerpPercent *= 0.8;
	}
	return false;
}


FVector USpaceFuncLib::GetDirection2DFromPlayerViewPoint(const int PlayerIndex)
{
	const UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!World) return FVector(0, 0, 0);

	FVector PlayerCharacterLocation = UGameplayStatics::GetPlayerCharacter(World, PlayerIndex)->GetActorLocation();

	FVector outLocation;
	FRotator out_Rotation;
	UGameplayStatics::GetPlayerController(World, PlayerIndex)->GetPlayerViewPoint(outLocation, out_Rotation);

	
	return (PlayerCharacterLocation - outLocation).GetSafeNormal2D(0.1);
}
