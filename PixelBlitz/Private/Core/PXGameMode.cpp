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
#include "Subsystems/WeatherSubsystem.h"
#include "Subsystems/TimerManagerFuncLib.h"
#include "UI/UIManager.h"
#include "UI/PXHUD.h"
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/PXGameplayStatics.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "PXCustomStruct.h"
#include "UnrealClient.h"


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

	// ---- 天气×时辰组合测试工具的解析/反射辅助 ----

	template<typename E>
	FString PXEnumName(E Value)
	{
		if (const UEnum* Enum = StaticEnum<E>())
		{
			return Enum->GetNameStringByValue(static_cast<int64>(Value));
		}
		return TEXT("?");
	}

	// 天气参数解析：纯数字=索引（0~13），否则按枚举名（大小写不敏感）
	bool PXParseWeather(const FString& In, EWeatherDetail& Out, FString& OutName, FString& ErrHint)
	{
		const UEnum* Enum = StaticEnum<EWeatherDetail>();
		if (!Enum)
		{
			ErrHint = TEXT("EWeatherDetail 反射失败");
			return false;
		}

		if (In.IsNumeric())
		{
			const int32 Idx = FCString::Atoi(*In);
			if (Idx >= 0 && Idx < Enum->NumEnums() - 1)
			{
				Out = static_cast<EWeatherDetail>(Enum->GetValueByIndex(Idx));
				OutName = Enum->GetNameStringByIndex(Idx);
				return true;
			}
			ErrHint = FString::Printf(TEXT("索引 %d 超界（0~%d）"), Idx, Enum->NumEnums() - 2);
			return false;
		}

		// UE5.6 的 GetValueByName 默认（None）即大小写不敏感匹配
		const int64 Value = Enum->GetValueByName(FName(*In));
		if (Value != INDEX_NONE)
		{
			Out = static_cast<EWeatherDetail>(Value);
			OutName = Enum->GetNameStringByValue(Value);
			return true;
		}
		ErrHint = TEXT("按枚举名未找到（如 ClearSky / DenseOvercast / Blizzard）");
		return false;
	}

	// 时间参数解析：纯数字=HHMM，否则按十二时辰名（取时段中点）
	bool PXParseTime(const FString& In, float& OutTime, FString& OutName, FString& ErrHint)
	{
		if (In.IsNumeric())
		{
			OutTime = FCString::Atof(*In);
			OutName = In;
			return true;
		}

		if (const UEnum* HourEnum = StaticEnum<EChineseHour>())
		{
			const int64 Value = HourEnum->GetValueByName(FName(*In));
			if (Value != INDEX_NONE)
			{
				OutTime = UCommonFuncLib::ChineseHourToTime(static_cast<EChineseHour>(Value), false);
				OutName = HourEnum->GetNameStringByValue(Value);
				return true;
			}
		}
		ErrHint = TEXT("支持 HHMM 数字（如 1200）或十二时辰名（Zi/Chou/Yin/Mao/Chen/Si/Wu/Wei/Shen/You/Xu/Hai）");
		return false;
	}

	// 定位 UDS actor（蓝图类无法 C++ 强引用，按类名匹配）
	AActor* PXFindUDSActor(UWorld* World)
	{
		if (!World)
		{
			return nullptr;
		}
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			if (It->GetClass()->GetName().Contains(TEXT("Ultra_Dynamic_Sky")))
			{
				return *It;
			}
		}
		return nullptr;
	}

	// 名字规范化：去空格/下划线/斜杠、统一大写。UDS 蓝图的变量名带空格或斜杠
	// （如 "Extra Night Brightness When Cloudy"、"Exposure Bias Dawn/Dusk"），
	// 规范化后可用驼峰写法命中真实属性
	FString PXNormalizeName(const FString& In)
	{
		FString Out;
		for (TCHAR C : In)
		{
			if (C != TEXT(' ') && C != TEXT('_') && C != TEXT('/'))
			{
				Out.AppendChar(FChar::ToUpper(C));
			}
		}
		return Out;
	}

	// 反射读 UDS 蓝图 float 变量（候选名规范化匹配，读不到返回 false）
	bool PXReadFloat(AActor* Actor, const TArray<FString>& CandidateNames, float& OutValue)
	{
		if (!Actor)
		{
			return false;
		}
		for (const FString& Name : CandidateNames)
		{
			const FName Target(*PXNormalizeName(Name));
			for (TFieldIterator<FFloatProperty> It(Actor->GetClass()); It; ++It)
			{
				if (FName(*PXNormalizeName(It->GetName())) == Target)
				{
					OutValue = It->GetPropertyValue_InContainer(Actor);
					return true;
				}
			}
		}
		return false;
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

// ---- 天气×时辰组合测试工具（Exec 控制台命令） ----

void APXGameMode::TestWeather(const FString& WeatherInput)
{
	APXGameState* GS = GetGameState<APXGameState>();
	if (!GS)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TestCombo] GameState 未就绪"));
		return;
	}

	EWeatherDetail Weather = EWeatherDetail::None;
	FString ParsedName, Hint;
	if (!PXParseWeather(WeatherInput, Weather, ParsedName, Hint))
	{
		FString Options;
		if (const UEnum* Enum = StaticEnum<EWeatherDetail>())
		{
			for (int32 i = 0; i < Enum->NumEnums() - 1; ++i)
			{
				Options += FString::Printf(TEXT("%d=%s  "), i, *Enum->GetNameStringByIndex(i));
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("[TestCombo] 天气参数无效 '%s'：%s\n可用：%s"), *WeatherInput, *Hint, *Options);
		return;
	}

	// SetWeather 只更新 WeatherName/WeatherEffect/UDW 画面，不写 CurWeatherDetail（只有 RandomWeather/ForceWeather 分支写），
	// 必须在此同步，否则 DumpCombo 的天气枚举与实际生效天气错位
	GS->CurWeatherDetail = Weather;
	GS->SetWeather(Weather);
	UE_LOG(LogTemp, Log, TEXT("[TestCombo] 天气已设为 %s（UDW 过渡约 5s）"), *ParsedName);
}

