// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Fight/Skills/BaseSkill.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SkillManager.generated.h"

class ABaseSkill;

// 技能池结构
USTRUCT()
struct FSkillPool
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<TWeakObjectPtr<ABaseSkill>> SkillsWaiting;  // 待机中

	UPROPERTY()
	TArray<TWeakObjectPtr<ABaseSkill>> SkillsActivated;  // 生效中

	// 安全的技能获取方法
	TWeakObjectPtr<ABaseSkill> GetAvailableSkill()
	{
		// 首先检查数组是否为空
		if (SkillsWaiting.Num() == 0)
		{
			return nullptr;
		}

		// 1. 直接获取队尾元素
		TWeakObjectPtr<ABaseSkill> SkillPtr = SkillsWaiting.Last();
    
		// 2. 检查其有效性
		if (SkillPtr.IsValid() && SkillPtr->IsAvailableForReuse())
		{
			// 3. 从等待队列中移除并加入激活队列
			SkillsWaiting.RemoveAt(SkillsWaiting.Num() - 1); // 移除最后一个元素
			SkillsActivated.Add(SkillPtr);
			return SkillPtr;
		}
		
		if (!SkillPtr.IsValid())
		{
			// 4. 如果无效，移除并继续尝试（可选的递归或循环逻辑）
			SkillsWaiting.RemoveAt(SkillsWaiting.Num() - 1);
			// 可以考虑递归调用自身，但注意栈溢出风险，对于预期无效对象不多的情况是可行的
			return GetAvailableSkill();
		}
    
		// 5. 如果有效但不可重用（根据业务逻辑判断是否可能发生）
		return nullptr;
	}

	
	// 安全的技能返还方法
	void ReturnSkill(TWeakObjectPtr<ABaseSkill> SkillPtr)
	{
		if (!SkillPtr.IsValid()) return;

		// 从激活数组中移除
		SkillsActivated.Remove(SkillPtr);
		SkillsWaiting.Add(SkillPtr);
	}
	

	// 清理所有无效引用
	void CleanupInvalidReferences()
	{
		SkillsWaiting.RemoveAll([](const TWeakObjectPtr<ABaseSkill>& Ptr) {
			return !Ptr.IsValid();
		});
		
		SkillsActivated.RemoveAll([](const TWeakObjectPtr<ABaseSkill>& Ptr) {
			return !Ptr.IsValid();
		});
	}




	
	// 默认构造函数
	FSkillPool() = default;
    
	// 拷贝构造函数
	FSkillPool(const FSkillPool& Other)
	{
		for (auto Node : Other.SkillsWaiting)
		{
			SkillsWaiting.Add(Node);
		}

		for (auto Node : Other.SkillsActivated)
		{
			SkillsActivated.Add(Node);
		}
	}
    
	// 拷贝赋值运算符
	FSkillPool& operator=(const FSkillPool& Other)
	{
		if (this != &Other)
		{
			SkillsWaiting = Other.SkillsWaiting;
			SkillsActivated = Other.SkillsActivated;
		}
		return *this;
	}
};


UCLASS()
class PIXEL2DKIT_API USkillManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// 初始化子系统
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// 预分配技能池
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	void PreallocateSkillPool(TSubclassOf<ABaseSkill> SkillClass, int32 Count = 1);

	// 激活技能（从池中获取或创建新实例）
	ABaseSkill* ActivateSkill(TSubclassOf<ABaseSkill> SkillClass, const FTransform& SpawnTransform);
	
	// 激活一个远程射击（投掷物）技能
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	ABaseRemoteShotSkill* ActivateRemoteShotSkill(TSubclassOf<ABaseRemoteShotSkill> SkillClass, const FTransform& SpawnTransform
		,AActor* Owner, UNiagaraSystem* HitNiagara = nullptr, float NewTargetLifeSpan = 2.0f, float InitSpeed = 500.f, float MaxSpeed = 1000.f
		,float MaxTraceDistance = 400.f, FVector Direction = FVector(1, 0, 0), int Damage = 1, int RemHitNum = 2
		,float DamageDecreasePercentPerHit = 0.1, FVector Knockback = FVector(50, 50, 100), int RemSpringNum = 0, int RemSplitNum = 0);

	// 激活一个远程追踪法术
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	ABaseTraceProjectileSkill* ActivateTraceProjectileSkill(TSubclassOf<ABaseTraceProjectileSkill> SkillClass, const FTransform& SpawnTransform
		,AActor* Owner, UNiagaraSystem* HitNiagara = nullptr, AActor* Target = nullptr, bool bIdle = true, float NewTargetLifeSpan = 2.0f, float CurMagnitude = 3000.0f
		,float InRangeNear = 1000.0f, float MagnitudeScaleNear = 2.0f, float MagnitudeScaleFar = 1.0f, float InitSpeed = 500.0f
		,float MaxSpeed = 1000.0f, float MaxTraceDistance = 600.0f, int Damage = 1, int RemHitNum = 1, bool bForce = false
		,float DamageDecreasePercentPerHit = 0.1, FVector Knockback = FVector(50, 50, 100)
		,float RepelSameSpeed = 0.0f, float RepelSameDistance = 50.0f);
	
	// 回收技能
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	void DeactivateSkill(ABaseSkill* SkillActor);

	// 获取池统计信息
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	void GetPoolStats(TSubclassOf<ABaseSkill> SkillClass, int32& OutTotalCount, int32& OutActiveCount);

private:

	// 技能类别到池的映射
	UPROPERTY()
	TMap<TSubclassOf<ABaseSkill>, FSkillPool> SkillPoolMap;

	// 内部方法
	ABaseSkill* CreateNewSkill(TSubclassOf<ABaseSkill> SkillClass);
	void InitializeSkill(ABaseSkill* Skill, const FTransform& SpawnTransform);
	FSkillPool& GetOrCreateSkillPool(TSubclassOf<ABaseSkill> SkillClass);



	void Tick_SameSkillFarFromEach();
};
