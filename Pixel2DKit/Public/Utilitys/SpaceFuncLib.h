// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	// 计算两个Actor之间世界相对方位
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
	static bool CheckCliffProcess(const FVector& StartLocation, const FVector& EndLocation, const float CliffHeight = 50);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static FVector GetDirection2DFromPlayerViewPoint(const int PlayerIndex = 0);


	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static bool IsActorInScreen(const UObject* WorldContextObject, AActor* Actor, FVector Offset = FVector(0));

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static FVector2D GetActorPositionInScreen(const UObject* WorldContextObject, AActor* Actor, FVector Offset = FVector(0));

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static float CalAngle(FVector A, FVector B);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Space")
	static float CalAngle2D(FVector2D A, FVector2D B);
};
	