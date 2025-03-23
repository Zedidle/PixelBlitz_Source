// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Components/FightComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "Fight/Components/HealthComponent.h"
#include "Interfaces/Fight_Interface.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UFightComponent::UFightComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UFightComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() != nullptr && GetOwner()->Implements<UFight_Interface>())
	{
		EnemyTags = IFight_Interface::Execute_GetEnemyCamp(GetOwner());
	}
	
	SetComponentTickEnabled(false);
}


// 目前仅仅用作近战攻击的碰撞生成
void UFightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AActor* Owner = GetOwner();
	if (!Owner) return;
	UWorld* World = GetWorld();
	if (!IsValid(World)) return;
	
	if (Owner->Implements<UFight_Interface>() && IFight_Interface::Execute_GetIsAttacking(Owner))
	{
		for (auto& name : SocketNames)
		{
			FVector end = CurSceneComponent->GetSocketLocation(name);

			TArray<FHitResult> OutHits;
			UKismetSystemLibrary::SphereTraceMulti(World, PreLocation, end, MeleeAttackRadius,
				ETraceTypeQuery::TraceTypeQuery2, false, MeleeAttackActorsIgnore,
				EDrawDebugTrace::None, OutHits, true, FLinearColor::Red,
				FLinearColor::Green, 1);
			if (OutHits.IsEmpty()) return;
			for (auto& hit : OutHits)
			{
				AActor* HitActor = hit.GetActor();
				if (!HitActor) continue;
				MeleeAttackActorsIgnore.AddUnique(HitActor);
				
				if (!HitActor->Implements<UFight_Interface>()) continue;					
				if (!IFight_Interface::Execute_GetOwnCamp(HitActor).HasAny(EnemyTags)) continue;

				if (UHealthComponent* HealthComponent = HitActor->GetComponentByClass<UHealthComponent>())
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, CurHitEffect, hit.ImpactPoint);
					HealthComponent->DecreaseHealth(CurMeleeDamage, CurKnockbackForceMelee, Owner);
					IFight_Interface::Execute_OnAttackHiting(Owner);
				}
			}
		}
	}
	else
	{
		SetComponentTickEnabled(false);
	}
	
	
}

void UFightComponent::MeleeTraceAttack(FName SocketName, USceneComponent* SceneComponent, int Damage,
	const FVector& KnockbackForce, UNiagaraSystem* HitEffect)
{
	CurKnockbackForceMelee = KnockbackForce;
	CurSceneComponent = SceneComponent;
	CurMeleeDamage = Damage;
	MeleeAttackActorsIgnore.Empty();
	SocketNames.Empty();
	CurHitEffect = HitEffect;
	PreLocation = GetOwner()->GetActorLocation();

	for (auto& name : CurSceneComponent->GetAllSocketNames())
	{
		if (UKismetStringLibrary::Contains(name.ToString(), SocketName.ToString()))
		{
			SocketNames.AddUnique(name);
		}
	}

	SetComponentTickEnabled(true);
}

