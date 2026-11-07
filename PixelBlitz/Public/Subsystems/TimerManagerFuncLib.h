// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"
#include <tuple>
#include <type_traits>
#include "TimerManagerFuncLib.generated.h"



UCLASS(Abstract, MinimalAPI)
class UTimerManagerFuncLib : public UObject
{
	GENERATED_BODY()

public:
    template<typename Callable>
    static void SetDelay(UObject* WorldContext,Callable&& Callback,float Delay)
    {
        if (UTimerManager* Subsystem = UTimerManager::GetInstance(WorldContext))
        {
            Subsystem->SetDelay(MoveTemp(Callback),Delay);
        }
    }
    
    template<typename Callable>
    static void SetDelayLoop(UObject* WorldContext, const FName& TimerName, Callable&& Callback, float InRate, float SustainTime = -1, int LoopTimes = -1)
    {
        if (UTimerManager* Subsystem = UTimerManager::GetInstance(WorldContext))
        {
            Subsystem->SetDelayLoop(TimerName, MoveTemp(Callback), InRate, SustainTime, LoopTimes);
        }
    }

    
    /* 安全对象版本：内部弱引用，对象销毁后不触发回调。
     * 两种用法：
     *   1. 成员函数指针 + 绑定参数（跟在 Delay 之后）：
     *      SetDelaySafe(GetWorld(), this, &ABaseEnemy::SetLanding, time, false, 0);
     *   2. Lambda（第一个参数会收到校验后的 T*）：
     *      SetDelaySafe(GetWorld(), this, [](ABaseEnemy* E){ E->SetLanding(false, 0); }, time);
     */
    template<typename T, typename Callable, typename... ArgTypes>
    static void SetDelaySafe(UObject* WorldContext, T* Object, Callable&& Callback, float Delay, ArgTypes&&... Args)
    {
        if (!Object) return;

        TWeakObjectPtr<T> WeakObject(Object);
        auto BoundArgs = std::make_tuple(std::forward<ArgTypes>(Args)...);

        SetDelay(WorldContext,
            [WeakObject, Callback = MoveTemp(Callback), BoundArgs = MoveTemp(BoundArgs)]() mutable
            {
                if (!WeakObject.IsValid()) return;
                T* Local = WeakObject.Get();

                if constexpr (std::is_member_pointer_v<std::decay_t<Callable>>)
                {
                    std::apply([Local, &Callback](auto&&... Unpacked) mutable
                    {
                        (Local->*Callback)(std::forward<decltype(Unpacked)>(Unpacked)...);
                    }, std::move(BoundArgs));
                }
                else if constexpr (std::is_invocable_v<Callable&, T*, ArgTypes&...>)
                {
                    std::apply(Callback, std::tuple_cat(std::make_tuple(Local), std::move(BoundArgs)));
                }
                else
                {
                    std::apply(Callback, std::move(BoundArgs));
                }
            },
            Delay
        );
    }

    template<typename T, typename Callable>
    static void SetDelayLoopSafe(UObject* WorldContext, const FName& TimerName, T* Object, Callable&& Callback, float InRate, float SustainTime = -1, int LoopTimes = -1)
    {
        if (!Object) return;
        
        TWeakObjectPtr<T> WeakObject(Object);
        SetDelayLoop(WorldContext, TimerName,
            [WeakObject, Callback = MoveTemp(Callback)]() mutable
            {
                if (T* Object = WeakObject.Get())
                {
                   (Object->*Callback)();
                }
            },
            InRate, SustainTime, LoopTimes
        );
    }

    
    /* 设置可重触发延迟
     */
    template<typename Callable>
    static void SetRetriggerableDelay(UObject* WorldContext, const FName& TimerName, Callable&& Callback,float Delay)
    {
        if (UTimerManager* Subsystem = UTimerManager::GetInstance(WorldContext))
        {
            Subsystem->SetRetriggerableDelay(
                TimerName,
                MoveTemp(Callback),
                Delay
            );
        }
    }
    
    
    
    /* 安全对象版本
     */
    template<typename T, typename Callable>
    static void SetRetriggerableDelaySafe(UObject* WorldContext,FName TimerName,T* Object, Callable&& Callback,float Delay)
    {
        if (!Object) return;
        
        TWeakObjectPtr<T> WeakObject(Object);
        
        SetRetriggerableDelay(
            WorldContext,
            TimerName,
            [WeakObject, Callback = MoveTemp(Callback)]() mutable
            {
                if (WeakObject.IsValid())
                {
                    Callback();
                }
            },
            Delay
        );
    }

    
    template<typename T>
    static void SetRetriggerableDelaySafe(UObject* WorldContext,FName TimerName,T* Object,  FTimerDelegate::TMethodPtr<T> Callback, float Delay)
    {
        if (!Object) return;
        
        TWeakObjectPtr<T> WeakObject(Object);
        SetRetriggerableDelay(
            WorldContext,
            TimerName,
            [WeakObject, Callback = MoveTemp(Callback)]() mutable
            {
                if (T* Object = WeakObject.Get())
                {
                    (Object->*Callback)();
                }
            },
            Delay
        );
    }

    

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Timer", meta = (WorldContext = "WorldContext"))
    static bool HasTimer(UObject* WorldContext, FName TimerName);
    
    UFUNCTION(BlueprintCallable, Category = "Timer", meta = (WorldContext = "WorldContext"))
    static void CancelDelay(UObject* WorldContext, FName TimerName);

    UFUNCTION(BlueprintCallable, Category = "Timer", meta = (WorldContext = "WorldContext"))
    static void PauseDelay(UObject* WorldContext, FName TimerName);

    UFUNCTION(BlueprintCallable, Category = "Timer", meta = (WorldContext = "WorldContext"))
    static void UnPauseDelay(UObject* WorldContext, FName TimerName);
    
    UFUNCTION(BlueprintCallable, Category = "Timer", meta = (WorldContext = "WorldContext"))
    static bool IsDelayActive(UObject* WorldContext, FName TimerName);

    UFUNCTION(BlueprintCallable, Category = "Timer", meta = (WorldContext = "WorldContext"))
    static float GetRemainingTime(UObject* WorldContext, FName TimerName);
	
};