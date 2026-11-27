// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/WeatherSubsystem.h"
#include "Core/PXGameState.h"
#include "Character/BasePXCharacter.h"
#include "Character/Components/BuffComponent.h"
#include "Enemy/BaseEnemy.h"
#include "EngineUtils.h"
#include "GAS/PXGameplayEffect.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/PointLightComponent.h"
#include "Engine/PointLight.h"
#include "Kismet/GameplayStatics.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXGameDataAsset.h"
#include "UObject/UnrealType.h"
#include "Utilitys/PXGameplayStatics.h"


namespace
{
	const TCHAR* RainParticleUnlitPath = TEXT("/Game/Weather/Particles/Rain_PX.Rain_PX");
	const TCHAR* SnowParticleUnlitPath = TEXT("/Game/Weather/Particles/Snow_PX.Snow_PX");
	const TCHAR* RainMaterialUnlitPath = TEXT("/Game/Weather/M_WeatherRain_Unlit.M_WeatherRain_Unlit");
	const TCHAR* SnowMaterialUnlitPath = TEXT("/Game/Weather/M_WeatherSnow_Unlit.M_WeatherSnow_Unlit");
	const TCHAR* RainMaterialOriginalPath = TEXT("/Game/UltraDynamicSky/Materials/Weather/Rain_ParticleMat.Rain_ParticleMat");
	const TCHAR* SnowMaterialOriginalPath = TEXT("/Game/UltraDynamicSky/Materials/Weather/Snow_ParticleMat.Snow_ParticleMat");

	AActor* FindUDWActor(UWorld* World)
	{
		if (!World)
		{
			return nullptr;
		}
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			if (It->GetClass()->GetName().Contains(TEXT("Ultra_Dynamic_Weather")))
			{
				return *It;
			}
		}
		return nullptr;
	}

	FWeatherParticleBrightness DefaultParticleBrightness(EDayTimeType DayTimeType)
	{
		FWeatherParticleBrightness Result;
		switch (DayTimeType)
		{
		case EDayTimeType::Night:
			Result.ParticleLightIntensity = 30.0f;
			break;
		case EDayTimeType::Dawn:
		case EDayTimeType::Dusk:
			Result.ParticleLightIntensity = 15.0f;
			break;
		default:
			break;
		}
		return Result;
	}
}

UWeatherSubsystem* UWeatherSubsystem::GetInstance(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;
	
	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;
	
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance) return nullptr;
	
	return GameInstance->GetSubsystem<UWeatherSubsystem>();
}

void UWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>())
	{
		if (const UPXGameDataAsset* GameDataAsset = Settings->GameDataAsset.LoadSynchronous())
		{
			DayTimeEffectSight = GameDataAsset->DayTimeEffect.LoadSynchronous();

		}
	}
}

void UWeatherSubsystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ParticleBrightnessTimerHandle);
	}
	bParticleTimerStarted = false;
	Super::Deinitialize();
}

void UWeatherSubsystem::ApplyParticleBrightness()
{
	// 纯打光方案（用户裁决）：UDW/UDS 的 MPC 与蓝图变量一律不写，只驱动跟随相机的点光
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APXGameState* GS = UPXGameplayStatics::GetGameState(World);
	if (!GS)
	{
		return;
	}

	FWeatherParticleBrightness Brightness = DefaultParticleBrightness(GS->DayTimeType);
	if (DayTimeEffectSight)
	{
		if (const FWeatherParticleBrightness* Configured = DayTimeEffectSight->ParticleBrightness.Find(GS->DayTimeType))
		{
			Brightness = *Configured;
		}
	}
	if (bParticleBrightnessOverride)
	{
		Brightness = ParticleBrightnessOverride;
	}

	UpdateParticleViewLight(Brightness);
	if (!bParticleTimerStarted)
	{
		bParticleTimerStarted = true;
		World->GetTimerManager().SetTimer(ParticleBrightnessTimerHandle,
			FTimerDelegate::CreateUObject(this, &UWeatherSubsystem::ApplyParticleBrightness), 0.5f, true);
	}
}