void APXGameMode::TestTime(const FString& TimeInput)
{
	APXGameState* GS = GetGameState<APXGameState>();
	if (!GS)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TestCombo] GameState 未就绪"));
		return;
	}

	float NewTime = 0.f;
	FString ParsedName, Hint;
	if (!PXParseTime(TimeInput, NewTime, ParsedName, Hint))
	{
		UE_LOG(LogTemp, Warning, TEXT("[TestCombo] 时辰参数无效 '%s'：%s"), *TimeInput, *Hint);
		return;
	}

	// DirectSet=true 直接跳转（BP 的 Transition Time Of Day 允许回拨）；档位由 BP CheckNewDayTimeType 每 0.2s 轮询刷新
	// bAbortWeatherChange=true：跨档触发 EventOnDayTimeTypeChanged 时不执行 RandomWeather，避免覆盖 TestWeather 手动设定的天气；
	// 该事件里的 MakeWeatherEffect 仍会执行，Buff 照常刷新
	GS->PassDayTime(NewTime, true, true, 3.0f);
	UE_LOG(LogTemp, Log, TEXT("[TestCombo] 时辰已设为 %s（HHMM=%.0f，预期档位=%s，GameState.DayTimeType 将于 0.2s 轮询后刷新）"),
		*ParsedName, NewTime, *PXEnumName(GS->CalDayTimeType(NewTime)));
}

void APXGameMode::TestCombo(const FString& WeatherInput, const FString& TimeInput, float ShotDelay)
{
	// 分开执行，便于单独失败时定位
	TestWeather(WeatherInput);
	TestTime(TimeInput);

	if (ShotDelay >= 0.f)
	{
		TestShot(FString::Printf(TEXT("%s_%s"), *WeatherInput, *TimeInput), ShotDelay);
	}
}

void APXGameMode::TestShot(const FString& ComboName, float ShotDelay)
{
	if (ShotDelay > 0.f)
	{
		// 延迟截图：等 UDW（5s）/时辰过渡完成后再截
		const TWeakObjectPtr<APXGameMode> WeakThis(this);
		const FString CapturedName = ComboName;
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, [WeakThis, CapturedName]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->TestShot(CapturedName, 0.f);
			}
		}, ShotDelay, false);
		UE_LOG(LogTemp, Log, TEXT("[TestCombo] 将在 %.1f 秒后截图 Combo_%s（等待过渡完成）"), ShotDelay, *ComboName);
		return;
	}

	const FString FileName = FString::Printf(TEXT("Combo_%s"), *ComboName);
	// bShowUI=false：不含 HUD；bAddFilenameSuffix=true：同组合重测自动编号不覆盖
	FScreenshotRequest::RequestScreenshot(FileName, false, true);
	UE_LOG(LogTemp, Log, TEXT("[TestCombo] 截图请求已发出：%s.png（位于 Saved/Screenshots/<平台>/）"), *FileName);
}

