// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Character/BasePixelCharacter.h"
#include "Interfaces/Fight_Interface.h"
#include "Interfaces/Enemy/AI/EnemyAI_Interface.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "BaseEnemy.generated.h"

class UEnemyAIComponent;
class UHealthComponent;
class UFightComponent;
/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API ABaseEnemy : public APaperZDCharacter, public IFight_Interface, public IEnemyAI_Interface
{
	GENERATED_BODY()
	
	UPROPERTY()
	ABasePixelCharacter* PixelCharacter;



	void SetLanding(const bool V, const float time = 0.1f);

	
public:
	ABaseEnemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(BlueprintReadOnly, Category = EnemyAI)
	TObjectPtr<UEnemyAIComponent> EnemyAIComponent;
	
	UPROPERTY(BlueprintReadOnly, Category = Fight)
	TObjectPtr<UHealthComponent> HealthComponent_CPP;
	
	UPROPERTY(BlueprintReadOnly, Category = Fight)
	TObjectPtr<UFightComponent> FightComp;

	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetPixelCharacter();
	UFUNCTION(BlueprintCallable, Category="Enemy")
	bool SetPixelCharacter(AActor* Character);

	// 基础攻击击退力度
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyAI")
	FVector BasicAttackRepel = FVector(50,50,100); 

	// 基础攻击击退力度
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyAI")
	FVector CurAttackRepel = FVector(50,50,100); 

	// 巡逻时随机移动的范围, 不应该超过平台的间隔导致空间位置不足，从而强行寻路掉落平台
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyAI")
	float RandomMoveRange = 100.0f; 
	
	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bDead; 

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bHurt;
	
	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bInAttackState;
	
	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bInAttackEffect;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bAttackAnimToggle;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bInDefendState;
	
	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bDefendStart;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bDefendHurt;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bJumping;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bLanding;

	virtual void Jump() override;
	virtual void Landed(const FHitResult& Hit) override;

	
	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetDead(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetHurt(const bool V, const float duration);
	
	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetInAttackState(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetInAttackEffect(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetAttackAnimToggle(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetInDefendState(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetDefendStart(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetDefendHurt(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetJumping(const bool V, const float time = 0.2f);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Enemy | Fight")
	float GetDistanceToPlayer() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
	void TryAttack();

	
	virtual bool GetIsAttacking() override;
	virtual bool IsAlive_Implementation() override;
	virtual AActor* GetTarget_Implementation() override;
	virtual float GetRandomMoveRange_Implementation() override;
	virtual bool CanMove_Implementation() override;
	virtual bool CanAttack_Implementation() override;
	virtual bool Dash_Implementation() override;
	virtual void OnBeAttacked_Invulnerable_Implementation() override;
	virtual int DamagePlus_Implementation(int inValue, AActor* ActorDamaged) override;
	virtual int OnDefendingHit_Implementation(int iniDamage) override;
	
	// 战斗的行动模式
	virtual void ActionAtPlayerEastNear_Implementation(float Distance) override;
	virtual void ActionAtPlayerEastMid_Implementation(float Distance) override;
	virtual void ActionAtPlayerEastFar_Implementation(float Distance) override;
	virtual void ActionAtPlayerEastRemote_Implementation(float Distance) override;
	virtual void ActionAtPlayerWestNear_Implementation(float Distance) override;
	virtual void ActionAtPlayerWestMid_Implementation(float Distance) override;
	virtual void ActionAtPlayerWestFar_Implementation(float Distance) override;
	virtual void ActionAtPlayerWestRemote_Implementation(float Distance) override;
	virtual void ActionAtPlayerNorthNear_Implementation(float Distance) override;
	virtual void ActionAtPlayerNorthMid_Implementation(float Distance) override;
	virtual void ActionAtPlayerNorthFar_Implementation(float Distance) override;
	virtual void ActionAtPlayerNorthRemote_Implementation(float Distance) override;
	virtual void ActionAtPlayerSouthNear_Implementation(float Distance) override;
	virtual void ActionAtPlayerSouthMid_Implementation(float Distance) override;
	virtual void ActionAtPlayerSouthFar_Implementation(float Distance) override;
	virtual void ActionAtPlayerSouthRemote_Implementation(float Distance) override;

	// 战斗距离判断, 当前的距离是否适合当前区间的攻击
	virtual bool InAttackRange() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fight")
	FGameplayTagContainer ActionFieldsCanAttack;
	
	
protected:
	void Tick_KeepFaceToPixelCharacter(float DeltaSeconds);

	virtual void Tick(float DeltaSeconds) override;

	
};

