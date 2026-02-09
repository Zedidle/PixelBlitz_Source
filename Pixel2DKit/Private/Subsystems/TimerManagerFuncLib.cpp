// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/TimerManagerFuncLib.h"

#include "Kismet/GameplayStatics.h"


bool UTimerManagerFuncLib::HasTimer(UObject* WorldContext, FName TimerName)
{
	if (UTimerManager* Subsystem = UTimerManager::GetInstance(WorldContext))
	{
		return Subsystem->HasTimer(TimerName);
	}
	return false;
}

void UTimerManagerFuncLib::CancelDelay(UObject* WorldContext, FName TimerName)
{
	if (UTimerManager* Subsystem = UTimerManager::GetInstance(WorldContext))
	{
		Subsystem->CancelDelay(TimerName);
	}
}

void UTimerManagerFuncLib::PauseDelay(UObject* WorldContext, FName TimerName)
{
	if (UTimerManager* Subsystem = UTimerManager::GetInstance(WorldContext))
	{
		Subsystem->PauseDelay(TimerName);
	}
}

void UTimerManagerFuncLib::UnPauseDelay(UObject* WorldContext, FName TimerName)
{
	if (UTimerManager* Subsystem = UTimerManager::GetInstance(WorldContext))
	{
		Subsystem->UnPauseDelay(TimerName);
	}
}

bool UTimerManagerFuncLib::IsDelayActive(UObject* WorldContext, FName TimerName)
{
	if (UTimerManager* Subsystem = UTimerManager::GetInstance(WorldContext))
	{
		return Subsystem->IsDelayActive(TimerName);
	}
	return false;
}

float UTimerManagerFuncLib::GetRemainingTime(UObject* WorldContext, FName TimerName)
{
	if (UTimerManager* Subsystem = UTimerManager::GetInstance(WorldContext))
	{
		return Subsystem->GetRemainingTime(TimerName);
	}
	return -1.0f;
}
