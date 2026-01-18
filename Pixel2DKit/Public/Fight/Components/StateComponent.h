// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "StateComponent.generated.h"


UENUM(BlueprintType)
enum class EStatChange : uint8
{
	Increase UMETA(DisplayName = "Increase"),
	Decrease UMETA(DisplayName = "Decrease"),
	Reset    UMETA(DisplayName = "Reset")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnHPChangedSignature, 
	int32,                 
	OldValue,               
	int32,                     
	NewValue                   
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnMaxHPChanged,
	int32,
	OldValue,
	int32,
	NewValue
);


UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UStateComponent : public UActorComponent
{
	GENERATED_BODY()

	bool bOwnerIsPlayer;
	
public:

	// 因为掉落地面死亡
	UPROPERTY(BlueprintReadWrite, Category = "StateComponent")
	bool DieByFalling = false;
	
	// 受到伤害后的一段时候才会触发无敌帧，支持短时间内的连击，后续考虑拉长并多段连击，攻击命中后会续时
	float OnHurtInvulnerableDelay = 0.15;  
	float InvulnerableDuration = 0.5; // 无敌帧时间

	bool bFlashing = false; // 是否在受伤闪烁
	
	UPROPERTY(BlueprintReadWrite, Category = "StateComponent")
	float InRockPercent = 0.0f; // 霸体抗性，抵御击退、击飞

	UPROPERTY(BlueprintReadWrite, Category = "StateComponent")
	FVector RepelResistance = FVector(10, 10, 10); // 各个方向的击退抵抗

	UPROPERTY(BlueprintReadWrite, Category = "StateComponent")
	float RepelResistancePercent = 0.f; // 最终结算时的击退削减百分比
	
	float KnockBackMultiplier = 1.f; // 受力缩放
	float MinPowerRepelForceValue = 300.f; // 强效击退触发的阈值
	
	int CalAcceptDamage(int InDamage, AActor* Maker);
	void OnHurtInvulnerable();
	
	
	FVector CalRepel(FVector& IncomeRepel, const AActor* Instigator) const;
	
	UPROPERTY(BlueprintAssignable, Category = "StateComponent|Health")
	FOnHPChangedSignature OnHPChanged;
	
	// Sets default values for this component's properties
	UStateComponent();
	
	UPROPERTY(BlueprintReadOnly)
	bool bInvulnerable = false; // 是否处于无敌帧

	UFUNCTION(BlueprintCallable, Category="StateComponent")
	void InvulnerableForDuration(float duration);

	UFUNCTION(BlueprintCallable, Category="StateComponent")
	void FlashForDuration(float Duration = 0.4f, float FlashRate = 0.1f, FLinearColor FlashColor = FLinearColor(0.9, 0.1, 0.1, 0.6)) ;

	
	UFUNCTION(BlueprintCallable, Category="StateComponent")
	virtual void SetInvulnerable(const bool v);

	UFUNCTION(BlueprintCallable, Category="StateComponent")
	virtual void ModifyMaxHP(int32 value, const EStatChange ChangeType, const bool current);

	UFUNCTION(BlueprintCallable, Category="StateComponent")
	virtual void SetHP(const int32 value, const bool broadcast = true);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void Event_OnHPChanged(int OldValue, int NewValue);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateComponent")
	int GetCurrentHP();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateComponent")
	int GetMaxHP();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateComponent")
	float GetHPPercent();
	
	UFUNCTION(BlueprintCallable, Category="StateComponent")
	virtual void DecreaseHP(int Damage, AActor* Maker, const FVector KnockbackForce = FVector(50), bool bForce = false);

	UFUNCTION(BlueprintCallable, Category="StateComponent")
	virtual void IncreaseHP(int32 value, AActor* Instigator);
	
	UFUNCTION(BlueprintCallable, Category="StateComponent | Movement")
	void KnockBack(FVector Repel, AActor* Maker);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="StateComponent")
	float CalHurtDuration(int32 ChangedHP);
	
	int32 CalEPCustomInFact(int32 Amount);
	
	UFUNCTION(BlueprintCallable, Category="StateComponent")
	bool DecreaseEP(int32 Amount);

	UFUNCTION(BlueprintCallable, Category="StateComponent")
	void IncreaseEP(int32 Amount);

	UFUNCTION(BlueprintCallable, Category="StateComponent")
	void SetEP(int32 NewValue);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="StateComponent")
	int32 GetCurrentEP();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="StateComponent")
	int32 GetMaxEP();

	UFUNCTION(BlueprintCallable, Category="StateComponent")
	void SetMaxEP(int32 NewValue);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="StateComponent")
	bool IsEPEnough(int32 Cost, bool bNeedTip = false);
};
