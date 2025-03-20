// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PIXEL2DKIT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()





public:

	// 不需要Navmesh的直接移动
	UFUNCTION(BlueprintCallable, Category="AI")
	void SimpleMoveToLocation(const FVector& Dest);
	
	virtual FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr);

	
};
