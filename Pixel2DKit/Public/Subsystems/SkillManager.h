// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SkillManager.generated.h"

class ABaseSkill;

// 技能池结构
USTRUCT()
struct FSkillPool
{
	GENERATED_BODY()
	
	TDoubleLinkedList<ABaseSkill*> SkillDoubleLinkedList;  // 双向链表：队头失效，队尾活跃
	int32 TotalCount = 0;
	int32 ActiveCount = 0;


	// 默认构造函数
	FSkillPool() = default;
    
	// 拷贝构造函数
	FSkillPool(const FSkillPool& Other)
		: TotalCount(Other.TotalCount)
		, ActiveCount(Other.ActiveCount)
	{
		// 手动拷贝链表内容，而不是直接拷贝链表对象
		for (auto Node = Other.SkillDoubleLinkedList.GetHead(); Node; Node = Node->GetNextNode())
		{
			SkillDoubleLinkedList.AddTail(Node->GetValue());
		}
	}
    
	// 拷贝赋值运算符
	FSkillPool& operator=(const FSkillPool& Other)
	{
		if (this != &Other)
		{
			// 清空当前链表
			SkillDoubleLinkedList.Empty();
            
			// 拷贝基础数据
			TotalCount = Other.TotalCount;
			ActiveCount = Other.ActiveCount;
            
			// 拷贝链表内容
			for (auto Node = Other.SkillDoubleLinkedList.GetHead(); Node; Node = Node->GetNextNode())
			{
				SkillDoubleLinkedList.AddTail(Node->GetValue());
			}
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
	void PreallocateSkillPool(TSubclassOf<ABaseSkill> SkillClass, int32 Count);

	// 激活技能（从池中获取或创建新实例）
	UFUNCTION(BlueprintCallable, Category = "Skill System")
	ABaseSkill* ActivateSkill(TSubclassOf<ABaseSkill> SkillClass, const FTransform& SpawnTransform);

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
	void ResetSkill(ABaseSkill* Skill);
	FSkillPool& GetOrCreateSkillPool(TSubclassOf<ABaseSkill> SkillClass);
};
