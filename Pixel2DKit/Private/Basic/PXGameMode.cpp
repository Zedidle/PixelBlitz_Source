// Fill out your copyright notice in the Description page of Project Settings.


#include "Basic/PXGameMode.h"
#include "NavigationSystem.h"
#include "Basic/PXGameInstance.h"
#include "Basic/PXPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Enemy/BaseEnemy.h"
#include "Engine/LevelStreamingDynamic.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/CustomResourceSettings.h"
#include "Utilitys/DebugFuncLab.h"


void APXGameMode::BeginPlay()
{
	Super::BeginPlay();


	if (AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()))
	{
		if (UCapsuleComponent* CapsuleComponent = PlayerStart->GetComponentByClass<UCapsuleComponent>())
		{
			CapsuleComponent->SetMobility(EComponentMobility::Movable);
		}
	}

	if (UPXGameInstance* GameInstance = GetGameInstance<UPXGameInstance>())
	{
		LoadLevel(GameInstance->GetCurLevelName_Simple(true));
		StartCurLevel();
	}
	
}

void APXGameMode::LoadLevel(FName LevelName, FVector StartLocation, FRotator StartRotation)
{
	if (AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()))
	{
		PlayerStart->SetActorLocation(StartLocation + FVector(0,0,200));
		if (!PlayerRespawnPoint)
		{
			if (const UCustomResourceSettings* ResourceSettings = GetDefault<UCustomResourceSettings>())
			{
				if (ResourceSettings->PlayerRespawnPointClass)
				{
					PlayerRespawnPoint = Cast<APlayerRespawnPoint>(GetWorld()->SpawnActor(ResourceSettings->PlayerRespawnPointClass));
				}
			}
		}
		if (PlayerRespawnPoint)
		{
			PlayerRespawnPoint->SetActorLocation(PlayerStart->GetActorLocation());
		}
	}

	if (CurLevelInstance)
	{
		CurLevelInstance->OnLevelLoaded.RemoveDynamic(this, &ThisClass::OnLevelLoaded);
	}
	
	PreLevelName = CurLevelName;
	CurLevelName = LevelName;

	
	bool LoadLevelSuccess;
	ULevelStreamingDynamic* LoadedLevel = ULevelStreamingDynamic::LoadLevelInstance(GetWorld(), LevelName.ToString(), StartLocation, StartRotation,
		LoadLevelSuccess);
	if (LoadLevelSuccess && LoadedLevel)
	{
		CurLevelInstance = LoadedLevel;

		CurLevelInstance->OnLevelLoaded.AddDynamic(this, &ThisClass::OnLevelLoaded);
	}
}

void APXGameMode::StartCurLevel()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);
	if (APXPlayerController* PC = Cast<APXPlayerController>(UGameplayStatics::GetPlayerController(World, 0)))
	{
		PC->OnCharacterControl(false);
	}
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClassWithTag(World, AActor::StaticClass(), PreLevelName, Actors);
	for (AActor* Actor : Actors)
	{
		Actor->SetActorEnableCollision(false);
		Actor->SetLifeSpan(FMath::RandRange(0.05, 0.2));
	}

	if (CurLevelInstance)
	{
		OnStartLevelSuccess();
	}
}

void APXGameMode::OnStartLevelSuccess_Implementation()
{
	
}

void APXGameMode::UnloadCurLevel()
{
	if (CurLevelInstance)
	{
		// 设置卸载参数
		CurLevelInstance->SetShouldBeLoaded(false);
		CurLevelInstance->SetShouldBeVisible(false);
    
		// 可选：设置卸载完成回调
		CurLevelInstance->OnLevelUnloaded.AddDynamic(this, &ThisClass::HandleLevelUnloaded);
    
		// 强制立即卸载
		GetWorld()->FlushLevelStreaming();
	}
}

void APXGameMode::HandleLevelUnloaded()
{
	UE_LOG(LogTemp, Log, TEXT("Dynamic level unloaded successfully"));
	CurLevelInstance = nullptr;
}

void APXGameMode::NavRebuild()
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	ANavMeshBoundsVolume* NavMesh = Cast<ANavMeshBoundsVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), ANavMeshBoundsVolume::StaticClass()));
	NavSys->OnNavigationBoundsAdded(NavMesh);
	NavSys->Build();
}

void APXGameMode::OnLevelLoaded()
{
	if (CurLevelInstance && CurLevelInstance->GetLoadedLevel())
	{
		TArray<AActor*>& FoundActors = CurLevelInstance->GetLoadedLevel()->Actors;
		for (AActor* Actor : FoundActors)
		{
			if (Actor)
			{
				Actor->Tags.Add(CurLevelName);
			}
		}
	}
}

	