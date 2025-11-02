// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/SpaceFuncLib.h"

#include "CollisionDebugDrawingPublic.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Enemy/BaseEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utilitys/DebugFuncLab.h"

extern TAutoConsoleVariable<int32> CDebugTraceDuration;


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

bool USpaceFuncLib::ActorAtActorRightWithOffset(AActor* A, AActor* B, const int PlayerIndex, FVector OffsetA,
	FVector OffsetB)
{
	if (!A || !B) return false;
	
	APlayerController* Controller = UGameplayStatics::GetPlayerController(A->GetWorld(), PlayerIndex);
	if (!Controller) return false;

	FVector2d v1, v2;
	UGameplayStatics::ProjectWorldToScreen(Controller, A->GetActorLocation() + OffsetA, v1, false);
	UGameplayStatics::ProjectWorldToScreen(Controller, B->GetActorLocation() + OffsetB, v2, false);

	return v1.X > v2.X;
}

FVector2D USpaceFuncLib::GetDistance2D_InScreen(AActor* A, AActor* B, const int PlayerIndex, FVector OffsetA, FVector OffsetB)
{
	if (!A || !B) return FVector2D::ZeroVector;
	
	APlayerController* Controller = UGameplayStatics::GetPlayerController(A->GetWorld(), PlayerIndex);
	if (!Controller) return FVector2D::ZeroVector;

	FVector2d v1, v2;
	UGameplayStatics::ProjectWorldToScreen(Controller, A->GetActorLocation() + OffsetA, v1, false);
	UGameplayStatics::ProjectWorldToScreen(Controller, B->GetActorLocation() + OffsetB, v2, false);

	return v1 - v2;
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
	FVector Dir_BtoA = (A->GetActorLocation() - B->GetActorLocation()).GetSafeNormal2D();
	
	float d_east = Dir_BtoA.Dot(BasicDir);
	float d_west = -d_east;

	float Cos45Deg = FMath::Cos(FMath::DegreesToRadians(45.0f));
	if (d_east >= Cos45Deg) return East;
	if (d_west >= Cos45Deg) return West;
	if (Dir_BtoA.Cross(BasicDir).Z < 0) return South;

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

	EDrawDebugTrace::Type DrawType = EDrawDebugTrace::None;

#if WITH_EDITOR
	if (CDebugTraceDuration.GetValueOnGameThread())
	{
		DrawType = EDrawDebugTrace::ForDuration;
	}
#endif

	// TraceTypeQuery1 是可视即可
	
	FHitResult OutHit;
	bool hit = UKismetSystemLibrary::LineTraceSingle(World, StartLocation, EndLocation,
		TraceTypeQuery1, false, {},
				EDrawDebugTrace::None, OutHit, true,
				FLinearColor::Red, FLinearColor::Green, 1.0f);

	return !hit;
}

bool USpaceFuncLib::CheckCliffProcess(const FVector& StartLocation, const FVector& EndLocation, float CliffHeight, float CheckRate, float MinDirSwitchDistance)
{
	float LerpPercent = 1.0f;
	float CurDistance = (EndLocation - StartLocation).Size2D();
	FVector DistanceVector = EndLocation - StartLocation;
	
	while ( CurDistance > MinDirSwitchDistance )
	{
		if (CheckCliff(StartLocation + DistanceVector * LerpPercent, CliffHeight))
		{
			return true;
		}
		LerpPercent *= CheckRate;
		CurDistance *= CheckRate;
	}
	return false;
}

bool USpaceFuncLib::GetJumpPoints(TArray<FVector>& Points, const FVector& StartLocation, const FVector& TargetLocation, float MaxHorizontalDistance, float HigherDistance, float LowerDistance, float PerCheckDistance)
{
	if (PerCheckDistance <= 0) return false;
	
	const UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!IsValid(World)) return false;

	FVector CheckDirection = (TargetLocation - StartLocation).GetSafeNormal2D();
	float RemDistance = FMath::Min(MaxHorizontalDistance,  (TargetLocation - StartLocation).Size() - 10);
	
	while (RemDistance > PerCheckDistance)
	{
		FVector HorizontalVector = StartLocation + CheckDirection * RemDistance;
		FVector CheckStart = HorizontalVector + FVector(0,0,HigherDistance);
		FVector CheckEnd = HorizontalVector - FVector(0,0,LowerDistance);
		
		TArray<FHitResult> OutHits;
		bool bHit = UKismetSystemLibrary::LineTraceMulti(World, CheckStart, CheckEnd, TraceTypeQuery1, false, {},
			EDrawDebugTrace::None, OutHits, true, FLinearColor::Red, FLinearColor::Green, 2.0f
		);

		if (bHit)
		{
			for (auto& Hit : OutHits)
			{
				Points.Add(Hit.Location);
			}
		}

		RemDistance -= PerCheckDistance;
	}

	return Points.Num() > 0;
}

