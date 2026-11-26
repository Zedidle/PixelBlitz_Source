// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/WeatherSubsystem.h"
#include "Core/PXGameState.h"
#include "Character/BasePXCharacter.h"
#include "Character/Components/BuffComponent.h"
#include "Enemy/BaseEnemy.h"
#include "GAS/PXGameplayEffect.h"
#include "Kismet/GameplayStatics.h"
#include "KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXGameDataAsset.h"
#include "Utilitys/PXGameplayStatics.h"


namespace
{
	const TCHAR* WeatherParticleCollectionPath = TEXT("/Game/UltraDynamicSky/Particles/UDW_WeatherParticle_Parameters.UDW_WeatherParticle_Parameters");

	FWeatherParticleBrightness DefaultParticleBrightness(EDayTimeType DayTimeType)
	{
		FWeatherParticleBrightness Result;
		// Manual EV 下 emissive 断裂：只抬亮度项；Alpha 维持 1 避免过曝/额外混合成本
		switch (DayTimeType)
		{
		case EDayTimeType::Night:
			Result.RainAmbientLightIntensity = 3.0f;
			Result.SnowAmbientLightIntensity = 3.0f;
			break;
		case EDayTimeType::Dawn:
		case EDayTimeType::Dusk:
			Result.RainAmbientLightIntensity = 1.5f;
			Result.SnowAmbientLightIntensity = 1.5f;
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

	WeatherParticleCollection = LoadObject<UMaterialParameterCollection>(nullptr, WeatherParticleCollectionPath);
	if (!WeatherParticleCollection)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ParticleBrightness] 未找到 UDW 粒子 MPC，暗档粒子补偿不生效"));
	}
}
void UWeatherSubsystem::ApplyParticleBrightness()
{
	UWorld* World = GetWorld();
	if (!World || !WeatherParticleCollection)
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

	UKismetMaterialLibrary::SetScalarParameterValue(World, WeatherParticleCollection, FName(TEXT("Rain Sprite Alpha")), Brightness.RainSpriteAlpha);
	UKismetMaterialLibrary::SetScalarParameterValue(World, WeatherParticleCollection, FName(TEXT("Rain Ambient Light Intensity")), Brightness.RainAmbientLightIntensity);
	UKismetMaterialLibrary::SetScalarParameterValue(World, WeatherParticleCollection, FName(TEXT("Snow Sprite Alpha")), Brightness.SnowSpriteAlpha);
	UKismetMaterialLibrary::SetScalarParameterValue(World, WeatherParticleCollection, FName(TEXT("Snow Ambient Light Intensity")), Brightness.SnowAmbientLightIntensity);
}

void UWeatherSubsystem::OverrideParticleBrightness(float RainAmbient, float SnowAmbient, float RainAlpha, float SnowAlpha)
{
	bParticleBrightnessOverride = RainAmbient > 0.f || SnowAmbient > 0.f || RainAlpha > 0.f || SnowAlpha > 0.f;
	if (!bParticleBrightnessOverride)
	{
		UE_LOG(LogTemp, Log, TEXT("[ParticleBrightness] 覆盖已清空，恢复时段分档"));
		ApplyParticleBrightness();
		return;
	}

	ParticleBrightnessOverride = FWeatherParticleBrightness();
	if (RainAmbient > 0.f) ParticleBrightnessOverride.RainAmbientLightIntensity = RainAmbient;
	if (SnowAmbient > 0.f) ParticleBrightnessOverride.SnowAmbientLightIntensity = SnowAmbient;
	if (RainAlpha > 0.f) ParticleBrightnessOverride.RainSpriteAlpha = RainAlpha;
	if (SnowAlpha > 0.f) ParticleBrightnessOverride.SnowSpriteAlpha = SnowAlpha;
	ApplyParticleBrightness();
}

bool UWeatherSubsystem::GetParticleParameterValues(float& OutRainAmbient, float& OutSnowAmbient, float& OutRainAlpha, float& OutSnowAlpha) const
{
	UWorld* World = GetWorld();
	if (!World || !WeatherParticleCollection)
	{
		return false;
	}

	OutRainAmbient = UKismetMaterialLibrary::GetScalarParameterValue(World, WeatherParticleCollection, FName(TEXT("Rain Ambient Light Intensity")));
	OutSnowAmbient = UKismetMaterialLibrary::GetScalarParameterValue(World, WeatherParticleCollection, FName(TEXT("Snow Ambient Light Intensity")));
	OutRainAlpha = UKismetMaterialLibrary::GetScalarParameterValue(World, WeatherParticleCollection, FName(TEXT("Rain Sprite Alpha")));
	OutSnowAlpha = UKismetMaterialLibrary::GetScalarParameterValue(World, WeatherParticleCollection, FName(TEXT("Snow Sprite Alpha")));
	return true;
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
