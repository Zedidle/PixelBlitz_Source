// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"


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
