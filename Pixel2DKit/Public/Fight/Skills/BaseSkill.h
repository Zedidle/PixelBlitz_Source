// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "UObject/FastReferenceCollector.h"
#include "Utilitys/DebugFuncLab.h"
#include "Utilitys/PXCustomStruct.h"
#include "BaseSkill.generated.h"


/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API ABaseSkill : public AActor
{
	GENERATED_BODY()


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	USoundBase* BeginSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bActive = false;
	
	
	// 技能触发时机，长期挂载才有用
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	EAbilityTiming ActivateTiming = EAbilityTiming::None;
	
protected:
	virtual void BeginPlay() override;

	// 用于判断技能在自行消失生命或伤害结算后将要进入 Waiting状态 倒计时（一般是0.5秒）触发消失特效的阶段
	UPROPERTY(BlueprintReadWrite)
	bool bEnding = false;
	
public:

	// 技能串联激活的优先级
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	int Priority = 1;
	
	// 处于不攻击且不能被定时回收的状态
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn))
	bool bIdle = false;

	// 是否无视对方受伤的闪烁状态，必定造成伤害
	UPROPERTY(BlueprintReadWrite, Category="Skill", meta = (ExposeOnSpawn))
	bool bForce = false;
	
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn))
	float LifeSpan = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn, ClampMin=0.0, ClampMax=1.0))
	float RepelSameSpeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn))
	float RepelSameDistance = 50;

	UFUNCTION(BlueprintCallable)
	virtual void RepelFromActor(AActor* Other);
	
	UFUNCTION(BlueprintCallable)
	virtual void SetActive(bool v);

	virtual bool IsAvailableForReuse() const 
	{
		bool bValidLowLevel = IsValidLowLevel();
		bool bKilling = IsPendingKillPending();

		return !bActive && bValidLowLevel && !bKilling; 
	}
	
	UFUNCTION(BlueprintCallable)
	void SetSkillLifeTimer(bool bActivate);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsActive() const;
	
	// 一般来说，每个技能都会有对应的Tag
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Skill")
	FGameplayTagContainer AbilityTags;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category="Skill")
	bool CanDamageEffect(AActor* Actor); 

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnSkillEnd")
	bool BP_OnSkillEnd();
	// 静止所有 碰撞/伤害 生效，触发Niagara消除效果
	UFUNCTION(BlueprintCallable)
	virtual void OnSkillEnd();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Skill | Niagara")
	void SetColor(FLinearColor Color);

	UFUNCTION(BlueprintCallable, Category="Skill | Niagara")
	void ReinitializeNiagara();

	UFUNCTION(BlueprintCallable, Category="Skill | Niagara")
	void SetActivateTiming(EAbilityTiming Timing);

	// 由于触发的参数不同，无法笼统使用一个Activate方法
	// UFUNCTION(BlueprintImplementableEvent, DisplayName="ActivateByTiming")
	// void BP_ActivateByTiming();
	// UFUNCTION(BlueprintCallable, Category="Skill | Timing")
	// virtual bool ActivateByTiming(EAbilityTiming Timing);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnAttackStart")
	bool BP_OnAttackStart();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnAttackStart();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnAttackHit")
	bool BP_OnAttackHit();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnAttackHit();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnAttackFinish")
	bool BP_OnAttackFinish();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnAttackFinish();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnAttackSkill")
	bool BP_OnAttackSkill();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnAttackSkill();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnSkillStart")
	bool BP_OnSkillStart();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnSkillStart();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnSkillHit")
	bool BP_OnSkillHit();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnSkillHit();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnSkillFinish")
	bool BP_OnSkillFinish();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnSkillFinish();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnKillEnemy")
	bool BP_OnKillEnemy();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnKillEnemy();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnBeAttacked")
	bool BP_OnBeAttacked(AActor* Maker, int InDamage, int& OutDamage, bool& Stop);
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnBeAttacked(AActor* Maker, int InDamage, int& OutDamage, bool& Stop);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnBeAttackedInvulnerable")
	bool BP_OnBeAttackedInvulnerable();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnBeAttackedInvulnerable();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnBeDamaged")
	bool BP_OnBeDamaged();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnBeDamaged();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnJumpStart")
	bool BP_OnJumpStart();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnJumpStart();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnLanding")
	bool BP_OnLanding();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnLanding();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnDefenseStart")
	bool BP_OnDefenseStart();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnDefenseStart();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnDefenseSuccess")
	bool BP_OnDefenseSuccess();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnDefenseSuccess();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnDefenseFinish")
	bool BP_OnDefenseFinish();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnDefenseFinish();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnDying")
	bool BP_OnDying();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnDying();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnPickGold")
	bool BP_OnPickGold();
	UFUNCTION(Category="Skill | Timing")
	virtual bool OnPickGold();
};

inline void ABaseSkill::SetColor_Implementation(FLinearColor Color)
{
	TArray<FName> ColorNames = { "LinearColor", "Color"};
	
	if (UNiagaraComponent* Niagara = GetComponentByClass<UNiagaraComponent>())
	{
		for (auto& ColorName : ColorNames)
		{
			Niagara->SetVariableLinearColor(ColorName, Color);
		}
	}
}

inline void ABaseSkill::ReinitializeNiagara()
{
	if (UNiagaraComponent* Niagara = GetComponentByClass<UNiagaraComponent>())
	{
		Niagara->ReinitializeSystem();
	}
}
