// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemySpawner.h"

#include "Enemy/BaseEnemy.h"

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
	if (IsValid(SelfEnemy))
	{
		return SelfEnemy;
	}

	int32 Max = 0;
	for (auto ele : EnemyClassRate)
	{
		Max += ele.Value;
	}
	
	if (Max <= 0) return nullptr;

	
	int32 LocalV = FMath::RandHelper(Max);
	int32 LocalR = 0;
	for (auto ele : EnemyClassRate)
	{
		LocalR += ele.Value;
		if (LocalR > LocalV)
		{
			if (IsValid(ele.Key))
			{
				FTransform* SpawnTransform = new FTransform();
				SpawnTransform->SetLocation(GetActorLocation());
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				SelfEnemy = Cast<ABaseEnemy>(GetWorld()->SpawnActor(ele.Key.Get(), SpawnTransform, SpawnParams));

				if (SelfEnemy)
				{
					for (auto Tag : Tags)
					{
						SelfEnemy->Tags.Add(Tag);
					}
				}
				return SelfEnemy;
			}
		}
	}
	return nullptr;
}

