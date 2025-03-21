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


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIXEL2DKIT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

	int CurrentHealth = 10;
	float OnHurtInvulnerableDelay = 0.1;  // 受到伤害后的一段时候才会触发无敌帧，支持短时间内的连击，后续考虑拉长并多段连击，攻击命中后会续时
	float InvulnerableDuration = 0.5; // 无敌帧时间
	bool bInRock = false; // 是否处于霸体状态
	float KnockBackMultiplier = 1.f; // 受力缩放
	
	int CalAcceptDamage(int initDamage, AActor* instigator);
	void OnHurtInvulnerable();
	void InvulnerableForDuration(float duration);
	void FlashForDuration(FLinearColor color = FLinearColor(0.9, 0.1, 0.1, 0.6), float duration = 1, bool force = false) ;


	
	FVector GetRepel(FVector IncomeRepel, AActor* Instigator);


	
	FOnHealthChangedSignature OnHealthChanged;





	
public:	
	// Sets default values for this component's properties
	UHealthComponent();


	UPROPERTY(BlueprintReadOnly)
	bool bInvulnerable; // 是否处于无敌帧


	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
	int GetCurrentHealth();
	
	UFUNCTION(BlueprintCallable, Category="Health")
	void DecreaseHealth(int Damage, const FVector& KnockbackForce, AActor* Instigator, bool bForce = false, bool bCauseInvul = true, bool bInner = false);

	UFUNCTION(BlueprintCallable, Category="Health")
	void IncreaseHealth();

	UFUNCTION(BlueprintCallable, Category="HealthComponent | Movement")
	void KnockBack(FVector Repel, AActor* Instigator);

	
		
};
