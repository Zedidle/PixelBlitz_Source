// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PixelBlitz.h"
#include "Core/PXGameState.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TimerManager.h"
#include "PXCustomStruct.h"
#include "WeatherSubsystem.generated.h"

class UMaterialInterface;
class UNiagaraSystem;
class APointLight;

// 纯打光方案（用户裁决 2026-11-27）：UDS/UDW 资产与运行时参数零改动，
// 暗档×雨/雪的粒子可见性只靠跟随相机的点光实现
USTRUCT(BlueprintType)
struct FWeatherParticleBrightness
{
	GENERATED_BODY()

	// 暗档×雨雪时跟随相机的点光强度（cd）；0 = 关闭。全向光，与月光方向无关
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather | Particle")
	float ParticleLightIntensity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather | Particle")
	float ParticleLightRadius = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather | Particle")
	FLinearColor ParticleLightColor = FLinearColor(0.8f, 0.85f, 1.0f);
};

UCLASS(Blueprintable)
class PIXELBLITZ_API UDayTimeEffectSight : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// 后续扩展 TArray<FAttributeEffect> ?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttributeEffect DawnEffect = {EPXAttribute::CurSpeed, 0.05f, 10.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttributeEffect LightEffect = {EPXAttribute::CurSpeed, 0.05, 20.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttributeEffect DuskEffect = {EPXAttribute::CurSpeed, -0.05, -10.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttributeEffect NightEffect = {EPXAttribute::CurSpeed, -0.05, -20.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EDayTimeType, FText> DayTime2Name;	

	// 粒子点光分档：未配置的档位走 C++ 默认表（夜 30 / 晨昏 15 / 白天 0）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather | Particle")
	TMap<EDayTimeType, FWeatherParticleBrightness> ParticleBrightness;
};


UCLASS()
class PIXELBLITZ_API UWeatherSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UDayTimeEffectSight* DayTimeEffectSight;
	
	
public:

	static UWeatherSubsystem* GetInstance(const UObject* WorldContextObject);
	
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	FString DayTimeNameString;
	FString WeatherNameString;
	FGameplayTag DayTimeTag = TAG("Buff.DayTime");
	FGameplayTag WeatherTag = TAG("Buff.Weather");
	
	UFUNCTION(BlueprintCallable, Category = "WeatherSystem")
	void MakeWeatherEffect();

	// 纯打光方案：按当前 EDayTimeType 分档驱动跟随相机的点光；MakeWeatherEffect 内自动调用 + 0.5s 重刷
	UFUNCTION(BlueprintCallable, Category = "WeatherSystem")
	void ApplyParticleBrightness();

	// [默认关闭] 暗档把 UDW 雨/雪组件切到项目 UNLIT 复制体的备用路径；打光方案裁决后停用，
	// 代码保留、bParticleMaterialRedirectEnabled 打开才生效
	UFUNCTION(BlueprintCallable, Category = "WeatherSystem")
	void ApplyParticleMaterialOverride();

	// 暗档×雨雪时维持一个跟随相机的点光，让受光的雨/雪丝在任意视角都受光可见
	void UpdateParticleViewLight(const FWeatherParticleBrightness& Brightness);

	// 当前粒子点光状态（DumpCombo 核对用）
	bool GetParticleLightState(float& OutIntensity, float& OutRadius) const;

	// PIE 阈值调试：负值 = 恢复分档默认
	void OverrideParticleBrightness(float CameraLight = -1.f);

private:
	// 雨/雪 Niagara 原版与项目 UNLIT 复制体（备用路径，默认关闭）
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> RainParticleOriginal;
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> SnowParticleOriginal;
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> RainParticleUnlit;
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> SnowParticleUnlit;
	// Sprite 渲染器材质绑定在 USER 参数（Rain/Snow Sprite Material）上，运行时需同步驱动
	UPROPERTY()
	TObjectPtr<UMaterialInterface> RainMaterialUnlit;
	UPROPERTY()
	TObjectPtr<UMaterialInterface> SnowMaterialUnlit;
	UPROPERTY()
	TObjectPtr<UMaterialInterface> RainMaterialOriginalLit;
	UPROPERTY()
	TObjectPtr<UMaterialInterface> SnowMaterialOriginalLit;
	bool bParticleSystemRedirectInitialized = false;

	FTimerHandle ParticleBrightnessTimerHandle;
	bool bParticleTimerStarted = false;
	bool bParticleBrightnessOverride = false;
	FWeatherParticleBrightness ParticleBrightnessOverride;

	// 备用路径总开关（打光方案裁决后默认关闭）
	bool bParticleMaterialRedirectEnabled = false;
	TWeakObjectPtr<APointLight> ParticleViewLight;
};
