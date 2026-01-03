// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/BaseTraceProjectileSkill.h"

#include "NiagaraFunctionLibrary.h"
#include "Pixel2DKit.h"
#include "Enemy/BaseEnemy.h"
#include "Utilitys/SpaceFuncLib.h"


// Sets default values
ABaseTraceProjectileSkill::ABaseTraceProjectileSkill()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileComp->ProjectileGravityScale = 0.0f;
	ProjectileComp->bRotationFollowsVelocity = true;
}

// Called when the game starts or when spawned
void ABaseTraceProjectileSkill::BeginPlay()
{
	Super::BeginPlay();

	if (Target)
	{
		SetNewTarget(Target, bIdle);
	}
}

void ABaseTraceProjectileSkill::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (ProjectileComp)
	{
		ProjectileComp->DestroyComponent();
	}
}

// Called every frame
void ABaseTraceProjectileSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bEnding) return;

	if (ProjectileComp)
	{
		FHitResult OutHit;
		bool bTraced = UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation(), GetActorLocation() + ProjectileComp->Velocity,
			TraceObstacleChannel, false, {},
				EDrawDebugTrace::None, OutHit, true,
				FLinearColor::Red, FLinearColor::Green, 1.0f);

		if (bTraced)
		{
			ProjectileComp->Velocity += OutHit.Normal * DeltaTime * 
				FMath::GetMappedRangeValueClamped(FVector2D(25, 1000000), FVector2D(500.0, 1), FMath::Pow(OutHit.Distance, 2));
		}
	}

	
	if (Target)
	{
		float DistanceToTarget = FVector::Distance(Target->GetActorLocation(), GetActorLocation());
		float Magnitude = CurMagnitude * FMath::GetMappedRangeValueClamped(
			FVector2D(InRangeNear, InRangeNear*3), FVector2D(MagnitudeScaleNear, MagnitudeScaleFar), DistanceToTarget);
		if (ProjectileComp)
		{
			ProjectileComp->HomingAccelerationMagnitude = Magnitude;
			if (bIdle && DistanceToTarget < InRangeNear)
			{
				ProjectileComp->Velocity *= 0.99;
			}
		}

		if (!bIdle)
		{
			// 避免贴近依然没有造成伤害，导致一直贴住抖动
			float HitRadius = Target->GetSimpleCollisionRadius() + GetSimpleCollisionRadius();
			if (Target->GetDistanceTo(this) < HitRadius)
			{
				OnHitTarget(Target);
			}
		}
	}
	else
	{
		TryFindNextTarget(Owner);
	}
	
}

void ABaseTraceProjectileSkill::RepelFromActor(AActor* Other)
{
	Super::RepelFromActor(Other);

	if (RepelSameSpeed <= 0.0f) return;

	FVector DirVector = GetActorLocation() - Other->GetActorLocation();
	FVector RepelDirection = DirVector.GetSafeNormal();
	float Distance = DirVector.Size();
	
	ProjectileComp->Velocity += RepelDirection * RepelSameSpeed * 
		FMath::GetMappedRangeValueClamped(FVector2D(25, 1000000), FVector2D(500.0, 1), FMath::Pow(Distance, 2));
}

void ABaseTraceProjectileSkill::SetActive(bool v)
{
	Super::SetActive(v);

	if (v)
	{
		bEnding = false;
		SetNewTarget(Target, bIdle);
	}
	else
	{
		bEnding = true;
		ProjectileComp->bIsHomingProjectile = false;
		ProjectileComp->Velocity = FVector::ZeroVector;
	}
	
}

void ABaseTraceProjectileSkill::OnSkillEnd()
{
	Super::OnSkillEnd();
	if (ProjectileComp)
	{
		ProjectileComp->bIsHomingProjectile = false;
	}

	// 还需要触发Niagara的消失效果
}

void ABaseTraceProjectileSkill::SetVelocity(const FVector& V)
{
	if (bEnding) return;
	
	if (ProjectileComp)
	{
		ProjectileComp->Velocity = V;
	}
}

void ABaseTraceProjectileSkill::TryFindNextTarget(AActor* CenterActor)
{
	if (bEnding) return;

	TArray<AActor*> ActorsToIgnore;
	if (ABaseEnemy* NewEnemy = USpaceFuncLib::FindActorInRangeClosest<ABaseEnemy>(GetWorld(), CenterActor, ActorsToIgnore,FVector2D(0, MaxTraceDistance)))
	{
		SetNewTarget(NewEnemy);
	}
	else
	{
		OnSkillEnd();
	}
}

