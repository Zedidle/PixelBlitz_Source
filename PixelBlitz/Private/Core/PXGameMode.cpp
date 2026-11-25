// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PXGameMode.h"
#include "NavigationSystem.h"
#include "PXGameplayTags.h"
#include "Character/BasePXCharacter.h"
#include "Character/Components/AbilityComponent.h"
#include "Core/PXGameInstance.h"
#include "Controller/PXPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Core/PXGameState.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Core/PXSaveGameSubSystemFuncLib.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/DataTable.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "PixelBlitz/PixelBlitz.h"
#include "Platform/BasePlatform.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXGameDataAsset.h"
#include "Settings/Config/PXResourceDataAsset.h" 
#include "Subsystems/DataTableSubsystem.h"
#include "Subsystems/PXAudioSubsystem.h"
#include "Subsystems/TimerManagerFuncLib.h"
#include "UI/UIManager.h"
#include "UI/PXHUD.h"
#include "Utilitys/PXGameplayStatics.h"


namespace
{
	// 关卡推进/排序键 = 行内首个有效实例名（一行可由多个流送实例组成）
	FName GetFirstValidInstanceName(const FLevelData& Row)
	{
		for (const FName& InstanceName : Row.LevelInstanceNames)
		{
			if (!InstanceName.IsNone())
			{
				return InstanceName;
			}
		}
		return FName();
	}
}

APXGameMode::APXGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 配置默认的框架类
	PlayerControllerClass = APXPlayerController::StaticClass();
	GameStateClass = APXGameState::StaticClass();
	HUDClass = APXHUD::StaticClass();
}


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

	bool bDevelopTestMap = false;

	if (UPXGameInstance* GameInstance = GetGameInstance<UPXGameInstance>())
	{

		FName LevelName = GameInstance->GetCurLevelName_Simple(false);
		#if WITH_EDITOR
		if (const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>())
		{
			UDevelopConfigDataAsset* DevelopConfig = Settings->DevelopConfig.LoadSynchronous();
			if (DevelopConfig && DevelopConfig->bLoadTestMap)
			{
				bDevelopTestMap = true;
				if (const UDataTable* TestLevelTable = DevelopConfig->TestMap)
				{
					// 读取测试关卡表：与 StartNewGame 相同的排序规则（LevelOrder 升序，未设置按行名兜底），
					// 取首个有效行的首个有效 LevelInstanceNames 作为测试关卡
					TArray<FLevelData> TestRows;
					for (const FName& RowName : TestLevelTable->GetRowNames())
					{
						if (const FLevelData* Data = TestLevelTable->FindRow<FLevelData>(RowName, TEXT("DevelopTestMap")))
						{
							TestRows.Add(*Data);
						}
					}

					TestRows.Sort([](const FLevelData& A, const FLevelData& B)
					{
						if (A.LevelOrder != B.LevelOrder)
						{
							if (A.LevelOrder == INDEX_NONE)
							{
								return false;
							}

							if (B.LevelOrder == INDEX_NONE)
							{
								return true;
							}

							return A.LevelOrder < B.LevelOrder;
						}

						return GetFirstValidInstanceName(A).ToString() < GetFirstValidInstanceName(B).ToString();
					});

					bool bFoundTestLevel = false;
					for (const FLevelData& Row : TestRows)
					{
						const FName FirstInstance = GetFirstValidInstanceName(Row);
						if (!FirstInstance.IsNone())
						{
							LevelName = FirstInstance;
							bFoundTestLevel = true;
							break;
						}
					}

					if (!bFoundTestLevel)
					{
						UE_LOG(LogTemp, Warning, TEXT("DevelopConfig.TestMap 表 '%s' 中没有有效行，回退到 L_Test"),
							*TestLevelTable->GetName());
						LevelName = FName("L_Test");
					}
				}
				else
				{
					LevelName = FName("L_Test");
				}
			}
		}
		#endif
		// 正式流程：行内候选变体随机加载其一；开发测试地图：确定性加载指定实例
		LoadLevel(LevelName, FVector(0), !bDevelopTestMap);
		TryStartCurLevel();
	}
}

void APXGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}



