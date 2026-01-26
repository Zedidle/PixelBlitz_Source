// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/TimerManager.h"

void UTimerManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ActiveTimers.Empty();
}

void UTimerManager::Deinitialize()
{
	// 清理所有计时器
	UWorld* World = GetWorld();
	if (World)
	{
		FTimerManager& TimerManager = World->GetTimerManager();
		for (auto& Pair : ActiveTimers)
		{
			TimerManager.ClearTimer(Pair.Value);
		}
	}
	ActiveTimers.Empty();
    
	Super::Deinitialize();
}

UTimerManager* UTimerManager::GetInstance(UWorld* World)
{
	if (World)
	{
		return World->GetSubsystem<UTimerManager>();
	}
	return nullptr;	
}

void UTimerManager::SetDelay(TFunction<void()>&& Callback, float DelayDuration)
{
	if (DelayDuration <= 0.0f) return;
	
	UWorld* World = GetWorld();
	if (!World) return;

	FTimerDelegate Delegate;
	Delegate.BindLambda(
		[Callback = MoveTemp(Callback)]() mutable
		{
			Callback();
		}
	);
	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(Handle, Delegate, DelayDuration, false);
}

void UTimerManager::SetDelayLoop(const FName& TimerName, TFunction<void()>&& Callback, float InRate, float SustainTime, int LoopTimes)
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	CancelDelay(TimerName);
	
	FTimerDelegate Delegate;
	Delegate.BindLambda(
		[TimerName, WeakThis = TWeakObjectPtr(this), Callback = MoveTemp(Callback)]() mutable
		{
			if (!WeakThis.IsValid()) return;
			Callback();

			if (WeakThis->RemLoopTimes.Contains(TimerName))
			{
				if (--WeakThis->RemLoopTimes[TimerName] == 0)
				{
					WeakThis->CancelDelay(TimerName);
				}
			}
		}
	);
	
	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(Handle, Delegate, InRate, true);
	ActiveTimers.Add(TimerName, Handle);
	RemLoopTimes.Add(TimerName, LoopTimes);
	
	if (SustainTime > 0.0f)
	{
		SetDelay([WeakThis=TWeakObjectPtr(this), TimerName]
		{
			if (WeakThis.IsValid())
			{
				WeakThis->CancelDelay(TimerName);				
			}
		}, SustainTime);
	}
}


void UTimerManager::SetRetriggerableDelay(const FName& TimerName, TFunction<void()>&& Callback, float DelayDuration)
{
	if (DelayDuration <= 0.0f) return;
	
	UWorld* World = GetWorld();
	if (!World) return;

	CancelDelay(TimerName);
	
	FTimerDelegate Delegate;
	Delegate.BindLambda([WeakThis = TWeakObjectPtr(this), TimerName, Callback = MoveTemp(Callback)]() mutable
		{
			if (!WeakThis.IsValid()) return;
			
			FTimerHandle Handle;
			if (WeakThis->ActiveTimers.RemoveAndCopyValue(TimerName, Handle))
			{
				Callback();
			}
		}
	);
    
	// 创建并存储新计时器
	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(Handle, Delegate, DelayDuration, false);
	ActiveTimers.Add(TimerName, Handle);
}


void UTimerManager::CancelDelay(FName TimerName)
{
	check(IsInGameThread());
	
	if (FTimerHandle* Handle = ActiveTimers.Find(TimerName))
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().ClearTimer(*Handle);
		}
		ActiveTimers.Remove(TimerName);
	}
}

bool UTimerManager::HasTimer(FName TimerName)
{
	return ActiveTimers.Contains(TimerName);
}

bool UTimerManager::IsDelayActive(FName TimerName) const
{
	if (const FTimerHandle* Handle = ActiveTimers.Find(TimerName))
	{
		UWorld* World = GetWorld();
		return World && World->GetTimerManager().IsTimerActive(*Handle);
	}
	return false;
}

float UTimerManager::GetRemainingTime(FName TimerName) const
{
	if (const FTimerHandle* Handle = ActiveTimers.Find(TimerName))
	{
		UWorld* World = GetWorld();
		if (World)
		{
			return World->GetTimerManager().GetTimerRemaining(*Handle);
		}
	}
	return -1.0f;
}


void UTimerManager::PauseDelay(FName TimerName)
{
	if (FTimerHandle* Handle = ActiveTimers.Find(TimerName))
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().PauseTimer(*Handle);
		}
	}
}

void UTimerManager::UnPauseDelay(FName TimerName)
{
	if (FTimerHandle* Handle = ActiveTimers.Find(TimerName))
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().UnPauseTimer(*Handle);
		}
	}
}

bool UTimerManager::IsDelayPaused(FName TimerName) const
{
	if (const FTimerHandle* Handle = ActiveTimers.Find(TimerName))
	{
		UWorld* World = GetWorld();
		return World && World->GetTimerManager().IsTimerPaused(*Handle);
	}
	return false;
}