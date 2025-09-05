// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TimerSubsystemFuncLib.generated.h"


UCLASS(Abstract, MinimalAPI)
class UTimerSubsystemFuncLib : public UObject
{
	GENERATED_BODY()

public:
    /* 获取计时器子系统
     */
    UFUNCTION(BlueprintCallable, Category = "Timer", meta = (WorldContext = "WorldContext"))
    static UTimerSubsystem* GetTimerSubsystem(UObject* WorldContext);


    template<typename Callable>
    static void SetDelay(UObject* WorldContext,Callable&& Callback,float Delay)
    {
        if (UTimerSubsystem* Subsystem = GetTimerSubsystem(WorldContext))
        {
            Subsystem->SetDelay(MoveTemp(Callback),Delay);
        }
    }
    
    template<typename Callable>
    static void SetDelayLoop(UObject* WorldContext, const FName& TimerName, Callable&& Callback, float InRate, float SustainTime = 0.f)
    {
        if (UTimerSubsystem* Subsystem = GetTimerSubsystem(WorldContext))
        {
            Subsystem->SetDelayLoop(TimerName, MoveTemp(Callback), InRate, SustainTime);
        }
    }

    
    template<typename T, typename Callable>
    static void SetDelaySafe(UObject* WorldContext,T* Object, Callable&& Callback,float Delay)
    {
        if (!Object) return;
        
        TWeakObjectPtr<T> WeakObject(Object);
        SetDelay(WorldContext,
            [WeakObject, Callback = MoveTemp(Callback)]() mutable
            {
                if (T* Object = WeakObject.IsValid())
                {
                   (Object->*Callback)();
                }
            },
            Delay
        );
    }
    
    
    /* 设置可重触发延迟
     */
    template<typename Callable>
    static void SetRetriggerableDelay(UObject* WorldContext, const FName& TimerName, Callable&& Callback,float Delay)
    {
        if (UTimerSubsystem* Subsystem = GetTimerSubsystem(WorldContext))
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

    

    UFUNCTION(BlueprintCallable, Category = "Timer", meta = (WorldContext = "WorldContext"))
    static void CancelDelay(UObject* WorldContext, FName TimerName);
    

    UFUNCTION(BlueprintCallable, Category = "Timer", meta = (WorldContext = "WorldContext"))
    static bool IsDelayActive(UObject* WorldContext, FName TimerName);
    

    UFUNCTION(BlueprintCallable, Category = "Timer", meta = (WorldContext = "WorldContext"))
    static float GetRemainingTime(UObject* WorldContext, FName TimerName);
	
};