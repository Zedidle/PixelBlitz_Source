// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PixelBlitz.h"
#include "Core/PXGameState.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PXCustomStruct.h"
#include "WeatherSubsystem.generated.h"

class UMaterialParameterCollection;

// UDW 粒子 MPC 的分档倍率（Rain/Snow Sprite Alpha + Ambient Light Intensity）
// 暗档下靠 Ambient Light Intensity 补 Manual EV 割裂的 emissive；Alpha 默认 1 不放大
USTRUCT(BlueprintType)
struct FWeatherParticleBrightness
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather | Particle")
	float RainSpriteAlpha = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather | Particle")
	float RainAmbientLightIntensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather | Particle")
	float SnowSpriteAlpha = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather | Particle")
	float SnowAmbientLightIntensity = 1.0f;
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

	// UDW 粒子 MPC 分档倍率：未配置的档位走 C++ 默认表（夜 3x / 晨昏 1.5x / 白天 1x）
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

	FString DayTimeNameString;
	FString WeatherNameString;
	FGameplayTag DayTimeTag = TAG("Buff.DayTime");
	FGameplayTag WeatherTag = TAG("Buff.Weather");
	
	UFUNCTION(BlueprintCallable, Category = "WeatherSystem")
	void MakeWeatherEffect();

	// 按当前 EDayTimeType 写 UDW 粒子 MPC（零插件资产改动）；MakeWeatherEffect 内自动调用
	UFUNCTION(BlueprintCallable, Category = "WeatherSystem")
	void ApplyParticleBrightness();

	// PIE 阈值调试：负值 = 该参数恢复分档默认；全部负值 = 清空覆盖
	void OverrideParticleBrightness(float RainAmbient = -1.f, float SnowAmbient = -1.f, float RainAlpha = -1.f, float SnowAlpha = -1.f);

	// 读取 MPC 实例当前值，供 DumpCombo 核对是否落到材质参数
	bool GetParticleParameterValues(float& OutRainAmbient, float& OutSnowAmbient, float& OutRainAlpha, float& OutSnowAlpha) const;

private:
	UPROPERTY()
	TObjectPtr<UMaterialParameterCollection> WeatherParticleCollection;

	bool bParticleBrightnessOverride = false;
	FWeatherParticleBrightness ParticleBrightnessOverride;
};