void APXGameMode::LoadLevel(FName LevelName, FVector StartLocation, bool bRandomInstanceFromRow)
{
	if (LevelName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("APXGameMode::LoadLevel called with an invalid level name."));
		return;
	}

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

	for (ULevelStreamingDynamic* OldInstance : CurLevelInstances)
	{
		if (OldInstance)
		{
			OldInstance->OnLevelLoaded.RemoveAll(this);
		}
	}

	PreLevelName = CurLevelName;
	CurLevelName = LevelName;

	if (!PreLevelName.IsNone())
	{
		DissolvePreLevel(PreLevelName, StartLocation);
	}

	IsLevelLoaded = false;
	IsLevelStarted = false;
	CurLevelInstance = nullptr;
	CurLevelInstances.Reset();
	ProcessedLevelInstances.Reset();
	PendingLevelInstanceCount = 0;

	// 读取关卡行：传入名 = 行名（关卡序号推进键，如 L1_Race），行内 LevelInstanceNames
	// 为同规模/同难度候选池，随机加载其中一个，其余候选仅作加载失败兜底；
	// bRandomInstanceFromRow=false（测试地图等）优先加载传入的实例名；
	// 行名/实例名都查不到时退化为仅加载传入名（保持旧行为）
	TArray<FName> ValidInstances;
	if (UDataTableSubsystem* DataTableSubsystem = UDataTableSubsystem::GetInstance(this))
	{
		if (const FLevelData* LevelRow = DataTableSubsystem->GetLevelDataByKey(LevelName))
		{
			for (const FName& InstanceName : LevelRow->LevelInstanceNames)
			{
				if (!InstanceName.IsNone())
				{
					ValidInstances.AddUnique(InstanceName);
				}
			}
		}
	}

	TArray<FName> LoadOrder;
	if (bRandomInstanceFromRow && ValidInstances.Num() > 0)
	{
		LoadOrder = ValidInstances;
		// 随机选中的候选提到队首，其余保持原序作为失败兜底
		if (LoadOrder.Num() > 1)
		{
			LoadOrder.Swap(0, FMath::RandRange(0, LoadOrder.Num() - 1));
		}
	}
	else
	{
		LoadOrder.AddUnique(LevelName);
		for (const FName& InstanceName : ValidInstances)
		{
			LoadOrder.AddUnique(InstanceName);
		}
	}

	bool bAnyLoadSuccess = false;
	for (const FName& InstanceName : LoadOrder)
	{
		bool LoadLevelSuccess = false;
		ULevelStreamingDynamic* LoadedLevel = ULevelStreamingDynamic::LoadLevelInstance(GetWorld(),
			InstanceName.ToString(), StartLocation, FRotator(0, 0,0),
			LoadLevelSuccess);
		if (LoadLevelSuccess && LoadedLevel)
		{
			LoadedLevel->OnLevelLoaded.AddDynamic(this, &ThisClass::OnLevelLoaded);
			CurLevelInstances.Add(LoadedLevel);
			if (!CurLevelInstance)
			{
				CurLevelInstance = LoadedLevel;
			}
			bAnyLoadSuccess = true;
			break; // 只加载一个实例：候选序中首个成功者
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("APXGameMode::LoadLevel failed to stream level instance '%s'."), *InstanceName.ToString());
		}
	}

	if (bAnyLoadSuccess)
	{
		PendingLevelInstanceCount = CurLevelInstances.Num();

		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		MessageSubsystem.BroadcastMessage(PXGameplayTags::GameplayFlow_OnLevelLoading, EMPTY_MESSAGE);
	}
}

void APXGameMode::DissolvePreLevel_Implementation(const FName& LevelName, FVector NewLevelLocation)
{
	
}

void APXGameMode::TryStartCurLevel()
{
	if (IsLevelLoaded)
	{
		OnLevelStarted();
	}

	IsLevelStarted = true;
}

