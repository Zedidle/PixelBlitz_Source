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
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXGameDataAsset.h"
#include "Settings/Config/PXResourceDataAsset.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/PXAudioSubsystem.h"
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

		FName LevelName = GameInstance->GetCurLevelName_Simple(true);
		#if WITH_EDITOR
		if (const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>())
		{
			UDevelopConfigDataAsset* DevelopConfig = Settings->DevelopConfig.LoadSynchronous();
			if (DevelopConfig && DevelopConfig->bLoadTestMap)
			{
				if (DevelopConfig->TestMap.IsValid())
				{
					LevelName = *DevelopConfig->TestMap.GetAssetName();
				}
				else
				{
					LevelName = FName("L_Test");
				}
			}
		}
		#endif
		LoadLevel(LevelName);
		TryStartCurLevel();
	}
}

void APXGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}



void APXGameMode::LoadLevel(FName LevelName, FVector StartLocation)
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
			PlayerRespawnPoint->SetLocation(PlayerStart->GetActorLocation());
		}
	}

	if (CurLevelInstance)
	{
		CurLevelInstance->OnLevelLoaded.RemoveDynamic(this, &ThisClass::OnLevelLoaded);
	}

	PreLevelName = CurLevelName;
	CurLevelName = LevelName;

	if (!PreLevelName.IsNone())
	{
		DissolvePreLevel(PreLevelName, StartLocation);
	}
	
	IsLevelLoaded = false;
	IsLevelStarted = false;
	
	bool LoadLevelSuccess;
	ULevelStreamingDynamic* LoadedLevel = ULevelStreamingDynamic::LoadLevelInstance(GetWorld(),
		LevelName.ToString(), StartLocation, FRotator(0, 0,0),
		LoadLevelSuccess);
	if (LoadLevelSuccess && LoadedLevel)
	{
		CurLevelInstance = LoadedLevel;
		CurLevelInstance->OnLevelLoaded.AddDynamic(this, &ThisClass::OnLevelLoaded);
	}
}

void APXGameMode::DissolvePreLevel_Implementation(const FName& LevelName, FVector NewLevelLocation)
{
	
}

void APXGameMode::TryStartCurLevel()
{
	if (IsLevelLoaded)
	{
		OnStartLevelSuccess();
	}

	IsLevelStarted = true;
}

void APXGameMode::OnStartLevelSuccess_Implementation()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);

	UPXGameInstance* GameInstance = GetGameInstance<UPXGameInstance>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem);

	UPXAudioSubsystem* AudioSubsystem = GameInstance->GetSubsystem<UPXAudioSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AudioSubsystem);
	
	
	if (APXPlayerController* PC = Cast<APXPlayerController>(UGameplayStatics::GetPlayerController(World, 0)))
	{
		PC->OnCharacterControl(false);
	}

	if (const FLevelData* Data = DataTableSubsystem->GetLevelDataByName(CurLevelName))
	{
		AudioSubsystem->PlayBGM(Data->BGM.LoadSynchronous());
	}

	ClearPreLevel();
}

void APXGameMode::ClearPreLevel_Implementation()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);

	if (PreLevelName.IsNone()) return;
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClassWithTag(World, AActor::StaticClass(), PreLevelName, Actors);
	for (AActor* Actor : Actors)
	{
		Actor->SetActorEnableCollision(false);
		Actor->SetLifeSpan(FMath::RandRange(4, 5));
	}
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

void APXGameMode::OnLevelLoaded()
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
		if (IsLevelStarted)
		{
			OnStartLevelSuccess();
		}
	}
}

	