// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "BasePixelCharacter.h"
#include "Interfaces/Fight_Interface.h"
#include "Interfaces/Enemy/AI/EnemyAI_Interface.h"
#include "BaseEnemy.generated.h"

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
	ABaseEnemy();
	
	UPROPERTY(BlueprintReadOnly, Category = Fight)
	TObjectPtr<UHealthComponent> HealthComponent_CPP;
	
	UPROPERTY(BlueprintReadOnly, Category = Fight)
	TObjectPtr<UFightComponent> FightComp;

	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetPixelCharacter();
	UFUNCTION(BlueprintCallable, Category="Enemy")
	bool SetPixelCharacter(AActor* Character);


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyAI")
	FVector BasicAttackRepel = FVector(50,50,100); // 基础攻击击退力度
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyAI")
	FVector CurAttackRepel = FVector(50,50,100); // 基础攻击击退力度
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "EnemyAI")
	float RandomMoveRange = 200.0f; // 巡逻时随机移动的范围
	
	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bDead; 

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bHurt;
	
	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bInAttackState;
	
	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bInAttackEffect;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bAttackStartX;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy | Fight")
	bool bAttackStartY;

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
	
	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetDead(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetHurt(const bool V, const float duration);
	
	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetInAttackState(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetInAttackEffect(const bool V);
	
	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetAttackStartX(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetAttackStartY(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetInDefendState(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetDefendStart(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetDefendHurt(const bool V);

	UFUNCTION(BlueprintCallable, Category = "Enemy | Fight")
	void SetJumping(const bool V, const float time = 0.2f);

	
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category="Fight_Interface")
	bool GetIsAttacking();
	virtual bool GetIsAttacking_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category="Fight_Interface")
	bool IsAlive();
	virtual bool IsAlive_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category="Fight_Interface")
	AActor* GetTarget();
	virtual AActor* GetTarget_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category="EnemyAI_Interface")
	float GetRandomMoveRange();
	virtual float GetRandomMoveRange_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category="EnemyAI_Interface")
	bool InAtttckRange_EnemyAI();
	virtual bool InAtttckRange_EnemyAI_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void OnReachedAttackLocation_EnemyAI();
	virtual void OnReachedAttackLocation_EnemyAI_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category="EnemyAI_Interface")
	bool CanMove_EnemyAI();
	virtual bool CanMove_EnemyAI_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure, Category="EnemyAI_Interface")
	bool Dash_EnemyAI();
	virtual bool Dash_EnemyAI_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void OnReachedEnemyX_EnemyAI();
	virtual void OnReachedEnemyX_EnemyAI_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="EnemyAI_Interface")
	void OnReachedEnemyY_EnemyAI();
	virtual void OnReachedEnemyY_EnemyAI_Implementation() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
	void OnBeAttacked_Invulnerable();
	virtual void OnBeAttacked_Invulnerable_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
	int DamagePlus(int inValue, AActor* ActorDamaged);
	virtual int DamagePlus_Implementation(int inValue, AActor* ActorDamaged) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Fight_Interface")
	int OnDefendingHit(int iniDamage);
	virtual int OnDefendingHit_Implementation(int iniDamage) override;

	
	virtual void Jump() override;
	virtual void Landed(const FHitResult& Hit) override;

	
protected:
	void Tick_KeepFaceToPixelCharacter(float DeltaSeconds);

	virtual void Tick(float DeltaSeconds) override;
	

	
};
