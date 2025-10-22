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

	for (auto& Data: ActionMoveData->ActionMoveCurveVectors)
	{
		ActionMoveCurveVector.Add(Data.CurveFloatName, Data);
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

FCurveFloatData UEnemyAISubsystem::GetActionMoveCurveData(const FName& CurveName) const
{
	return  ActionMoveCurveVector.FindRef(CurveName);
}
