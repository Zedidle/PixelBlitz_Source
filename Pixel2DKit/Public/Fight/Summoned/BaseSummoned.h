// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseSummoned.generated.h"
















UCLASS(Abstract)
class PIXEL2DKIT_API ABaseSummoned : public AActor
{
	GENERATED_BODY()

	
	UPROPERTY()
	TWeakObjectPtr<AActor> Target = nullptr;
	

	bool bNeedKeepFaceTarget = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<int> DamagesAtLevel;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true))
	int SummonedLevel = 1;
	
public:
	// Sets default values for this actor's properties
	ABaseSummoned();

	UPROPERTY(BlueprintReadWrite)
	bool bInReady = false;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void Tick_KeepFaceTarget();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetTarget();
	
	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* _Target);
	
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void FaceTargetEffect();
	
	
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetDamage();
	
	
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SummonedEnd();
	
	
};