void ABaseTraceProjectileSkill::SetDamageData(int _Damage, FVector& _Knockback, int _RemHitNum, bool Force, int _DamageDecreasePercentPerHit)
{
	Damage = _Damage;
	Knockback = _Knockback;
	RemHitNum = _RemHitNum;
	bForce = Force;
	DamageDecreasePercentPerHit = _DamageDecreasePercentPerHit;
}

void ABaseTraceProjectileSkill::StartPlusDamageByDistance(int _DamagePlusPer100Meter)
{
	if (_DamagePlusPer100Meter <= 0)
	{
		bPlusDamageByDistance = false;
	}
	else
	{
		bPlusDamageByDistance = true;
		DamagePlusPer100Meter = _DamagePlusPer100Meter;
		DistanceDamageInitLocation = GetActorLocation();
	}
}


void ABaseTraceProjectileSkill::SetTraceData(const FTraceProjectileData& Data)
{
	bIdle = Data.bIdle;
	LifeSpan = Data.NewTargetLifeSpan;
	CurMagnitude = Data.CurMagnitude;
	InRangeNear = Data.InRangeNear;
	MagnitudeScaleNear= Data.MagnitudeScaleNear;
	MagnitudeScaleFar = Data.MagnitudeScaleFar;
	InitSpeed = Data.InitSpeed;
	MaxSpeed = Data.MaxSpeed;
	MaxTraceDistance = Data.MaxTraceDistance;
}

void ABaseTraceProjectileSkill::SetNewTarget(AActor* TargetActor, bool Idle)
{
	if (!TargetActor)
	{
		OnSkillEnd();
		return;
	}

	if (bEnding) return;

	Target = TargetActor;
	bIdle = Idle;

	SetSkillLifeTimer(true);
	
	if (USceneComponent* TargetRootComponent = TargetActor->GetRootComponent())
	{
		CurMagnitude *= FMath::RandRange(0.8, 1.2);
		InRangeNear *= FMath::RandRange(0.8, 1.2);
		MagnitudeScaleNear *= FMath::RandRange(0.8, 1.2);
		MagnitudeScaleFar *= FMath::RandRange(0.8, 1.2);
		
		if (ProjectileComp)
		{
			ProjectileComp->bIsHomingProjectile = true;
			ProjectileComp->HomingAccelerationMagnitude = CurMagnitude;

			// 初始速度设置实际可能无效
			ProjectileComp->InitialSpeed = InitSpeed;
			ProjectileComp->MaxSpeed = MaxSpeed;

			ProjectileComp->HomingTargetComponent = TargetRootComponent;
			ProjectileComp->Velocity = InitSpeed * (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		}
	}

	if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(Target))
	{
		Enemy->OnEnemyDie.AddUniqueDynamic(this, &ThisClass::OnEnemyDie);
	}
}

void ABaseTraceProjectileSkill::OnEnemyDie(ABaseEnemy* Enemy)
{
	Enemy->OnEnemyDie.RemoveDynamic(this, &ThisClass::OnEnemyDie);
	TryFindNextTarget(Enemy);
}

void ABaseTraceProjectileSkill::OnHitTarget_Implementation(AActor* HitTarget)
{
	if (HitTarget != Target) return;

	if (UHealthComponent* HealthComponent = Target->GetComponentByClass<UHealthComponent>())
	{
		int DistanceDamage = bPlusDamageByDistance ? FVector::Distance(DistanceDamageInitLocation, GetActorLocation()) * DamagePlusPer100Meter / 100 : 0;
		HealthComponent->DecreaseHP(Damage + DistanceDamage, Owner, Knockback, bForce);
	}

	Damage *= 1 - DamageDecreasePercentPerHit;
	DamagePlusPer100Meter *= 1 - DamageDecreasePercentPerHit;

	if (--RemHitNum <= 0 || Damage <= 0)
	{
		OnSkillEnd();
	}
	else
	{
		TryFindNextTarget(Target);
	}

	if (HitNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagara, GetActorLocation(), FRotator(0,0,0),  GetActorScale3D());
	}
}
