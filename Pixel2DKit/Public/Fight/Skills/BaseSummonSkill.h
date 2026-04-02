// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Fight/Skills/BaseSkill.h"
#include "BaseSummonSkill.generated.h"

class ABaseSummoned;

UCLASS()
class PIXEL2DKIT_API ABaseSummonSkill : public ABaseSkill
{
	GENERATED_BODY()

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true, ExposeOnSpawn = true))
	int SkillLevel = 1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true, ExposeOnSpawn = true))
	float SpawnRadius = 100.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
	TSubclassOf<ABaseSummoned> SummonedClass;
	
	FTimerHandle SummonTimer;
	float SpawnInterval;
	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
	TArray<float> SustainTimesAtLevel;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess=true))
	TArray<int> SpawnNumsAtLevel;
	
	UPROPERTY()
	TWeakObjectPtr<AActor> Target;
	
	
	
public:
	// Sets default values for this actor's properties
	ABaseSummonSkill();

	UPROPERTY()
	TArray<ABaseSummoned*> Summoneds;
	
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void OnDispel(int localnum);
	
	UFUNCTION(BlueprintCallable)
	void OnOwnerDie();
	
	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* _Target);

	void Summon();
	
	void End();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetSustainTime();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetSpawnNum();
};
