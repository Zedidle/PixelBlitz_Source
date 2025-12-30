// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/SkillManager.h"
#include "Pixel2DKit.h"
#include "Fight/Skills/BaseSkill.h"
#include "Engine/World.h"
#include "Fight/Skills/BaseRemoteShotSkill.h"
#include "Fight/Skills/BaseTraceProjectileSkill.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "Utilitys/DebugFuncLab.h"

void USkillManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

}

void USkillManager::Deinitialize()
{
    UTimerSubsystemFuncLib::CancelDelay(GetWorld(),"USkillManager_SameSkillFarFromEach");
    
    // 清理所有技能实例
    for (auto& Pair : SkillPoolMap)
    {
        FSkillPool& Pool = Pair.Value;
        for (auto& Skill : Pool.SkillsWaiting)
        {
            if (Skill.IsValid())
            {
                Skill->Destroy();
            }
        }
        for (auto& Skill : Pool.SkillsActivated)
        {
            if (Skill.IsValid())
            {
                Skill->Destroy();
            }
        }

        Pool.SkillsWaiting.Empty();
        Pool.SkillsActivated.Empty();
    }
    SkillPoolMap.Empty();
    
    Super::Deinitialize();
}

void USkillManager::PreallocateSkillPool(TSubclassOf<ABaseSkill> SkillClass, int32 Count)
{
    if (!SkillClass || Count <= 0) return;

    if (SkillPoolMap.IsEmpty())
    {
        if (!UTimerSubsystemFuncLib::HasTimer(GetWorld(), "USkillManager_SameSkillFarFromEach"))
        {
            UTimerSubsystemFuncLib::SetDelayLoopSafe(GetWorld(), "USkillManager_SameSkillFarFromEach", this, &ThisClass::Tick_SameSkillFarFromEach, 0.1);
        }
    }
    
    FSkillPool& Pool = GetOrCreateSkillPool(SkillClass);
    for (int32 i = 0; i < Count; i++)
    {
        if (ABaseSkill* NewSkill = CreateNewSkill(SkillClass))
        {
            Pool.SkillsWaiting.Add(NewSkill);
        }
    }
}

ABaseSkill* USkillManager::ActivateSkill(TSubclassOf<ABaseSkill> SkillClass, const FTransform& SpawnTransform)
{
    FSkillPool* Pool = SkillPoolMap.Find(SkillClass);
    if (!Pool)
    {
        // 如果没有对应的池，先创建一个
        PreallocateSkillPool(SkillClass);
        Pool = SkillPoolMap.Find(SkillClass);
        if (!Pool) return nullptr;
    }

    ABaseSkill* AvailableSkill = Pool->GetAvailableSkill().Get();

    if (!AvailableSkill)
    {
        AvailableSkill = CreateNewSkill(SkillClass);
        Pool->SkillsActivated.Add(AvailableSkill);
    }

    InitializeSkill(AvailableSkill, SpawnTransform);

    UDebugFuncLab::ScreenMessage(FString::Printf(TEXT("USkillManager::ActivateSkill : %s,  Pool Waiting: %d   Active: %d"),
        *AvailableSkill->GetActorNameOrLabel(), Pool->SkillsWaiting.Num(), Pool->SkillsActivated.Num()));

    return AvailableSkill;
}

ABaseRemoteShotSkill* USkillManager::ActivateRemoteShotSkill(TSubclassOf<ABaseRemoteShotSkill> SkillClass,
    const FTransform& SpawnTransform, AActor* Owner, UNiagaraSystem* HitNiagara, float NewTargetLifeSpan,
    float InitSpeed, float MaxSpeed, float MaxTraceDistance, FVector Direction, int Damage, int RemHitNum,
    float DamageDecreasePercentPerHit, FVector Knockback, int RemSpringNum, int RemSplitNum)
{
    UClass* BaseSkillClass = SkillClass.Get();
    ABaseSkill* BaseSkill = ActivateSkill(TSubclassOf<ABaseSkill>(BaseSkillClass), SpawnTransform);
    ABaseRemoteShotSkill* RemoteShotSkill = Cast<ABaseRemoteShotSkill>(BaseSkill);

    CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(RemoteShotSkill, nullptr);

    RemoteShotSkill->Owner = Owner;
    RemoteShotSkill->HitNiagara = HitNiagara;
    RemoteShotSkill->LifeSpan = NewTargetLifeSpan;
    RemoteShotSkill->InitSpeed = InitSpeed;
    RemoteShotSkill->MaxSpeed = MaxSpeed;
    RemoteShotSkill->MaxTraceDistance = MaxTraceDistance;
    RemoteShotSkill->Direction = Direction;
    RemoteShotSkill->Damage = Damage;
    RemoteShotSkill->RemHitNum = RemHitNum;
    RemoteShotSkill->DamageDecreasePercentPerHit = DamageDecreasePercentPerHit;
    RemoteShotSkill->Knockback = Knockback;
    RemoteShotSkill->RemSpringNum = RemSpringNum;
    RemoteShotSkill->RemSplitNum = RemSplitNum;

    RemoteShotSkill->SetActive(true);
    
    return RemoteShotSkill;
}

