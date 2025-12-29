// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/SkillManager.h"
#include "Pixel2DKit.h"
#include "Fight/Skills/BaseSkill.h"
#include "Engine/World.h"
#include "Fight/Skills/BaseRemoteShotSkill.h"
#include "Utilitys/DebugFuncLab.h"

void USkillManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("SkillManager Subsystem Initialized"));
}

void USkillManager::Deinitialize()
{
    // 清理所有技能实例
    for (auto& Pair : SkillPoolMap)
    {
        FSkillPool& Pool = Pair.Value;
        for (ABaseSkill* Skill : Pool.SkillDoubleLinkedList)
        {
            if (IsValid(Skill))
            {
                Skill->Destroy();
            }
        }
        Pool.SkillDoubleLinkedList.Empty();
    }
    SkillPoolMap.Empty();
    
    Super::Deinitialize();
}

void USkillManager::PreallocateSkillPool(TSubclassOf<ABaseSkill> SkillClass, int32 Count)
{
    if (!SkillClass || Count <= 0) return;

    FSkillPool& Pool = GetOrCreateSkillPool(SkillClass);
    for (int32 i = 0; i < Count; i++)
    {
        if (ABaseSkill* NewSkill = CreateNewSkill(SkillClass))
        {
            // 新技能直接加入队头（失效池）
            NewSkill->SetActive(false);
            Pool.SkillDoubleLinkedList.AddHead(NewSkill);
            Pool.TotalCount++;
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

    // 从对头获取失效待用的技能
    ABaseSkill* AvailableSkill = nullptr;
    if (auto Head = Pool->SkillDoubleLinkedList.GetHead())
    {
        ABaseSkill* Skill = Head->GetValue();
        if (Skill && !Skill->IsActive())
        {
            AvailableSkill = Skill;
            Pool->SkillDoubleLinkedList.RemoveNode(Head);
        }
    }

    // 如果没有可用技能，创建新实例
    if (!AvailableSkill)
    {
        AvailableSkill = CreateNewSkill(SkillClass);
        if (AvailableSkill)
        {
            Pool->TotalCount++;
        }
    }

    if (AvailableSkill)
    {
        InitializeSkill(AvailableSkill, SpawnTransform);
        
        Pool->SkillDoubleLinkedList.AddTail(AvailableSkill);
        Pool->ActiveCount++;
        UDebugFuncLab::ScreenMessage(FString::Printf(TEXT("USkillManager::ActivateSkill : %s,  Pool TotalCount: %d   ActiveCount: %d"), *AvailableSkill->GetActorNameOrLabel(), Pool->TotalCount, Pool->ActiveCount));
    }

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

    RemoteShotSkill->StartTrace();
    
    return RemoteShotSkill;
}

void USkillManager::DeactivateSkill(ABaseSkill* SkillActor)
{
    if (!SkillActor) return;

    TSubclassOf<ABaseSkill> SkillClass = SkillActor->GetClass();
    FSkillPool* Pool = SkillPoolMap.Find(SkillClass);
    
    if (Pool && Pool->ActiveCount > 0)
    {
        // 从当前位置移除
        Pool->SkillDoubleLinkedList.RemoveNode(SkillActor);
        
        // 重置技能状态
        ResetSkill(SkillActor);
        
        // 添加到队头（失效池）
        Pool->SkillDoubleLinkedList.AddHead(SkillActor);
        Pool->ActiveCount--;
    }
}

void USkillManager::GetPoolStats(TSubclassOf<ABaseSkill> SkillClass, int32& OutTotalCount, int32& OutActiveCount)
{
    FSkillPool* Pool = SkillPoolMap.Find(SkillClass);
    if (Pool)
    {
        OutTotalCount = Pool->TotalCount;
        OutActiveCount = Pool->ActiveCount;
    }
}

ABaseSkill* USkillManager::CreateNewSkill(TSubclassOf<ABaseSkill> SkillClass)
{
    ABaseSkill* SkillActor = GetWorld()->SpawnActor<ABaseSkill>(SkillClass);
    SkillActor->SetActive(false);
    return SkillActor;
}

void USkillManager::InitializeSkill(ABaseSkill* Skill, const FTransform& SpawnTransform)
{
    Skill->SetActorTransform(SpawnTransform);
}

void USkillManager::ResetSkill(ABaseSkill* Skill)
{
    // Skill->SetActive(false);
}

FSkillPool& USkillManager::GetOrCreateSkillPool(TSubclassOf<ABaseSkill> SkillClass)
{
    return SkillPoolMap.FindOrAdd(SkillClass);
}