void APXGameMode::DumpCombo()
{
	APXGameState* GS = GetGameState<APXGameState>();
	if (!GS)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DumpCombo] GameState 未就绪"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[DumpCombo] TimeOfDay=%.0f | 预期档位=%s | GameState.DayTimeType=%s | 天气=%s(%s) | 视距罚=%.2f"),
		GS->TimeOfDay,
		*PXEnumName(GS->CalDayTimeType(GS->TimeOfDay)),
		*PXEnumName(GS->DayTimeType),
		*PXEnumName(GS->CurWeatherDetail),
		*GS->WeatherName.ToString(),
		GS->WeatherEffect.SightPercent);

	// 天气属性全字段：用于排查伤害/回血异常（如暴风雪下打不死怪）
	UE_LOG(LogTemp, Log, TEXT("[DumpCombo] WeatherEffect: 视距=%.2f | HP/s=%d | 移速=%.2f | EP=%.2f | 伤害修正=%.2f"),
		GS->WeatherEffect.SightPercent,
		GS->WeatherEffect.HPEffectPerSecond,
		GS->WeatherEffect.MoveSpeedEffectPercent,
		GS->WeatherEffect.EPConsumePercent,
		GS->WeatherEffect.DamagePlusPercent);

	if (AActor* Sky = PXFindUDSActor(GetWorld()))
	{
		auto LogFloat = [&](const TCHAR* Label, const TArray<FString>& Names)
		{
			float Value = 0.f;
			if (PXReadFloat(Sky, Names, Value))
			{
				UE_LOG(LogTemp, Log, TEXT("[DumpCombo] UDS %s = %.2f"), Label, Value);
			}
		};

		// UDS 蓝图变量为带空格 FName（实测于 Ultra_Dynamic_Sky.uasset），无空格驼峰形式读不到；
		// Lighting Brightness Day/Night 分档变量在 UDS 中不存在，直射亮度走单一 "Lighting Brightness" + 函数混合
		LogFloat(TEXT("Time of Day"), { TEXT("Time Of Day") });
		LogFloat(TEXT("Dawn Time"), { TEXT("Dawn Time") });
		LogFloat(TEXT("Dusk Time"), { TEXT("Dusk Time") });
		LogFloat(TEXT("Extend Dawn And Dusk"), { TEXT("Extend Dawn And Dusk") });
		LogFloat(TEXT("Lighting Brightness"), { TEXT("Lighting Brightness") });
		LogFloat(TEXT("Night Brightness"), { TEXT("Night Brightness") });
		LogFloat(TEXT("Extra Night Brightness When Cloudy"), { TEXT("Extra Night Brightness When Cloudy") });
		LogFloat(TEXT("Overcast Brightness Night"), { TEXT("Overcast Brightness Night") });
		LogFloat(TEXT("Space Layer Brightness"), { TEXT("Space Layer Brightness") });
		LogFloat(TEXT("Scene Lighting Brightness Scale"), { TEXT("Scene Lighting Brightness Scale") });
		LogFloat(TEXT("Current Exposure Bias"), { TEXT("Current Exposure Bias") });
		LogFloat(TEXT("Exposure Bias Day"), { TEXT("Exposure Bias Day") });
		LogFloat(TEXT("Exposure Bias Night"), { TEXT("Exposure Bias Night") });
		LogFloat(TEXT("Exposure Bias Dawn/Dusk"), { TEXT("Exposure Bias Dawn/Dusk") });
		LogFloat(TEXT("Exposure Bias Cloudy"), { TEXT("Exposure Bias Cloudy") });
		LogFloat(TEXT("Exposure Bias Foggy"), { TEXT("Exposure Bias Foggy") });
		LogFloat(TEXT("Exposure Bias Dusty"), { TEXT("Exposure Bias Dusty") });
		// 收敛判据：同一组合多次截图，以下值应一致；不一致 = 状态未收敛，截图作废
		LogFloat(TEXT("Cloud Coverage"), { TEXT("Current Cloud Coverage"), TEXT("Cloud Coverage") });
		LogFloat(TEXT("Current Space Layer Brightness"), { TEXT("Current Space Layer Brightness") });
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[DumpCombo] 未找到 UDS actor"));
	}

	float RainAmbient = 0.f, SnowAmbient = 0.f, RainAlpha = 0.f, SnowAlpha = 0.f;
	if (UWeatherSubsystem* Weather = GetGameInstance() ? GetGameInstance()->GetSubsystem<UWeatherSubsystem>() : nullptr)
	{
		if (Weather->GetParticleParameterValues(RainAmbient, SnowAmbient, RainAlpha, SnowAlpha))
		{
			UE_LOG(LogTemp, Log, TEXT("[DumpCombo] UDW粒子MPC: RainAmbient=%.2f SnowAmbient=%.2f RainAlpha=%.2f SnowAlpha=%.2f"),
				RainAmbient, SnowAmbient, RainAlpha, SnowAlpha);
		}
	}
}