void APXGameMode::OnLevelStarted_Implementation()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);

	UPXGameInstance* GameInstance = GetGameInstance<UPXGameInstance>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UDataTableSubsystem* DataTableSubsystem = UDataTableSubsystem::GetInstance(this);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem);

	UPXAudioSubsystem* AudioSubsystem = UPXAudioSubsystem::GetInstance(this);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(AudioSubsystem);

	if (const FLevelData* Data = DataTableSubsystem->GetLevelDataByKey(CurLevelName))
	{
		AudioSubsystem->PlayBGM(Data->BGM.LoadSynchronous());
	}

	ClearPreLevel();
	
	PrepareGame();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.BroadcastMessage(PXGameplayTags::GameplayFlow_OnLevelStarted, EMPTY_MESSAGE);
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

void APXGameMode::PrepareGame()
{
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings);

	UPXGameDataAsset* GameDataAsset = Settings->GameDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameDataAsset);

	UPXSaveGameSubsystem* SaveGameSubsystem = UPXSaveGameSubsystem::GetInstance(this);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SaveGameSubsystem);
	
	UPXMainSaveGame* MainSaveGame = UPXSaveGameSubSystemFuncLib::GetMainData(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSaveGame);

	ABasePXCharacter* PXCharacter = UPXGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);
	
	// 如果是新手引导关卡，则不需要技能选择和倒计时
	if (MainSaveGame->CurLevelName == "L_NewGuide" ||
		MainSaveGame->CurLevelName == "L_Home")
	{
		PXCharacter->PreReadyToStart();
		
		if (APXGameState* GS = UPXGameplayStatics::GetGameState(GetWorld()))
		{
			GS->OnGameStart();
		}
		return;
	}
	
	if (SaveGameSubsystem->Main_HasChoiceAbility())
	{
		if (GameStartCountWidgetClass)
		{
			if (UUIManager* UIManager = UUIManager::GetSelfInstance(this))
			{
				UIManager->AddWidget(GameStartCountWidgetClass, ESlateVisibility::SelfHitTestInvisible, false);
			}
		}
		return;
	}
	
	if (MainSaveGame->CurLevel == 1)
	{
		if (PXCharacter->AbilityComponent)
		{
			float Potential_SkillPoints;
			PXCharacter->AbilityComponent->FindExtendData(TAG("Ability.Potential.Set.Value"), Potential_SkillPoints);
			MainSaveGame->RemSkillPoints += static_cast<int>(Potential_SkillPoints);
		}
	}
	MainSaveGame->RemSkillPoints += GameDataAsset->BasePerLevelSkillPointsGet;
	MainSaveGame->RemRefreshPoints += GameDataAsset->BasePerLevelSkillRefreshPointsGet;

	UTimerManagerFuncLib::SetDelay(GetWorld(), [WeakThis = TWeakObjectPtr(this)]
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
	// 多实例关卡：每个实例加载完成后各处理一次（防重复），全部就绪才置 IsLevelLoaded
	for (ULevelStreamingDynamic* Instance : CurLevelInstances)
	{
		if (!Instance || !Instance->GetLoadedLevel() || ProcessedLevelInstances.Contains(Instance))
		{
			continue;
		}
		ProcessedLevelInstances.Add(Instance);

		TArray<TObjectPtr<AActor>>& FoundActors = Instance->GetLoadedLevel()->Actors;
		TArray<AActor*> TempLights;
		for (AActor* Actor : FoundActors)
		{
			if (Actor)
			{
				Actor->Tags.Add(CurLevelName);
				// 白盒临时编辑灯（Tag=EditorLight，由白盒生成脚本自动打标）：运行期移除，正式光照交给 UDS
				if (Actor->ActorHasTag(TEXT("EditorLight")))
				{
					TempLights.Add(Actor);
				}
			}
			if (ABasePlatform* Platform = Cast<ABasePlatform>(Actor))
			{
				Platform->SetLoadLevelLocationOffset(Instance->LevelTransform.GetLocation());
			}
		}

		for (AActor* TempLight : TempLights)
		{
			TempLight->Destroy();
		}
	}

	if (PendingLevelInstanceCount > 0 && --PendingLevelInstanceCount == 0)
	{
		IsLevelLoaded = true;
		ProcessedLevelInstances.Reset();

		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		MessageSubsystem.BroadcastMessage(PXGameplayTags::GameplayFlow_OnLevelLoaded, EMPTY_MESSAGE);

		if (IsLevelStarted)
		{
			OnLevelStarted();
		}
	}
}

	