FVector USpaceFuncLib::GetHorizontalFarestPosition(const FVector& StartLocation, FVector& Direction, float RemDistance, float CliffHeight, float PerCheckDistance)
{
	if (PerCheckDistance <= 0) return StartLocation;
	
	const UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!IsValid(World)) return StartLocation;
	
	// 目标水平方向的墙体检测
	FHitResult OutHit;
	bool bWallBlock = UKismetSystemLibrary::LineTraceSingle(World, StartLocation,StartLocation + RemDistance * Direction,
	TraceTypeQuery1, false, {},
			EDrawDebugTrace::None, OutHit, true,
			FLinearColor::Red, FLinearColor::Green, 2.0f);

	if (bWallBlock)
	{
		if (OutHit.Distance < PerCheckDistance) return StartLocation;

		FVector NewEndLocation = OutHit.Location + OutHit.Normal * PerCheckDistance;

		Direction = (NewEndLocation - StartLocation).GetSafeNormal2D();
		RemDistance = (NewEndLocation - StartLocation).Size();
	}

	FVector CurFarestPosition = StartLocation;
	while (RemDistance > PerCheckDistance)
	{
		FVector CheckStart = StartLocation + Direction * RemDistance;
		FVector CheckEnd = CheckStart - FVector(0,0,CliffHeight);
		
		FHitResult HitResult;
		bool bHit = UKismetSystemLibrary::LineTraceSingle(World, CheckStart, CheckEnd, TraceTypeQuery1, false, {},
			EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green, 2.0f
		);

		if (bHit)
		{
			if (StartLocation == CurFarestPosition)
			{
				CurFarestPosition = HitResult.Location;
			}
		}
		else
		{
			CurFarestPosition = StartLocation;
		}
		
		RemDistance -= PerCheckDistance;
	}

	return CurFarestPosition;
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

bool USpaceFuncLib::IsActorInScreen(const UObject* WorldContextObject, AActor* Actor, float BufferPercentage, FVector Offset)
{
	if (!Actor) return false;
	return IsPointInScreen(WorldContextObject, Actor->GetActorLocation() + Offset, BufferPercentage);
}

bool USpaceFuncLib::IsPointInScreen(const UObject* WorldContextObject, const FVector& Point, float BufferPercentage)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	if (!GameInstance) return false;

	UWorld* World = GameInstance->GetWorld();
	if (!World) return false;

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC) return false;

	int SizeX, SizeY;
	PC->GetViewportSize(SizeX, SizeY);

	BufferPercentage = FMath::Clamp(BufferPercentage, -0.1f, 0.2f);
	int LeftLedge = -BufferPercentage * SizeX;
	int UpLedge = -BufferPercentage * SizeY;
	int RightLedge = (BufferPercentage + 1) * SizeX;
	int DownLedge = (BufferPercentage + 1) * SizeY;
	
	FVector2D ScreenPosition;
	UGameplayStatics::ProjectWorldToScreen(PC, Point, ScreenPosition, false);
	
	return ScreenPosition.X > LeftLedge && ScreenPosition.Y > UpLedge &&
			ScreenPosition.X < RightLedge && ScreenPosition.Y < DownLedge;
}

FVector2D USpaceFuncLib::GetActorPositionInScreen(const UObject* WorldContextObject, AActor* Actor, FVector Offset)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	if (!GameInstance) return FVector2D::ZeroVector;

	UWorld* World = GameInstance->GetWorld();
	if (!World) return FVector2D::ZeroVector;

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC) return FVector2D::ZeroVector;

	FVector2D ScreenPosition;
	UGameplayStatics::ProjectWorldToScreen(PC, Actor->GetActorLocation() + Offset, ScreenPosition, false);
	
	return ScreenPosition;
}

float USpaceFuncLib::CalAngle(FVector A, FVector B)
{
	float Angle = UKismetMathLibrary::DegAcos(FVector::DotProduct(A.GetSafeNormal(), B.GetSafeNormal()));
	return Angle;
}

float USpaceFuncLib::CalAngle2D(FVector2D A, FVector2D B)
{
	float Angle = UKismetMathLibrary::DegAcos(FVector2D::DotProduct(A.GetSafeNormal(), B.GetSafeNormal()));
	return Angle;
}

ABaseEnemy* USpaceFuncLib::FindEnemyInRangeClosest(const UObject* WorldContextObject, AActor* A, const TArray<AActor*>& ActorsToIgnore,  FVector2D InRange)
{
	return FindActorInRangeClosest<ABaseEnemy>(WorldContextObject, A, ActorsToIgnore, InRange);
}

ABaseEnemy* USpaceFuncLib::FindEnemyInRangeFarthest(const UObject* WorldContextObject, AActor* A, const TArray<AActor*>& ActorsToIgnore, FVector2D InRange)
{
	return FindActorInRangeFarthest<ABaseEnemy>(WorldContextObject, A,ActorsToIgnore,  InRange);
}

ABaseEnemy* USpaceFuncLib::FindEnemyInRangeRandomOne(const UObject* WorldContextObject, AActor* A, const TArray<AActor*>& ActorsToIgnore, FVector2D InRange)
{
	return FindActorInRangeRandomOne<ABaseEnemy>(WorldContextObject, A,ActorsToIgnore,  InRange);
}
