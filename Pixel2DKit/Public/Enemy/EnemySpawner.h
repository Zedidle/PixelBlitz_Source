// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"


class ABaseEnemy;

UCLASS(BlueprintType)
class PIXEL2DKIT_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	ABaseEnemy* SelfEnemy;
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool EnsureSpawn = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<ABaseEnemy>, int32> EnemyClassRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EnemyLevel;
	
	UFUNCTION(BlueprintCallable)
	ABaseEnemy* SpawnEnemy();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ABaseEnemy* GetSelfEnemy();
};