void UWeatherSubsystem::ApplyParticleMaterialOverride()
{
	// 打光方案裁决后默认停用；打开 bParticleMaterialRedirectEnabled 才启用备用路径
	if (!bParticleMaterialRedirectEnabled)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APXGameState* GS = UPXGameplayStatics::GetGameState(World);
	if (!GS)
	{
		return;
	}

	if (!bParticleSystemRedirectInitialized)
	{
		bParticleSystemRedirectInitialized = true;
		RainParticleUnlit = LoadObject<UNiagaraSystem>(nullptr, RainParticleUnlitPath);
		SnowParticleUnlit = LoadObject<UNiagaraSystem>(nullptr, SnowParticleUnlitPath);
		RainMaterialUnlit = LoadObject<UMaterialInterface>(nullptr, RainMaterialUnlitPath);
		SnowMaterialUnlit = LoadObject<UMaterialInterface>(nullptr, SnowMaterialUnlitPath);
		RainMaterialOriginalLit = LoadObject<UMaterialInterface>(nullptr, RainMaterialOriginalPath);
		SnowMaterialOriginalLit = LoadObject<UMaterialInterface>(nullptr, SnowMaterialOriginalPath);
		if (!RainParticleUnlit || !SnowParticleUnlit)
		{
			UE_LOG(LogTemp, Warning, TEXT("[ParticleBrightness] 未找到 /Game/Weather/Particles/Rain_PX|Snow_PX 复制体，暗档自发光粒子不生效（渲染器材质需指向 M_WeatherRain/Snow_Unlit）"));
		}
	}

	// 白天档且从未接管过时无事可做
	const bool bDark = GS->DayTimeType != EDayTimeType::Light;
	if (!bDark && !RainParticleOriginal && !SnowParticleOriginal)
	{
		return;
	}

	AActor* UDW = FindUDWActor(World);
	if (!UDW)
	{
		return;
	}

	auto RedirectSystem = [&](UNiagaraComponent* Component, FName UserParamName, const TObjectPtr<UNiagaraSystem>& UnlitSystem,
		TObjectPtr<UNiagaraSystem>& OutOriginal, UMaterialInterface* UnlitMat, UMaterialInterface* OriginalLitMat)
	{
		if (bDark && UnlitSystem)
		{
			if (!OutOriginal)
			{
				OutOriginal = Component->GetAsset();
			}
			if (Component->GetAsset() != UnlitSystem.Get())
			{
				Component->SetAsset(UnlitSystem.Get());
			}
			// 渲染器材质绑在 USER 参数上，UDW 运行时会用它覆盖资产默认值，必须同步驱动
			if (UnlitMat)
			{
				Component->SetVariableMaterial(UserParamName, UnlitMat);
			}
		}
		else if (!bDark && OutOriginal)
		{
			if (Component->GetAsset() != OutOriginal.Get())
			{
				Component->SetAsset(OutOriginal.Get());
			}
			if (OriginalLitMat)
			{
				Component->SetVariableMaterial(UserParamName, OriginalLitMat);
			}
		}
	};

	TArray<UNiagaraComponent*> NiagaraComponents;
	UDW->GetComponents<UNiagaraComponent>(NiagaraComponents);
	for (UNiagaraComponent* NiagaraComp : NiagaraComponents)
	{
		if (!NiagaraComp)
		{
			continue;
		}
		UNiagaraSystem* Sys = NiagaraComp->GetAsset();
		const FString SysName = Sys ? Sys->GetName() : FString();
		if (SysName.Contains(TEXT("Rain")))
		{
			RedirectSystem(NiagaraComp, FName(TEXT("Rain Sprite Material")), RainParticleUnlit,
				RainParticleOriginal, RainMaterialUnlit, RainMaterialOriginalLit);
		}
		else if (SysName.Contains(TEXT("Snow")))
		{
			RedirectSystem(NiagaraComp, FName(TEXT("Snow Sprite Material")), SnowParticleUnlit,
				SnowParticleOriginal, SnowMaterialUnlit, SnowMaterialOriginalLit);
		}
	}
}

void UWeatherSubsystem::UpdateParticleViewLight(const FWeatherParticleBrightness& Brightness)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APXGameState* GS = UPXGameplayStatics::GetGameState(World);
	if (!GS)
	{
		return;
	}

	// 只在暗档 × 雨/雪天气点亮；天气/时段切换有 0.5s 重刷兜底
	const bool bWeatherMatches = WeatherNameString.Contains(TEXT("Rain")) || WeatherNameString.Contains(TEXT("Snow"));
	float TargetIntensity = 0.f;
	if (bWeatherMatches && GS->DayTimeType != EDayTimeType::Light)
	{
		TargetIntensity = Brightness.ParticleLightIntensity;
	}

	APointLight* Light = ParticleViewLight.Get();
	if (TargetIntensity <= 0.f)
	{
		// 关灯 = 强度归零（不切显隐，避免多余的渲染状态切换）
		if (Light && Light->PointLightComponent)
		{
			Light->PointLightComponent->SetIntensity(0.f);
		}
		return;
	}

	if (!Light)
	{
		Light = World->SpawnActor<APointLight>(APointLight::StaticClass(), FTransform::Identity);
		if (!Light)
		{
			return;
		}
		Light->SetMobility(EComponentMobility::Movable);
		if (Light->PointLightComponent)
		{
			Light->PointLightComponent->SetCastShadows(false);
		}
		ParticleViewLight = Light;
	}

	if (Light->PointLightComponent)
	{
		Light->PointLightComponent->SetLightColor(Brightness.ParticleLightColor);
		Light->PointLightComponent->SetAttenuationRadius(Brightness.ParticleLightRadius);
		Light->PointLightComponent->SetIntensity(TargetIntensity);
	}
	if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(World, 0))
	{
		Light->SetActorLocation(CameraManager->GetCameraLocation());
	}
}

