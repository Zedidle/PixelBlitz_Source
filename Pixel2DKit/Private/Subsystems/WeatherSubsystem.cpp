// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/WeatherSubsystem.h"
#include "Core/PXGameState.h"
#include "Character/BasePXCharacter.h"
#include "Character/Components/BuffComponent.h"
#include "Enemy/BaseEnemy.h"
#include "GAS/PXGameplayEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXGameDataAsset.h"
#include "Utilitys/PXGameplayStatics.h"


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
			BuffComp->AddAttributeEffect(DayTimeTag,
				FAttributeEffect( EPXAttribute::CurSight, DayTimeEffect.EffectedPercent, DayTimeEffect.EffectedValue));
			BuffComp->AddBuffOnWidget(DayTimeTag, DayTimeNameString, FLinearColor::White, false);
		}

		// 天气影响
		if (!GS->WeatherName.IsEmpty() && WeatherNameString != GS->WeatherName.ToString())
		{
			WeatherNameString = GS->WeatherName.ToString();
			BuffComp->RemoveBuff(WeatherTag, true);
			BuffComp->AddAttributeEffect(WeatherTag,
				FAttributeEffect(EPXAttribute::CurSight, GS->WeatherEffect.SightPercent));
			BuffComp->AddAttributeEffect(WeatherTag,
				FAttributeEffect(EPXAttribute::CurSpeed, GS->WeatherEffect.MoveSpeedEffectPercent));
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
