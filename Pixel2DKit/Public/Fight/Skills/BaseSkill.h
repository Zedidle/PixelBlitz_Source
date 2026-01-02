// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "UObject/FastReferenceCollector.h"
#include "Utilitys/DebugFuncLab.h"
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
	

	
protected:
	virtual void BeginPlay() override;

	// 用于判断技能在自行消失生命或伤害结算后将要进入 Waiting状态 倒计时（一般是0.5秒）触发消失特效的阶段
	UPROPERTY(BlueprintReadWrite)
	bool bEnding = false;
	
public:

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
	void BP_OnSkillEnd();
	// 静止所有 碰撞/伤害 生效，触发Niagara消除效果
	UFUNCTION(BlueprintCallable)
	virtual void OnSkillEnd();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Skill | Niagara")
	void SetColor(FLinearColor Color);

	UFUNCTION(BlueprintCallable, Category="Skill | Niagara")
	void ReinitializeNiagara();
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
