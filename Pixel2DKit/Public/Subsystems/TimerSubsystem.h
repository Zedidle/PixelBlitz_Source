// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TimerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API UTimerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	// 存储所有活动的计时器
	TMap<FName, FTimerHandle> ActiveTimers;
	TMap<FName, int> RemLoopTimes;

	
public:
	// 初始化子系统
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 清理子系统
	virtual void Deinitialize() override;


	void SetDelay(TFunction<void()>&& Callback, float DelayDuration);

	void SetDelayLoop(const FName& TimerName, TFunction<void()>&& Callback, float InRate, float SustainTime = -1, int LoopTimes = -1);
	
	template<typename T>
	void SetDelay(T* Object, FTimerDelegate::TMethodPtr<T> Function, float DelayDuration)
	{
		TWeakObjectPtr<T> WeakObject(Object);
		SetDelay([WeakObject, Function = MoveTemp(Function)]() mutable
		{
			if (T* Obj = WeakObject.Get())
			{
				(Obj->*Function)();
			}
		}, DelayDuration);
	}



	/* 通用版：支持任意可调用对象
	 * 
	 * @param TimerName 计时器唯一标识符
	 * @param Callback 延迟结束后调用的函数
	 * @param DelayDuration 延迟时间（秒）
	 */
	void SetRetriggerableDelay(const FName& TimerName, TFunction<void()>&& Callback, float DelayDuration);

	
	template<typename T>
	void SetRetriggerableDelay(const FName& TimerName, T* Object, FTimerDelegate::TMethodPtr<T> Function, float DelayDuration)
	{
		TWeakObjectPtr<T> WeakObject(Object);
		SetRetriggerableDelay( TimerName,
			[WeakObject, Function = MoveTemp(Function)]() mutable
			{
				if (T* Obj = WeakObject.Get())
				{
					(Obj->*Function)();
				}
			},
			DelayDuration
		);
	}


	template<typename... Args>
	void SetRetriggerableDelayWithArgs(
		FName TimerName,
		TFunction<void(Args...)>&& Callback,
		float DelayDuration,
		Args... Arg
	)
	{
		// 使用完美转发
		SetRetriggerableDelay(
			TimerName,
			[Callback = MoveTemp(Callback), Arg...]() mutable
			{
				Callback(Forward<Args>(Arg)...);
			},
			DelayDuration
		);
	}
	
	 /* 取消指定的延迟计时器
	 * 
	 * @param TimerName 要取消的计时器名称
	 */
	void CancelDelay(FName TimerName);

	/* 检查指定的延迟计时器是否处于活动状态
	 * 
	 * @param TimerName 要检查的计时器名称
	 * @return 如果计时器处于活动状态返回true，否则false
	 */
	bool IsDelayActive(FName TimerName) const;

	float GetRemainingTime(FName TimerName) const;


	// 暂停指定的定时器
	void PauseDelay(FName TimerName);
	// 恢复指定的定时器
	void UnPauseDelay(FName TimerName);
	// 检查定时器是否处于暂停状态
	bool IsDelayPaused(FName TimerName) const;
	
};



