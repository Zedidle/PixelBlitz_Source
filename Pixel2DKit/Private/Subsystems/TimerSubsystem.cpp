// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/TimerSubsystem.h"

void UTimerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ActiveTimers.Empty();
}

void UTimerSubsystem::Deinitialize()
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

void UTimerSubsystem::SetRetriggerableDelay(FName TimerName, TFunction<void()>&& Callback, float DelayDuration)
{
	UWorld* World = GetWorld();
	if (!World) return;

	CancelDelay(TimerName);

	FTimerDelegate Delegate;
	Delegate.BindLambda(
		[this, TimerName, Callback = MoveTemp(Callback)]() mutable
		{
			// 先移除记录
			FTimerHandle Handle;
			if (ActiveTimers.RemoveAndCopyValue(TimerName, Handle))
			{
				// 执行回调（使用移动后的Callback）
				Callback();
			}
		}
	);
    
	// 创建并存储新计时器
	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(Handle, Delegate, DelayDuration, false);
	ActiveTimers.Add(TimerName, Handle);
}


void UTimerSubsystem::CancelDelay(FName TimerName)
{
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

bool UTimerSubsystem::IsDelayActive(FName TimerName) const
{
	if (const FTimerHandle* Handle = ActiveTimers.Find(TimerName))
	{
		UWorld* World = GetWorld();
		return World && World->GetTimerManager().IsTimerActive(*Handle);
	}
	return false;
}
