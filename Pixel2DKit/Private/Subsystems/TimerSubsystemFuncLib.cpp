// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/TimerSubsystemFuncLib.h"

#include "Kismet/GameplayStatics.h"


UTimerSubsystem* UTimerSubsystemFuncLib::GetTimerSubsystem(UObject* WorldContext)
{
	if (!WorldContext) return nullptr;
	
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContext);
	if (!GameInstance) return nullptr;
	
	if (UTimerSubsystem* Subsystem = GameInstance->GetSubsystem<UTimerSubsystem>())
	{
		return Subsystem;
	}
	return nullptr;
}

bool UTimerSubsystemFuncLib::HasTimer(UObject* WorldContext, FName TimerName)
{
	if (UTimerSubsystem* Subsystem = GetTimerSubsystem(WorldContext))
	{
		return Subsystem->HasTimer(TimerName);
	}
	return false;
}

void UTimerSubsystemFuncLib::CancelDelay(UObject* WorldContext, FName TimerName)
{
	if (UTimerSubsystem* Subsystem = GetTimerSubsystem(WorldContext))
	{
		Subsystem->CancelDelay(TimerName);
	}
}

void UTimerSubsystemFuncLib::PauseDelay(UObject* WorldContext, FName TimerName)
{
	if (UTimerSubsystem* Subsystem = GetTimerSubsystem(WorldContext))
	{
		Subsystem->PauseDelay(TimerName);
	}
}

void UTimerSubsystemFuncLib::UnPauseDelay(UObject* WorldContext, FName TimerName)
{
	if (UTimerSubsystem* Subsystem = GetTimerSubsystem(WorldContext))
	{
		Subsystem->UnPauseDelay(TimerName);
	}
}

bool UTimerSubsystemFuncLib::IsDelayActive(UObject* WorldContext, FName TimerName)
{
	if (UTimerSubsystem* Subsystem = GetTimerSubsystem(WorldContext))
	{
		return Subsystem->IsDelayActive(TimerName);
	}
	return false;
}

float UTimerSubsystemFuncLib::GetRemainingTime(UObject* WorldContext, FName TimerName)
{
	if (UTimerSubsystem* Subsystem = GetTimerSubsystem(WorldContext))
	{
		return Subsystem->GetRemainingTime(TimerName);
	}
	return -1.0f;
}
