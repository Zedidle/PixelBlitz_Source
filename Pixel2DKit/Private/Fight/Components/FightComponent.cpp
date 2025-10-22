// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Components/FightComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "Fight/Components/HealthComponent.h"
#include "Interfaces/Fight_Interface.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Utilitys/DebugFuncLab.h"


UFightComponent::UFightComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
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
	
	if (IFight_Interface* FightOwner = Cast<IFight_Interface>(Owner))
	{
		if (!FightOwner->GetIsAttacking())
		{
			SetComponentTickEnabled(false);
			return;
		}
		
		for (auto& name : SocketNames)
		{
			FVector CurLocation = CurSceneComponent->GetSocketLocation(name);

			TArray<FHitResult> OutHits;
			UKismetSystemLibrary::SphereTraceMulti(World, PreLocation, CurLocation, MeleeAttackRadius,
				TraceTypeQuery2, false, MeleeAttackActorsIgnore,
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
					// UDebugFuncLab::ScreenMessage(FString::Printf(TEXT("FightComponent.Tick Hit: %s"), *HitActor->GetName()));
					HealthComponent->DecreaseHP(CurMeleeDamage, CurKnockbackForceMelee, Owner);
					IFight_Interface::Execute_OnAttackHiting(Owner);
				}
			}
			PreLocation = CurLocation;
		}
	}
	else
	{
		SetComponentTickEnabled(false);
	}
	
	
}

void UFightComponent::MeleeTraceAttack(FName SocketName, USceneComponent* SceneComponent, int Damage,
	const FVector& KnockBackForce, UNiagaraSystem* HitEffect)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SceneComponent)
	
	CurKnockbackForceMelee = KnockBackForce;
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

