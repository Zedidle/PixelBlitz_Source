// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemySpawner.h"

#include "Enemy/BaseEnemy.h"
#include "Utilitys/CommonFuncLib.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ABaseEnemy* AEnemySpawner::SpawnEnemy()
{
	if (!EnsureSpawn) return nullptr;
	if (IsValid(SelfEnemy))
	{
		return SelfEnemy;
	}
	TSubclassOf<ABaseEnemy> EnemyClass;
	if (UCommonFuncLib::CalRandomMap(EnemyClassRate, EnemyClass))
	{
		FTransform* SpawnTransform = new FTransform();
		SpawnTransform->SetLocation(GetActorLocation());
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SelfEnemy = Cast<ABaseEnemy>(GetWorld()->SpawnActor(EnemyClass, SpawnTransform, SpawnParams));
		SelfEnemy->Initialize(EnemyLevel);
		if (SelfEnemy)
		{
			for (auto Tag : Tags)
			{
				SelfEnemy->Tags.Add(Tag);
			}
		}
		return SelfEnemy;
	}

	return nullptr;
}

