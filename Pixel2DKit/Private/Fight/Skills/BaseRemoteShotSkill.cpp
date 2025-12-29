// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/BaseRemoteShotSkill.h"

#include "NiagaraFunctionLibrary.h"
#include "Enemy/BaseEnemy.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Utilitys/DebugFuncLab.h"
#include "Utilitys/SpaceFuncLib.h"


// Sets default values
ABaseRemoteShotSkill::ABaseRemoteShotSkill()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileComp->ProjectileGravityScale = 0.01f;
	ProjectileComp->bRotationFollowsVelocity = true;
	
}

void ABaseRemoteShotSkill::SetTraceData(const FRemoteShotData& Data)
{
	LifeSpan = Data.NewTargetLifeSpan;
	InitSpeed = Data.InitSpeed;
	MaxSpeed = Data.MaxSpeed;
	MaxTraceDistance = Data.MaxTraceDistance;
	Direction = Data.Direction;
}

void ABaseRemoteShotSkill::SetDamageData(int _Damage, FVector _Knockback, int _RemHitNum, 
	int _DamageDecreasePercentPerHit, int _RemSpringNum, int _RemSplitNum)
{
	Damage = _Damage;
	Knockback = _Knockback;
	RemHitNum = _RemHitNum;
	DamageDecreasePercentPerHit = _DamageDecreasePercentPerHit;
	RemSpringNum = _RemSpringNum;
	RemSplitNum = _RemSplitNum;
}

// Called when the game starts or when spawned
void ABaseRemoteShotSkill::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnHitTarget);
	OnActorHit.AddDynamic(this, &ThisClass::OnHitObstacle);
}

void ABaseRemoteShotSkill::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::OnHitTarget);
	OnActorHit.RemoveDynamic(this, &ThisClass::OnHitObstacle);
}


// Called every frame
void ABaseRemoteShotSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseRemoteShotSkill::SetActive(bool v)
{
	Super::SetActive(v);
	if (v)
	{
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ProjectileComp)
	
		ProjectileComp->InitialSpeed = InitSpeed;
		ProjectileComp->MaxSpeed = MaxSpeed;
		ProjectileComp->Velocity = Direction * InitSpeed;
		ActorsEffected.Empty();
	}
}

FVector ABaseRemoteShotSkill::FindNextTargetDirection()
{
	ABaseEnemy* Enemy = USpaceFuncLib::FindActorInRangeRandomOne<ABaseEnemy>(GetWorld(), this, ActorsEffected, FVector2D(0, MaxTraceDistance));
	if (Enemy)
	{
		SetSkillLifeTimer(true);
		
		return (Enemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	}

	if (ProjectileComp)
	{
		return ProjectileComp->Velocity.GetSafeNormal();
	}
	
	return FVector::ZeroVector;
}

void ABaseRemoteShotSkill::OnHitObstacle_Implementation(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse,
	const FHitResult& Hit)
{
	ReinitializeNiagara();
	RemHitNum -- ;
}


void ABaseRemoteShotSkill::OnHitTarget_Implementation(AActor* OverlappedActor, AActor* HitTarget)
{
	if (ActorsEffected.Contains(HitTarget)) return;
	if (!Owner || HitTarget == Owner) return;
	
	if (!CanDamageEffect(HitTarget))
	{
		if (HitTarget->ActorHasTag("Obstacle"))
		{
			if (HitNiagara)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagara, GetActorLocation(), FRotator(0,0,0),  GetActorScale3D());
			}

			// 由于目前没做大部分的技能消失效果，先直接Destroy
			// Destroy();

			// OnSkillEnd();
			
			SetActive(false);
		}
		return;
	}


	if (UHealthComponent* HealthComponent = HitTarget->GetComponentByClass<UHealthComponent>())
	{
		HealthComponent->DecreaseHP(Damage, Owner, Knockback, false);
		IFight_Interface::Execute_OnAttackHiting(Owner);
		ActorsEffected.Add(HitTarget);
	}

	Damage *= 1 - DamageDecreasePercentPerHit;

	if (--RemHitNum <= 0 || Damage <= 0)
	{
		OnSkillEnd();
	}
	else
	{
		if (OnSplit() | OnSpring())
		{
			AddActorWorldOffset(ProjectileComp->Velocity.GetSafeNormal() * 20);
			ReinitializeNiagara();
		}
	}
	
	if (HitNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitNiagara, GetActorLocation(), FRotator(0,0,0),  GetActorScale3D());
	}
}

bool ABaseRemoteShotSkill::OnSplit()
{
	// 分裂
	if (RemSplitNum <= 0) return false;
	
	RemSplitNum --;
	float TmpSplitRotation = (FMath::RandBool() ? 1 : -1 ) * FMath::FRandRange(25.0f, 35.0f);

	UClass* SelfClass = GetClass();
	if (ProjectileComp)
	{
		FVector CurVelocity = ProjectileComp->Velocity;
		FVector NewDir = FRotator(0, TmpSplitRotation, 0).RotateVector(CurVelocity).GetSafeNormal();
		FVector NewLocation = GetActorLocation() + NewDir * 20;
		FVector NewScale = GetActorScale3D() * 0.9;

		UDebugFuncLab::ScreenMessage(*NewScale.ToString());
			
		FTransform NewTrans(FRotator(0,0,0), NewLocation , NewScale);
			
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Owner;
		SpawnParams.TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;
			
		if (ABaseRemoteShotSkill* NewSkill = GetWorld()->SpawnActorDeferred<ABaseRemoteShotSkill>(SelfClass, NewTrans, Owner, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
		{
			FRemoteShotData Data;
			Data.InitSpeed = InitSpeed * 0.9;
			Data.MaxSpeed = MaxSpeed * 0.9;
			Data.NewTargetLifeSpan = LifeSpan * 0.9;
			Data.MaxTraceDistance = MaxTraceDistance * 0.9;
			Data.Direction = NewDir;
				
			NewSkill->SetTraceData(Data);
			NewSkill->SetDamageData(Damage * 0.6, Knockback * 0.8, RemHitNum, DamageDecreasePercentPerHit, RemSplitNum, RemSplitNum);

			UGameplayStatics::FinishSpawningActor(NewSkill, NewTrans);
			NewSkill->SetActive(true);
		}

		ProjectileComp->Velocity =  FRotator(0, -TmpSplitRotation, 0).RotateVector(ProjectileComp->Velocity);
	}
	return true;
}

bool ABaseRemoteShotSkill::OnSpring()
{
	// 弹射
	if (RemSpringNum <= 0) return false;
	
	RemSpringNum --;
	if (ProjectileComp)
	{
		FVector NewDir = FindNextTargetDirection();
		ProjectileComp->Velocity = ProjectileComp->Velocity.Size() * NewDir;
	}

	return true;
}

