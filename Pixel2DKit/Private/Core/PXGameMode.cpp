// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PXGameMode.h"
#include "NavigationSystem.h"
#include "Character/BasePXCharacter.h"
#include "Core/PXGameInstance.h"
#include "Controller/PXPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Engine/LevelStreamingDynamic.h"
#include "GameFramework/PlayerStart.h"
#include "Interfaces/Fight_Interface.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXGameDataAsset.h"
#include "Settings/Config/PXResourceDataAsset.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "UI/UIManager.h"
#include "Utilitys/PXGameplayStatics.h"


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
	}
}

void APXGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	OnStartLevelLoaded.RemoveAll(this);
}



void APXGameMode::LoadLevel(FName LevelName, FVector StartLocation, FRotator StartRotation)
{
	if (AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()))
	{
		PlayerStart->SetActorLocation(StartLocation + FVector(0,0,200));
		if (!PlayerRespawnPoint)
		{
			const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

			const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
			CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceDataAsset)
			
			if (ResourceDataAsset->PlayerRespawnPointClass)
			{
				PlayerRespawnPoint = Cast<APlayerRespawnPoint>(GetWorld()->SpawnActor(ResourceDataAsset->PlayerRespawnPointClass));
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

	IsLevelLoaded = false;
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
	if (IsLevelLoaded)
	{
		OnStartLevelSuccess();
	}

	if (!OnStartLevelLoaded.IsAlreadyBound(this, &ThisClass::OnStartLevelSuccess))
	{
		OnStartLevelLoaded.AddDynamic(this, &ThisClass::OnStartLevelSuccess);
	}
}

void APXGameMode::OnStartLevelSuccess_Implementation()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CurLevelInstance);

	OnStartLevelLoaded.RemoveDynamic(this, &ThisClass::OnStartLevelSuccess);
	
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

void APXGameMode::PrepareForRole()
{
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings);

	UPXGameDataAsset* GameDataAsset = Settings->GameDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameDataAsset);

	UPXMainSaveGame* MainSaveGame = UPXSaveGameSubSystemFuncLib::GetMainData(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSaveGame);

	ABasePXCharacter* PXCharacter = UPXGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);

	if (MainSaveGame->CurLevel == 1)
	{
		float Potential_SkillPoints = PXCharacter->GetEffectGameplayTag(TAG("TalentSet.Potential"));
		MainSaveGame->RemSkillPoints += static_cast<int>(Potential_SkillPoints);
	}
	MainSaveGame->RemSkillPoints += GameDataAsset->BasePerLevelSkillPointsGet;
	MainSaveGame->RemRefreshPoints += GameDataAsset->BasePerLevelSkillRefreshPointsGet;

	UTimerSubsystemFuncLib::SetDelay(GetWorld(), [WeakThis = TWeakObjectPtr<ThisClass>(this)]
	{
		if (!WeakThis.IsValid()) return;

		UWorld* World = WeakThis->GetWorld();
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

		UUIManager* UIManager = UUIManager::GetSelfInstance(World);
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(UIManager);
		
		UIManager->OpenUI("AbilityChoice");
		
	}, 1.0f);
	
}

void APXGameMode::NavRebuild()
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	ANavMeshBoundsVolume* NavMesh = Cast<ANavMeshBoundsVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), ANavMeshBoundsVolume::StaticClass()));
	NavSys->OnNavigationBoundsAdded(NavMesh);
	NavSys->Build();
}

void APXGameMode::OnLevelLoaded_Implementation()
{
	if (CurLevelInstance && CurLevelInstance->GetLoadedLevel())
	{
		IsLevelLoaded = true;
		TArray<AActor*>& FoundActors = CurLevelInstance->GetLoadedLevel()->Actors;
		for (AActor* Actor : FoundActors)
		{
			if (Actor)
			{
				Actor->Tags.Add(CurLevelName);
			}
		}
		OnStartLevelLoaded.Broadcast();
	}
}

	