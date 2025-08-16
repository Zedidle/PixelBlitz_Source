// Fill out your copyright notice in the Description page of Project Settings.


#include "Basic/PXGameMode.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"




void APXGameMode::NavRebuild()
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	ANavMeshBoundsVolume* NavMesh = Cast<ANavMeshBoundsVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), ANavMeshBoundsVolume::StaticClass()));
	NavSys->OnNavigationBoundsAdded(NavMesh);
	NavSys->Build();
}

	