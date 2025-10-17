// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PIXEL2DKIT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()





public:


	AEnemyAIController(const FObjectInitializer& ObjectInitializer);


	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = EnemyAI)
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	UFUNCTION(BlueprintCallable)
	void UpdateSightRadius(float NewRadius);


	UFUNCTION(BlueprintCallable)
	void OnPerceptionUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);
	


	
	// 不需要Navmesh的直接移动
	UFUNCTION(BlueprintCallable, Category="AI")
	void SimpleMoveToLocation(const FVector& Dest);
	
	virtual FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr);

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;














	
};
