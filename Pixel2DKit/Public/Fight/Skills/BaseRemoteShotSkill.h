// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"
#include "BaseRemoteShotSkill.generated.h"

class UProjectileMovementComponent;





USTRUCT(BlueprintType)
struct FRemoteShotData
{
	GENERATED_BODY()

	// 找到新目标时刷新的技能存活时间
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn, AllowPrivateAccess=true))
	float NewTargetLifeSpan = 2.0f;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn, AllowPrivateAccess=true))
	float InitSpeed = 500.0f;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn, AllowPrivateAccess=true))
	float MaxSpeed = 1000.0f;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn, AllowPrivateAccess=true))
	float MaxTraceDistance = 1000.0f;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn, AllowPrivateAccess=true))
	FVector Direction = FVector(1,0,0);
	
};



UCLASS()
class PIXEL2DKIT_API ABaseRemoteShotSkill : public ABaseSkill
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseRemoteShotSkill();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Skill | Projectile")
	UProjectileMovementComponent* ProjectileComp = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn, AllowPrivateAccess=true))
	UNiagaraSystem* HitNiagara;
	
	
	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> ActorsEffected;


	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn))
	float NewTargetLifeSpan = 2.0f;
	UPROPERTY(BlueprintReadWrite, Category="Skill | Projectile",meta = (ExposeOnSpawn))
	float InitSpeed = 500.0f;
	UPROPERTY(BlueprintReadWrite, Category="Skill | Projectile",meta = (ExposeOnSpawn))
	float MaxSpeed = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn))
	float MaxTraceDistance = 400;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn))
	FVector Direction = FVector(1,0,0);
	
	UFUNCTION(BlueprintCallable, Category="Skill | Projectile")
	void SetTraceData(const FRemoteShotData& Data);

	
	UPROPERTY(BlueprintReadWrite, Category="Skill", meta = (ExposeOnSpawn))
	int Damage = 0;
	UPROPERTY(BlueprintReadWrite, Category="Skill", meta = (ExposeOnSpawn))
	int RemHitNum = 1;	
	UPROPERTY(BlueprintReadWrite, Category="Skill", meta = (ExposeOnSpawn))
	float DamageDecreasePercentPerHit = 0.1;
	UPROPERTY(BlueprintReadWrite, Category="Skill", meta = (ExposeOnSpawn))
	FVector Knockback = FVector(50, 50, 100);

	// 剩余的弹射追踪次数
	UPROPERTY(BlueprintReadWrite, Category="Skill", meta = (ExposeOnSpawn))
	int RemSpringNum = 0;
	
	// 剩余的分裂次数
	UPROPERTY(BlueprintReadWrite, Category="Skill", meta = (ExposeOnSpawn))
	int RemSplitNum = 0;	

	
	void SetDamageData(int _Damage, FVector _Knockback, int _RemHitNum = 1, int _DamageDecreasePercentPerHit = 0.1, int _RemSpringNum = 0, int _RemSplitNum = 0);
	
	void StartTrace();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;










	

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector FindNextTargetDirection();

	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnHitTarget(AActor* HitTarget);

	
};
