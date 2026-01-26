// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/TimerManagerFuncLib.h"

#include "Kismet/GameplayStatics.h"


UTimerManager* UTimerManagerFuncLib::GetTimerSubsystem(UObject* WorldContext)
{
	if (!WorldContext) return nullptr;
	return UTimerManager::GetInstance(WorldContext->GetWorld());
}

bool UTimerManagerFuncLib::HasTimer(UObject* WorldContext, FName TimerName)
{
	if (UTimerManager* Subsystem = GetTimerSubsystem(WorldContext))
	{
		return Subsystem->HasTimer(TimerName);
	}
	return false;
}

void UTimerManagerFuncLib::CancelDelay(UObject* WorldContext, FName TimerName)
{
	if (UTimerManager* Subsystem = GetTimerSubsystem(WorldContext))
	{
		Subsystem->CancelDelay(TimerName);
	}
}

void UTimerManagerFuncLib::PauseDelay(UObject* WorldContext, FName TimerName)
{
	if (UTimerManager* Subsystem = GetTimerSubsystem(WorldContext))
	{
		Subsystem->PauseDelay(TimerName);
	}
}

void UTimerManagerFuncLib::UnPauseDelay(UObject* WorldContext, FName TimerName)
{
	if (UTimerManager* Subsystem = GetTimerSubsystem(WorldContext))
	{
		Subsystem->UnPauseDelay(TimerName);
	}
}

bool UTimerManagerFuncLib::IsDelayActive(UObject* WorldContext, FName TimerName)
{
	if (UTimerManager* Subsystem = GetTimerSubsystem(WorldContext))
	{
		return Subsystem->IsDelayActive(TimerName);
	}
	return false;
}

float UTimerManagerFuncLib::GetRemainingTime(UObject* WorldContext, FName TimerName)
{
	if (UTimerManager* Subsystem = GetTimerSubsystem(WorldContext))
	{
		return Subsystem->GetRemainingTime(TimerName);
	}
	return -1.0f;
}
