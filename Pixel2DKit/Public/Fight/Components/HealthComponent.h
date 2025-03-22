// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UENUM(BlueprintType)
enum class EStatChange : uint8
{
	Increase UMETA(DisplayName = "Increase"),
	Decrease UMETA(DisplayName = "Decrease"),
	Reset    UMETA(DisplayName = "Reset")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(
	FOnHealthChangedSignature,      // 委託類型名稱
	int32,                          // NewValue 參數類型
	NewValue,                       // NewValue 參數名稱
	int32,                          // ChangedValue 參數類型
	ChangedValue,                   // ChangedValue 參數名稱
	EStatChange,                    // Change 參數類型
	Change,                         // Change 參數名稱
	AActor*,                        // Instigator 參數類型
	Instigator,                     // Instigator 參數名稱
	bool,                           // bInner 參數類型
	bInner                          // bInner 參數名稱
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FMaxHealthChanged,
	int32,
	NewValue,
	EStatChange,
	Change
);


// UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
UCLASS(MinimalAPI)
class UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

	int MaxHealth = 100;
	int CurrentHealth = 100;
	float OnHurtInvulnerableDelay = 0.1;  // 受到伤害后的一段时候才会触发无敌帧，支持短时间内的连击，后续考虑拉长并多段连击，攻击命中后会续时
	float InvulnerableDuration = 0.5; // 无敌帧时间

	float PreHurtTime = 0; // 上次受到伤害的时间
	bool bFlashing = false; // 是否在受伤闪烁
	FLinearColor FlashColor = FLinearColor::Red; // 闪烁颜色
	float FlashDuration = 1;
	
	UPROPERTY(BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
	bool bInRock = false; // 是否处于霸体状态

	UPROPERTY(BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
	FVector RepelResistance = FVector(10, 10, 10); // 各个方向的击退抵抗

	UPROPERTY(BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float RepelResistancePercent = 0.f; // 最终结算时的击退削减百分比
	
	float KnockBackMultiplier = 1.f; // 受力缩放
	float MinPowerRepelForceValue = 300.f; // 强效击退触发的阈值

	TMap<FGameplayTag, int> Tag2EffectHealthPoint; // 每秒的生命值影响
	FTimerHandle TimerHandle_EffectBySeconds;

	
	int CalAcceptDamage(int initDamage, AActor* instigator);
	void OnHurtInvulnerable();
	
	
	FVector GetRepel(FVector IncomeRepel, AActor* Instigator);
	
	UPROPERTY(BlueprintAssignable, Category = "Components|Health")
	FOnHealthChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Components|Health")
	FMaxHealthChanged OnMaxHealthChanged;

	


	
public:	
	// Sets default values for this component's properties
	UHealthComponent();

	UPROPERTY(BlueprintReadOnly)
	bool bInvulnerable; // 是否处于无敌帧

	UFUNCTION(BlueprintCallable, Category="Health")
	void InvulnerableForDuration(float duration);

	UFUNCTION(BlueprintCallable, Category="Health")
	void FlashForDuration(FLinearColor color = FLinearColor(0.9, 0.1, 0.1, 0.6), float duration = 1, bool force = false) ;

	
	UFUNCTION(BlueprintCallable, Category="Health")
	virtual void SetInvulnerable(const bool v);

	UFUNCTION(BlueprintCallable, Category="Health")
	virtual void ModifyMaxHealth(const int32 value, const EStatChange ChangeType, const bool current);

	UFUNCTION(BlueprintCallable, Category="Health")
	virtual void SetHealth(const int32 value, const bool broadcast = true);

	UFUNCTION(BlueprintCallable, Category="Health")
	virtual void SetEffectBySeconds(const int32 value, const FGameplayTag Tag);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
	int GetCurrentHealth();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
	int GetMaxHealth();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
	float GetHealthPercent();
	
	UFUNCTION(BlueprintCallable, Category="Health")
	virtual void DecreaseHealth(int Damage, const FVector KnockbackForce, AActor* Instigator, bool bForce = false, bool bCauseInvul = true, bool bInner = false);

	UFUNCTION(BlueprintCallable, Category="Health")
	virtual void IncreaseHealth(const int value, AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category="Health")
	void SetActivateHealthEffectBySeconds(const bool activate = true); 
	
	UFUNCTION(BlueprintCallable, Category="HealthComponent | Movement")
	void KnockBack(FVector Repel, AActor* Instigator);

	
		
};