bool UWeatherSubsystem::GetParticleLightState(float& OutIntensity, float& OutRadius) const
{
	const APointLight* Light = ParticleViewLight.Get();
	if (!Light || !Light->PointLightComponent)
	{
		OutIntensity = 0.f;
		OutRadius = 0.f;
		return false;
	}
	OutIntensity = Light->PointLightComponent->Intensity;
	OutRadius = Light->PointLightComponent->AttenuationRadius;
	return OutIntensity > 0.f;
}

void UWeatherSubsystem::OverrideParticleBrightness(float CameraLight)
{
	bParticleBrightnessOverride = CameraLight > 0.f;
	if (!bParticleBrightnessOverride)
	{
		UE_LOG(LogTemp, Log, TEXT("[ParticleBrightness] 覆盖已清空，恢复时段分档"));
		ApplyParticleBrightness();
		return;
	}

	ParticleBrightnessOverride = FWeatherParticleBrightness();
	ParticleBrightnessOverride.ParticleLightIntensity = CameraLight;
	ApplyParticleBrightness();
}

void UWeatherSubsystem::MakeWeatherEffect()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);
	APXGameState* GS = UPXGameplayStatics::GetGameState(World);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GS);
	
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings);

	const UPXGameDataAsset* GameDataAsset = Settings->GameDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameDataAsset);

	// 与 WeatherExposureOffset 同一钩子；必须早于 FoundActors 空表早退
	ApplyParticleBrightness();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePXCharacter::StaticClass(), FoundActors);
	TArray<AActor*> FoundActors2;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), FoundActors2);

	FoundActors += FoundActors2;
	
	if (FoundActors.IsEmpty()) return;

	FAttributeEffect DayTimeEffect;
	FText DayTimeName = DayTimeEffectSight->DayTime2Name.FindRef(GS->DayTimeType);
	// 时分影响
	if (GS->DayTimeType == EDayTimeType::Dawn)
	{
		DayTimeEffect = DayTimeEffectSight->DawnEffect;
	}
	else if (GS->DayTimeType == EDayTimeType::Light)
	{
		DayTimeEffect = DayTimeEffectSight->LightEffect;
	}
	else if (GS->DayTimeType == EDayTimeType::Dusk)
	{
		DayTimeEffect = DayTimeEffectSight->DuskEffect;
	}
	else if (GS->DayTimeType == EDayTimeType::Night)
	{
		DayTimeEffect = DayTimeEffectSight->NightEffect;
	}
	
	for (auto& Actor : FoundActors)
	{
		UBuffComponent* BuffComp = Actor->GetComponentByClass<UBuffComponent>();
		if (!BuffComp) continue;

		// 时分影响
		if (DayTimeNameString != DayTimeName.ToString())
		{
			DayTimeNameString = DayTimeName.ToString();
			BuffComp->RemoveBuff(DayTimeTag, true);
			FAttributeEffect Effect = FAttributeEffect( EPXAttribute::CurSight, DayTimeEffect.EffectedPercent, DayTimeEffect.EffectedValue);
			
			BuffComp->AddAttributeEffect(DayTimeTag, Effect);
			BuffComp->AddBuffOnWidget(DayTimeTag, DayTimeNameString, FLinearColor::White, false);
		}

		// 天气影响
		if (!GS->WeatherName.IsEmpty() && WeatherNameString != GS->WeatherName.ToString())
		{
			WeatherNameString = GS->WeatherName.ToString();
			BuffComp->RemoveBuff(WeatherTag, true);
			
			FAttributeEffect EffectSight = FAttributeEffect(EPXAttribute::CurSight, GS->WeatherEffect.SightPercent);
			BuffComp->AddAttributeEffect(WeatherTag, EffectSight);

			FAttributeEffect EffectSpeed = FAttributeEffect(EPXAttribute::CurSpeed, GS->WeatherEffect.MoveSpeedEffectPercent);
			BuffComp->AddAttributeEffect(WeatherTag, EffectSpeed);
			
			BuffComp->AddBuffOnWidget(WeatherTag, WeatherNameString, FLinearColor::White, false);
		}
		
		// 针对玩家的血量影响
		if (Cast<ABasePXCharacter>(Actor) && GameDataAsset->GE_WeatherEffect_HP)
		{
			if (const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor))
			{
				if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
				{
					ASC->RemoveActiveGameplayEffectBySourceEffect(GameDataAsset->GE_WeatherEffect_HP, nullptr);
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GameDataAsset->GE_WeatherEffect_HP, 1, ASC->MakeEffectContext());
					if (FGameplayEffectSpec* Spec = SpecHandle.Data.Get())
					{
						Spec->SetSetByCallerMagnitude(TAG("Buff.Weather.HPEffectPerSeconds"), GS->WeatherEffect.HPEffectPerSecond);
						ASC->ApplyGameplayEffectSpecToSelf(*Spec);
					}
				}
			}
		}
	}
}
