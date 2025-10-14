// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Interfaces/Fight_Interface.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/NoExportTypes.h"
#include "SpaceFuncLib.generated.h"

UENUM(BlueprintType)
enum EWorldDirection
{
	East,
	North,
	West,
	South
};



/**
 * 
 */
UCLASS(Abstract, MinimalAPI)
class USpaceFuncLib : public UObject
{
	GENERATED_BODY()
	
public:

	// 判断某个镜头中，A是否在B的右侧
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static bool ActorAtActorRight(AActor* A, AActor* B, const int PlayerIndex = 0);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static bool ActorAtActorRightWithOffset(AActor* A, AActor* B, const int PlayerIndex = 0, FVector OffsetA = FVector(0), FVector OffsetB = FVector(0));

	// 获得某镜头中，A相对B的距离
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static FVector2D GetDistance2D_InScreen(AActor* A, AActor* B, const int PlayerIndex = 0, FVector OffsetA = FVector(0), FVector OffsetB = FVector(0));
	
	// 判断某个镜头中，A是否在B的前侧（离镜头更近）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static bool ActorAtActorFront(AActor* A, AActor* B, const int PlayerIndex = 0);

	// 判断A、B的上下关系
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static bool ActorAtActorUP(AActor* A, AActor* B);

	// 计算A相对于B的世界方位
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static EWorldDirection ActorAtActorWorldDirection(AActor* A, AActor* B, const float OffsetRotation = 0);

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static float GetDistanceX(AActor* A, AActor* B, const int PlayerIndex = 0);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static float GetDistanceY(AActor* A, AActor* B, const int PlayerIndex = 0);

	// 判断目标下方是否有悬崖
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static bool CheckCliff(const FVector& StartLocation, const float CliffHeight = 50);

	// 判断是否过程是否有悬崖
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static bool CheckCliffProcess(const FVector& StartLocation, const FVector& EndLocation, float CliffHeight = 50, float CheckRate = 0.8, float MinDirSwitchDistance = 5);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static FVector GetDirection2DFromPlayerViewPoint(const int PlayerIndex = 0);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space", meta = (WorldContext = "WorldContextObject"))
	static bool IsActorInScreen(const UObject* WorldContextObject, AActor* Actor, FVector Offset = FVector(0), float BufferPercentage = 0.05);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space", meta = (WorldContext = "WorldContextObject"))
	static bool IsPointInScreen(const UObject* WorldContextObject, const FVector& Point, float BufferPercentage = 0.05);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space", meta = (WorldContext = "WorldContextObject"))
	static FVector2D GetActorPositionInScreen(const UObject* WorldContextObject, AActor* Actor, FVector Offset = FVector(0));

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static float CalAngle(FVector A, FVector B);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static float CalAngle2D(FVector2D A, FVector2D B);


	template<typename T>
	static T* FindActorInRangeClosest(const UObject* WorldContextObject, AActor* A, FVector2D InRange = {0, 1000}, bool bAlive = true);
	template<typename T>
	static T* FindActorInRangeFarthest(const UObject* WorldContextObject, AActor* A, FVector2D InRange = {0, 1000}, bool bAlive = true);
	template<typename T>
	static T* FindActorInRangeRandomOne(const UObject* WorldContextObject, AActor* A, FVector2D InRange = {0, 1000}, bool bAlive = true);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space", meta = (WorldContext = "WorldContextObject"))
	static ABaseEnemy* FindEnemyInRangeClosest(const UObject* WorldContextObject, AActor* A, FVector2D InRange = FVector2D(0, 1000));
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space", meta = (WorldContext = "WorldContextObject"))
	static ABaseEnemy* FindEnemyInRangeFarthest(const UObject* WorldContextObject, AActor* A, FVector2D InRange = FVector2D(0, 1000));

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space", meta = (WorldContext = "WorldContextObject"))
	static ABaseEnemy* FindEnemyInRangeRandomOne(const UObject* WorldContextObject, AActor* A, FVector2D InRange = FVector2D(0, 1000));
};

template <typename T>
T* USpaceFuncLib::FindActorInRangeClosest(const UObject* WorldContextObject, AActor* A, FVector2D InRange, bool bAlive)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(A, nullptr)
	UWorld* World = WorldContextObject->GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(World, nullptr)

	T* Result = nullptr;
	float TmpCurDistance = MAX_FLOAT;
	
	for (TActorIterator<T> It(World); It; ++It)
	{
		T* B = *It;
		if (!IsValid(B)) continue;
		
		if (bAlive)
		{
			if (!B->template Implements<UFight_Interface>() || !IFight_Interface::Execute_IsAlive(B))
			{
				continue;
			}
		}
		
		float Distance = A->GetDistanceTo(B);
		if (Distance < InRange.X || Distance > InRange.Y) continue;
		
		if (Distance < TmpCurDistance)
		{
			TmpCurDistance = Distance;
			Result = B;
		}
	}

	return Result;
}

template <typename T>
T* USpaceFuncLib::FindActorInRangeFarthest(const UObject* WorldContextObject, AActor* A, FVector2D InRange, bool bAlive)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(A, nullptr)
	UWorld* World = WorldContextObject->GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(World, nullptr)
	
	T* Result = nullptr;
	float TmpCurDistance = 0;
	for (TActorIterator<T> It(World); It; ++It)
	{
		T* B = *It;
		if (!IsValid(B)) continue;

		if (bAlive)
		{
			if (!B->template Implements<UFight_Interface>() || !IFight_Interface::Execute_IsAlive(B))
			{
				continue;
			}
		}

		float Distance = A->GetDistanceTo(B);
		if (Distance < InRange.X || Distance > InRange.Y) continue;
		
		if (Distance > TmpCurDistance)
		{
			TmpCurDistance = Distance;
			Result = B;
		}
	}

	return Result;
}

template <typename T>
T* USpaceFuncLib::FindActorInRangeRandomOne(const UObject* WorldContextObject, AActor* A, FVector2D InRange, bool bAlive)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(A, nullptr)
	UWorld* World = WorldContextObject->GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(World, nullptr)
	
	TArray<T*> TmpTargets;

	for (TActorIterator<T> It(World); It; ++It)
	{
		T* B = *It;
		if (!IsValid(B)) continue;
		
		if (bAlive)
		{
			if (!B->template Implements<UFight_Interface>() || !IFight_Interface::Execute_IsAlive(B))
			{
				continue;
			}
		}

		float Distance = A->GetDistanceTo(B);
		if (Distance < InRange.X || Distance > InRange.Y) continue;

		TmpTargets.Add(B);
	}

	if (TmpTargets.IsEmpty()) return nullptr;

	
	return TmpTargets[FMath::RandHelper(TmpTargets.Num())];
}