ABaseTraceProjectileSkill* USkillManager::ActivateTraceProjectileSkill(TSubclassOf<ABaseTraceProjectileSkill> SkillClass,
    const FTransform& SpawnTransform, AActor* Owner, UNiagaraSystem* HitNiagara, AActor* Target, bool bIdle,
    float NewTargetLifeSpan, float CurMagnitude, float InRangeNear, float MagnitudeScaleNear, float MagnitudeScaleFar,
    float InitSpeed, float MaxSpeed, float MaxTraceDistance, int Damage, int RemHitNum, bool bForce,
    float DamageDecreasePercentPerHit, FVector Knockback, float RepelSameSpeed, float RepelSameDistance)
{
    UClass* BaseSkillClass = SkillClass.Get();
    ABaseSkill* BaseSkill = ActivateSkill(TSubclassOf<ABaseSkill>(BaseSkillClass), SpawnTransform);
    ABaseTraceProjectileSkill* TraceProjectileSkill = Cast<ABaseTraceProjectileSkill>(BaseSkill);

    CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(TraceProjectileSkill, nullptr);

    TraceProjectileSkill->Owner = Owner;
    TraceProjectileSkill->HitNiagara = HitNiagara;
    TraceProjectileSkill->Target = Target;
    TraceProjectileSkill->bIdle = bIdle;
    TraceProjectileSkill->LifeSpan = NewTargetLifeSpan;
    TraceProjectileSkill->CurMagnitude = CurMagnitude;
    TraceProjectileSkill->InRangeNear = InRangeNear;
    TraceProjectileSkill->MagnitudeScaleNear = MagnitudeScaleNear;
    TraceProjectileSkill->MagnitudeScaleFar = MagnitudeScaleFar;
    TraceProjectileSkill->InitSpeed = InitSpeed;
    TraceProjectileSkill->MaxSpeed = MaxSpeed;
    TraceProjectileSkill->MaxTraceDistance = MaxTraceDistance;
    TraceProjectileSkill->Damage = Damage;
    TraceProjectileSkill->RemHitNum = RemHitNum;
    TraceProjectileSkill->bForce = bForce;
    TraceProjectileSkill->DamageDecreasePercentPerHit = DamageDecreasePercentPerHit;
    TraceProjectileSkill->Knockback = Knockback;
    TraceProjectileSkill->RepelSameSpeed = RepelSameSpeed;
    TraceProjectileSkill->RepelSameDistance = RepelSameDistance;

    TraceProjectileSkill->SetActive(true);
    
    return TraceProjectileSkill;
}

void USkillManager::DeactivateSkill(ABaseSkill* SkillActor)
{
    if (!SkillActor) return;

    TSubclassOf<ABaseSkill> SkillClass = SkillActor->GetClass();
    if (FSkillPool* Pool = SkillPoolMap.Find(SkillClass))
    {
        Pool->ReturnSkill(SkillActor);
        UDebugFuncLab::ScreenMessage(FString::Printf(TEXT("USkillManager::DeactivateSkill : %s,  Pool Waiting: %d   Active: %d"),
        *SkillActor->GetActorNameOrLabel(), Pool->SkillsWaiting.Num(), Pool->SkillsActivated.Num()));
    }
}

void USkillManager::GetPoolStats(TSubclassOf<ABaseSkill> SkillClass, int32& OutTotalCount, int32& OutActiveCount)
{
    if (FSkillPool* Pool = SkillPoolMap.Find(SkillClass))
    {
        OutTotalCount = Pool->SkillsActivated.Num() + Pool->SkillsWaiting.Num();
        OutActiveCount = Pool->SkillsActivated.Num();
    }
}

ABaseSkill* USkillManager::CreateNewSkill(TSubclassOf<ABaseSkill> SkillClass)
{
    ABaseSkill* SkillActor = GetWorld()->SpawnActor<ABaseSkill>(SkillClass);
    return SkillActor;
}

void USkillManager::InitializeSkill(ABaseSkill* Skill, const FTransform& SpawnTransform)
{
    if (Skill)
    {
        Skill->SetActorTransform(SpawnTransform);
    }
}

FSkillPool& USkillManager::GetOrCreateSkillPool(TSubclassOf<ABaseSkill> SkillClass)
{
    return SkillPoolMap.FindOrAdd(SkillClass);
}

void USkillManager::Tick_SameSkillFarFromEach()
{
    for (auto& Pair : SkillPoolMap)
    {
        FSkillPool& Pool = Pair.Value;
        for (auto& SkillA : Pool.SkillsActivated)
        {
            if (SkillA->RepelSameSpeed <= 0) continue;
            
            for (auto& SkillB : Pool.SkillsActivated)
            {
                if (SkillA == SkillB) continue;
                
                float Distance = SkillA->GetDistanceTo(SkillB.Get());
                if (Distance > SkillA->RepelSameDistance) continue;

                SkillA->RepelFromActor(SkillB.Get());
            }
        }
    }
}
