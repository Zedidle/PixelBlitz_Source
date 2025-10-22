// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/EnemyAISubsystem.h"
#include "Pixel2DKit.h"
#include "Settings/Config/EnemyActionMoveDataAsset.h"
#include "Settings/Config/PXCustomSettings.h"


void UEnemyAISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	UEnemyActionMoveDataAsset* ActionMoveData = Settings->EnemyActionMoveDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ActionMoveData);

	for (auto& [CurveFloatName, Curve]: ActionMoveData->ActionMoveCurveVectors)
	{
		ActionMoveCurveVector.Add(CurveFloatName, Curve.LoadSynchronous());
	}
	
}

void UEnemyAISubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UEnemyAISubsystem::BeginDestroy()
{
	Super::BeginDestroy();
}

UCurveVector* UEnemyAISubsystem::GetActionMoveCurve(FName CurveName) const
{
	return ActionMoveCurveVector.FindRef(CurveName);
}
