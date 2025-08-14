// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraSystem.h"
#include "Components/ActorComponent.h"
#include "FightComponent.generated.h"


UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIXEL2DKIT_API UFightComponent : public UActorComponent
{
	GENERATED_BODY()

	TArray<FName> SocketNames;

	UPROPERTY()
	USceneComponent* CurSceneComponent;

	UPROPERTY()
	TArray<AActor*> MeleeAttackActorsIgnore;

	UPROPERTY()
	int CurMeleeDamage;

	UPROPERTY()
	FVector CurKnockbackForceMelee;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer EnemyTags;

	UPROPERTY()
	UNiagaraSystem* CurHitEffect;

	UPROPERTY()
	FVector PreLocation;

	UPROPERTY(BlueprintReadOnly, Category="Components|Fight", meta=(AllowPrivateAccess=true))
	float AttackInterval = 0.5f;
	
public:	
	// Sets default values for this component's properties
	UFightComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Components | Fight")
	float MeleeAttackRadius = 15.f;
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	
	UFUNCTION(BlueprintCallable, Category="Components|Fight")
	void MeleeTraceAttack(FName SocketName, USceneComponent* SceneComponent, int Damage, const FVector& KnockbackForce, UNiagaraSystem* HitEffect);
		
};
