// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BaseTraceProjectileSkill.generated.h"

class ABaseEnemy;




USTRUCT(BlueprintType)
struct FTraceProjectileData
{
	GENERATED_BODY()

	// 是否处于待机状态
	UPROPERTY(BlueprintReadWrite, Category="Skill | Projectile", meta = (ExposeOnSpawn))
	bool bIdle = true;

	// 找到新目标时刷新的技能存活时间
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn, AllowPrivateAccess=true))
	float NewTargetLifeSpan = 2.0f;
	
	UPROPERTY(BlueprintReadWrite, Category="Skill | Projectile", meta = (ExposeOnSpawn))
	float CurMagnitude = 3000.0f;

	// 与目标低于此距离，促发近距离效果
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn, AllowPrivateAccess = true))
	float InRangeNear = 1000.0f;
	
	UPROPERTY(BlueprintReadWrite, Category="Skill | Projectile", meta = (ExposeOnSpawn))
	float MagnitudeScaleNear = 2.0f;
	
	UPROPERTY(BlueprintReadWrite, Category="Skill | Projectile", meta = (ExposeOnSpawn))
	float MagnitudeScaleFar = 1.0f;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn, AllowPrivateAccess=true))
	float InitSpeed = 500.0f;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn, AllowPrivateAccess=true))
	float MaxSpeed = 1000.0f;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn, AllowPrivateAccess=true))
	float MaxTraceDistance = 1000.0f;
	
};



// 追踪弹（法术），命中目标后可以寻找下一个目标，直至消耗完所有命中机会，可以在追踪图中转换目标
// 如果在命中目标后，找不到下一个目标就会消失
UCLASS()
class PIXEL2DKIT_API ABaseTraceProjectileSkill : public ABaseSkill
{
	GENERATED_BODY()

	bool bPlusDamageByDistance = false;
	int DamagePlusPer100Meter = 0;
	FVector DistanceDamageInitLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skill | Projectile", meta = (AllowPrivateAccess = true))
	TEnumAsByte<ETraceTypeQuery> TraceObstacleChannel = TraceTypeQuery5;

	
public:
	// Sets default values for this actor's properties
	ABaseTraceProjectileSkill();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Skill | Projectile")
	UProjectileMovementComponent* ProjectileComp = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn, AllowPrivateAccess=true))
	UNiagaraSystem* HitNiagara;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	virtual void RepelFromActor(AActor* Other) override;

	
	virtual void SetActive(bool v) override;
	
	UPROPERTY(BlueprintReadOnly, Category="Skill | Projectile", meta = (ExposeOnSpawn))
	AActor* Target = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category="Skill | Projectile", meta = (ExposeOnSpawn))
	bool bIdle = false;
	UPROPERTY(BlueprintReadOnly, Category="Skill | Projectile", meta = (ExposeOnSpawn))
	float CurMagnitude = 3000.0f;
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn))
	float InRangeNear = 1000.0f;
	UPROPERTY(BlueprintReadOnly, Category="Skill | Projectile", meta = (ExposeOnSpawn))
	float MagnitudeScaleNear = 2.0f;
	UPROPERTY(BlueprintReadOnly, Category="Skill | Projectile", meta = (ExposeOnSpawn))
	float MagnitudeScaleFar = 1.0f;
	UPROPERTY(BlueprintReadWrite, Category="Skill | Projectile",meta = (ExposeOnSpawn))
	float InitSpeed = 500.0f;
	UPROPERTY(BlueprintReadWrite, Category="Skill | Projectile",meta = (ExposeOnSpawn))
	float MaxSpeed = 1000.0f;
	UPROPERTY(BlueprintReadWrite, Category="Skill | Projectile",meta = (ExposeOnSpawn))
	float MaxTraceDistance = 600.0f;
	
	UFUNCTION(BlueprintCallable, Category="Skill | Projectile")
	void SetTraceData(const FTraceProjectileData& Data);


	
	UPROPERTY(BlueprintReadWrite, Category="Skill", meta = (ExposeOnSpawn))
	int Damage = 1;
	UPROPERTY(BlueprintReadWrite, Category="Skill", meta = (ExposeOnSpawn))
	int RemHitNum = 1;	
	UPROPERTY(BlueprintReadWrite, Category="Skill", meta = (ExposeOnSpawn))
	bool bForce = false;
	UPROPERTY(BlueprintReadWrite, Category="Skill", meta = (ExposeOnSpawn))
	float DamageDecreasePercentPerHit = 0.1;
	UPROPERTY(BlueprintReadWrite, Category="Skill", meta = (ExposeOnSpawn))
	FVector Knockback = FVector(50, 50, 100);
	
	void SetDamageData(int _Damage, FVector& _Knockback, int _RemHitNum = 1, bool Force = false, int _DamageDecreasePercentPerHit = 0.1);


	
	UFUNCTION(BlueprintCallable, Category="Skill")
	void StartPlusDamageByDistance(int _DamagePlusPer100Meter);

	
	UFUNCTION(BlueprintCallable, Category="Skill | Projectile")
	void SetVelocity(const FVector& V);
		
	UFUNCTION(BlueprintCallable, Category="Skill | Projectile")
	void TryFindNextTarget(AActor* CenterActor);


	
	UFUNCTION(BlueprintCallable)
	void SetNewTarget(AActor* TargetActor, bool Idle = false);
	
	UFUNCTION()
	void OnEnemyDie(ABaseEnemy* Enemy);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnHitTarget(AActor* HitTarget);

	virtual void OnSkillEnd() override;




	
};
