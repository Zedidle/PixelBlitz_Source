// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAIController.h"
#include "NavigationSystem.h"
#include "Enemy/BaseEnemy.h"
#include "Interfaces/Fight_Interface.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("SightConfig");

	if (AIPerception && SightConfig)
	{
		SightConfig->SightRadius = 1000.f;
		SightConfig->LoseSightRadius = 1500.f;
		SightConfig->PeripheralVisionAngleDegrees = 90.f;

		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		// SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		// SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

		AIPerception->ConfigureSense(*SightConfig);
		AIPerception->SetDominantSense(UAISenseConfig_Sight::StaticClass());

		AIPerception->OnTargetPerceptionInfoUpdated.AddDynamic(this, &ThisClass::OnPerceptionUpdated);
	}
}

void AEnemyAIController::OnPerceptionUpdated(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	ABaseEnemy* Enemy = Cast<ABaseEnemy>(GetPawn());
	if (!Enemy) return;
 
	AActor* Actor = UpdateInfo.Target.Get();
	const FAIStimulus& Stimulus = UpdateInfo.Stimulus;
 
	if (Stimulus.WasSuccessfullySensed())
	{
		if (IsValid(Actor) && !Actor->IsPendingKillPending())
		{
			Enemy->OnSensingPlayer(Actor);
		}
	}
	else
	{
		Enemy->DelayLosePlayer();
	}
}

void AEnemyAIController::UpdateSightRadius(float NewRadius)
{
	if (SightConfig && AIPerception)
	{
		// 修改配置对象的属性
		SightConfig->SightRadius = NewRadius;
		SightConfig->LoseSightRadius = NewRadius * 1.2 + 100.0f;
    
		// 重新配置感知组件中的该感官
		AIPerception->RequestStimuliListenerUpdate();
	}
}

void AEnemyAIController::SimpleMoveToLocation(const FVector& Dest)
{
	FAIMoveRequest MoveRequest(Dest);
	MoveRequest.SetNavigationFilter(nullptr)
		.SetUsePathfinding(false) // 不使用路径查询，直接移动（如果没有NavMesh事先构建导航，则无法移动）
		.SetAllowPartialPath(true) // 不管是否到达得了，允许走一部分路程；false时，会导致切换目标位置时卡顿
		.SetRequireNavigableEndLocation(false)
		.SetApplyCostLimitFromHeuristic(false)
		.SetProjectGoalLocation(false)
		.SetCanStrafe(true)
		.SetReachTestIncludesAgentRadius(false)
		.SetReachTestIncludesGoalRadius(false)
		.SetAcceptanceRadius(-1)
		.SetUserData(nullptr)
		.SetUserFlags(0);

	MoveTo(MoveRequest);
}

FPathFollowingRequestResult AEnemyAIController::MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath)
{
	return Super::MoveTo(MoveRequest, OutPath);
}

ETeamAttitude::Type AEnemyAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (Other.Implements<UFight_Interface>())
	{
		return IFight_Interface::Execute_GetOwnCamp(&Other).HasTag(TAG("Enemy")) ? ETeamAttitude::Friendly : ETeamAttitude::Hostile;
	}

	return ETeamAttitude::Neutral;
	
}
