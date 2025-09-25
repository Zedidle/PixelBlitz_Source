// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/WeatherSubsystem.h"
#include "Core/PXGameState.h"
#include "Character/BasePXCharacter.h"
#include "Character/Components/BuffComponent.h"
#include "Enemy/BaseEnemy.h"
#include "GAS/PXGameplayEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/Config/CustomConfigSettings.h"
#include "Utilitys/PXGameplayStatics.h"


void UWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UCustomConfigSettings* ConfigSettings = GetDefault<UCustomConfigSettings>();
	if (ConfigSettings)
	{
		DayTimeEffectSight = ConfigSettings->DayTimeEffect.LoadSynchronous();
	}
	
}
void UWeatherSubsystem::MakeWeatherEffect()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);
	APXGameState* GS = UPXGameplayStatics::GetGameState(World);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GS);
	const UCustomConfigSettings* ConfigSettings = GetDefault<UCustomConfigSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ConfigSettings);
	// CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DayTimeEffectSight);
	
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABasePXCharacter::StaticClass(), FoundActors);
	TArray<AActor*> FoundActors2;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), FoundActors2);

	FoundActors += FoundActors2;
	
	if (FoundActors.IsEmpty()) return;

	FGameplayTag DayTimeTag = FGameplayTag::RequestGameplayTag("Buff.DayTime");
	FGameplayTag WeatherTag = FGameplayTag::RequestGameplayTag("Buff.Weather");
	FBuffValueEffect DayTimeEffect;

	for (auto& Actor : FoundActors)
	{
		UBuffComponent* BuffComp = Actor->GetComponentByClass<UBuffComponent>();
		if (!BuffComp) continue;

		// 时分影响
		IBuff_Interface::Execute_RemoveBuff(BuffComp, DayTimeTag, false);
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
		
		if (DayTimeEffectSight->DayTimeType2BuffTag.Contains(GS->DayTimeType))
		{
			BuffComp->AddBuffByTag(DayTimeEffectSight->DayTimeType2BuffTag[GS->DayTimeType]);
		}
		IBuff_Interface::Execute_BuffEffect_Sight(BuffComp, DayTimeTag, DayTimeEffect.EffectedPercent, DayTimeEffect.EffectedValue, 9999);


		// 天气影响
		IBuff_Interface::Execute_RemoveBuff(BuffComp, WeatherTag, true);
		if (GS->WeatherName_Localized.IsValid())
		{
			IBuff_Interface::Execute_AddBuff(BuffComp, WeatherTag,
				ULocalizationFuncLib::GetLocalizedString(GS->WeatherName_Localized), FLinearColor::White, false);
		}

		IBuff_Interface::Execute_BuffEffect_Sight(BuffComp, WeatherTag, GS->WeatherEffect.SightDistancePercent, 0, 9999);
		IBuff_Interface::Execute_BuffEffect_Speed(BuffComp, WeatherTag, GS->WeatherEffect.MoveSpeedEffectPercent, 0, 9999);
		
		
		// 针对玩家的血量影响
		if (Cast<ABasePXCharacter>(Actor) && ConfigSettings->GE_WeatherEffect_HP)
		{
			if (const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Actor))
			{
				if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
				{
					ASC->RemoveActiveGameplayEffectBySourceEffect(ConfigSettings->GE_WeatherEffect_HP, nullptr);
					FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ConfigSettings->GE_WeatherEffect_HP, 1, ASC->MakeEffectContext());
					if (FGameplayEffectSpec* Spec = SpecHandle.Data.Get())
					{
						Spec->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Buff.Weather.HPEffectPerSeconds"), GS->WeatherEffect.HPEffectPerSecond);
						ASC->ApplyGameplayEffectSpecToSelf(*Spec);
					}
				}
			}
		}
	}


}
