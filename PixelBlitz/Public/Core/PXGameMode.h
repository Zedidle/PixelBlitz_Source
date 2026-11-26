// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "GameFramework/GameMode.h"
#include "Player/PlayerRespawnPoint.h"

#include "PXGameMode.generated.h"




class APlayerRespawnPoint;
class ULevelStreamingDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartLevelSuccessSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadingLevelSignature);

UCLASS(BlueprintType, Blueprintable)
class PIXELBLITZ_API APXGameMode : public AGameMode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FName PreLevelName;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FName CurLevelName;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	ULevelStreamingDynamic* CurLevelInstance;

	// 当前流送的关卡实例（FLevelData.LevelInstanceNames 随机加载其一，故常态只有 1 个）
	TArray<TObjectPtr<ULevelStreamingDynamic>> CurLevelInstances;

	// 尚未完成加载的实例数，归零即整关就绪
	int32 PendingLevelInstanceCount = 0;

	// 已完成加载处理的实例（防止 OnLevelLoaded 重复触发时重复处理）
	TSet<TObjectPtr<ULevelStreamingDynamic>> ProcessedLevelInstances;

	bool IsLevelLoaded = false;
	bool IsLevelStarted = false;

	
protected:
	
	APXGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> GameStartCountWidgetClass;
	
public:
	
#pragma region GameplayFlow
	UPROPERTY(BlueprintReadOnly, Category=GameplayFlow)
	APlayerRespawnPoint* PlayerRespawnPoint;
	
	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void LoadLevel(FName LevelName, FVector StartLocation=FVector(0), bool bRandomInstanceFromRow=false);

	UFUNCTION(BlueprintNativeEvent, Category=GameplayFlow)
	void DissolvePreLevel(const FName& LevelName, FVector NewLevelLocation);
	
	// 尝试开始当前关卡
	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void TryStartCurLevel();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=GameplayFlow)
	void OnLevelStarted();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=GameplayFlow)
	void ClearPreLevel();
	
	UFUNCTION(BlueprintCallable, Category=GameplayFlow)
	void PrepareGame();

	
#pragma endregion 


	
#pragma region Sound Setting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Setting")
	float SoundSetting_Arg_MusicBasicMulti = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Setting")
	float SoundSetting_Arg_VolumeBasicMulti = 1.0f;
	
#pragma endregion

	
#pragma region Nav
	UFUNCTION(BlueprintCallable, Category = "Nav")
	void NavRebuild();
#pragma endregion

#pragma region WeatherTimeTest
	// ---- 天气×时辰组合测试工具（PIE 控制台命令，配合亮度校准/截图使用） ----

	// 设置天气：TestWeather ClearSky 或 TestWeather 3（枚举名不区分大小写，或 0~14 索引）
	UFUNCTION(Exec, Category = "Testing|WeatherTime")
	void TestWeather(const FString& WeatherInput);

	// 设置时辰：TestTime 1200（HHMM）或 TestTime Wu（十二时辰名，取时段中点）；DirectSet 直接跳转
	UFUNCTION(Exec, Category = "Testing|WeatherTime")
	void TestTime(const FString& TimeInput);

	// 一键组合 + 自动截图：TestCombo ClearSky 1200（ShotDelay 秒后截图，默认 6s 等 UDW 5s 过渡完成；传 -1 不截图）
	UFUNCTION(Exec, Category = "Testing|WeatherTime")
	void TestCombo(const FString& WeatherInput, const FString& TimeInput, float ShotDelay = 6.0f);

	// 单独截图：TestShot ClearSky_1200 [Delay]，保存为 Combo_<名称>.png（Saved/Screenshots/<平台>/）
	UFUNCTION(Exec, Category = "Testing|WeatherTime")
	void TestShot(const FString& ComboName, float ShotDelay = 0.0f);

	// 打印当前组合状态：GameState 档位/天气 + UDS 关键亮度参数（反射读取）
	UFUNCTION(Exec, Category = "Testing|WeatherTime")
	void DumpCombo();

	// 实时改 UDS 变量：SetUDS ExtraNightBrightnessWhenCloudy 0.8（名字去空格/下划线书写亦可命中）；
	// 名字未命中时会在日志列出全部可调 float 属性
	UFUNCTION(Exec, Category = "Testing|WeatherTime")
	void SetUDS(const FString& PropertyName, float Value);

	// 粒子亮度阈值调试：TestParticleBrightness 3 3 -1 -1（顺序 RainAmbient/SnowAmbient/RainAlpha/SnowAlpha）；
	// 负值 = 该项恢复分档默认，全部负值 = 清空覆盖
	UFUNCTION(Exec, Category = "Testing|WeatherTime")
	void TestParticleBrightness(float RainAmbient = -1.f, float SnowAmbient = -1.f, float RainAlpha = -1.f, float SnowAlpha = -1.f);

	// 状态归零组合测试：先归零到锚点（白天×晴）等 8s 收敛，再切目标并延迟 10s 自动截图，
	// 消除前置天气残留/UDW 积雪湿度累积/过渡未收敛对截图的影响
	UFUNCTION(Exec, Category = "Testing|WeatherTime")
	void StabilizeCombo(const FString& WeatherInput, const FString& TimeInput);
#pragma endregion

	UFUNCTION()
	void OnLevelLoaded();
	
};
