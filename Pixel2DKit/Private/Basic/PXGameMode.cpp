// Fill out your copyright notice in the Description page of Project Settings.


#include "Basic/PXGameMode.h"
#include "NavigationSystem.h"
#include "Engine/LevelStreamingDynamic.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"


void APXGameMode::LoadLevel(FString LevelName, FVector Location)
{
	if (AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()))
	{
		PlayerStart->SetActorLocation(Location + FVector(0,0,200));
		if (!PlayerRespawnPoint)
		{
			PlayerRespawnPoint = GetWorld()->SpawnActor<APlayerRespawnPoint>();
		}
		PlayerRespawnPoint->SetActorLocation(Location + FVector(0,0,200));
	}

	PreLevelName = CurLevelName;
	CurLevelName = LevelName;

	FTransform LevelTransform;
	LevelTransform.SetLocation(Location);
	ULevelStreamingDynamic::FLoadLevelInstanceParams Params(GetWorld(), CurLevelName, LevelTransform);
	bool LoadLevelSuccess;
	CurLevelInstance = ULevelStreamingDynamic::LoadLevelInstance(Params, LoadLevelSuccess);
}

void APXGameMode::NavRebuild()
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	ANavMeshBoundsVolume* NavMesh = Cast<ANavMeshBoundsVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), ANavMeshBoundsVolume::StaticClass()));
	NavSys->OnNavigationBoundsAdded(NavMesh);
	NavSys->Build();
}

	