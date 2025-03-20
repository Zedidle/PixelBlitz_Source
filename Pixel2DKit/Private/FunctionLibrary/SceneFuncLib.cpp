// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary/SceneFuncLib.h"

void USceneFuncLib::SetActorWorldRotationByQuat(AActor* Actor, const FQuat& NewRotation)
{
	Actor->SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);
	
}

void USceneFuncLib::AddActorWorldRotationByQuat(AActor* Actor, const FQuat& DeltaRotation)
{
	Actor->AddActorWorldRotation(DeltaRotation);
}

void USceneFuncLib::SetSceneComponentRotationByQuat(USceneComponent* Component, const FQuat& NewRotation)
{
	Component->SetWorldRotation(NewRotation);
}