void APXGameMode::SetUDS(const FString& PropertyName, float Value)
{
	AActor* Sky = PXFindUDSActor(GetWorld());
	if (!Sky)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SetUDS] 未找到 UDS actor"));
		return;
	}

	const FName Target(*PXNormalizeName(PropertyName));
	for (TFieldIterator<FFloatProperty> It(Sky->GetClass()); It; ++It)
	{
		if (FName(*PXNormalizeName(It->GetName())) == Target)
		{
			It->SetPropertyValue_InContainer(Sky, Value);
			UE_LOG(LogTemp, Log, TEXT("[SetUDS] 已写入 %s = %.3f（若画面未变化，该属性可能需经 UDS 内部刷新流程，改在 Details 面板试或把现象发我）"), *It->GetName(), Value);
			return;
		}
	}

	// 未命中：列出全部 float 属性（含父类），帮用户对名字
	FString FloatProps;
	for (TFieldIterator<FFloatProperty> It(Sky->GetClass()); It; ++It)
	{
		FloatProps += FString::Printf(TEXT("%s=%.3f\n"), *It->GetName(), It->GetPropertyValue_InContainer(Sky));
	}
	UE_LOG(LogTemp, Warning, TEXT("[SetUDS] 未找到 float 属性 '%s'。可用属性（名字可去空格/下划线书写）：\n%s"), *PropertyName, *FloatProps);
}

void APXGameMode::TestParticleBrightness(float RainAmbient, float SnowAmbient, float RainAlpha, float SnowAlpha)
{
	UGameInstance* GI = GetGameInstance();
	UWeatherSubsystem* Weather = GI ? GI->GetSubsystem<UWeatherSubsystem>() : nullptr;
	if (!Weather)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TestParticleBrightness] WeatherSubsystem 未就绪"));
		return;
	}

	Weather->OverrideParticleBrightness(RainAmbient, SnowAmbient, RainAlpha, SnowAlpha);
	UE_LOG(LogTemp, Log, TEXT("[TestParticleBrightness] 已下发 RainAmbient=%.2f SnowAmbient=%.2f RainAlpha=%.2f SnowAlpha=%.2f（负值=恢复分档默认）"),
		RainAmbient, SnowAmbient, RainAlpha, SnowAlpha);
}

void APXGameMode::StabilizeCombo(const FString& WeatherInput, const FString& TimeInput)
{
	// 归零：切锚点（白天×晴，远离暗侧补偿与积雪通道），不截图
	TestWeather(TEXT("ClearSky"));
	TestTime(TEXT("1200"));
	UE_LOG(LogTemp, Log, TEXT("[StabilizeCombo] 已归零到锚点（白天×晴），8 秒后切目标…"));

	const TWeakObjectPtr<APXGameMode> WeakThis(this);
	const FString CapturedWeather = WeatherInput;
	const FString CapturedTime = TimeInput;

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, [WeakThis, CapturedWeather, CapturedTime]()
	{
		if (!WeakThis.IsValid())
		{
			return;
		}
		// 目标组合：10s 后截图（UDW 5s 过渡 + 收敛余量）
		WeakThis->TestCombo(CapturedWeather, CapturedTime, 10.f);
		UE_LOG(LogTemp, Log, TEXT("[StabilizeCombo] 目标组合已下发，10 秒后自动截图；期间请勿敲其它命令，截完立即 DumpCombo 核对收敛值"));
	}, 8.f, false);
}

	
