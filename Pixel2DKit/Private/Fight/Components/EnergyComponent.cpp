// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Components/EnergyComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Fight/Components/HealthComponent.h"
#include "GAS/AttributeSet/PXAttributeSet.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Utilitys/CommonFuncLib.h"

void UEnergyComponent::Recover()
{
}

// Sets default values for this component's properties
UEnergyComponent::UEnergyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnergyComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<ABasePXCharacter>(GetOwner());

	
	
	
}


// Called every frame
void UEnergyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UEnergyComponent::DecreaseEnergy(int32 Amount, AActor* Instigator)
{
	int32 Consume = FMath::RoundToInt(Amount * (PlayerCharacter->WeatherEffect.StaminaConsumePercent + 1.0f));

	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
			{
				float CurValue = PixelAS->GetEnergy();
				if (CurValue >= Consume)
				{
					CurValue -= Consume;
					TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetEnergyAttribute(), CurValue);
					OnEnergyChanged.Broadcast(CurValue, EStatChange::Increase, Instigator);
					return true;
				}
			}
		}
	}
	
	return false;
}

void UEnergyComponent::IncreaseEnergy(int32 Amount, AActor* Instigator)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Instigator)
	
	UCommonFuncLib::SpawnFloatingCombatText(EFloatingTextType::Energy, FText::AsNumber(Amount),
		nullptr, Instigator->GetActorLocation());

	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
			{
				float NewValue = FMath::Min(PixelAS->GetEnergy() + Amount, PixelAS->GetMaxEnergy());
				TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetEnergyAttribute(), NewValue);
				OnEnergyChanged.Broadcast(NewValue, EStatChange::Increase, Instigator);
			}
		}
	}
}

void UEnergyComponent::SetEnergy(int32 NewValue, AActor* Instigator)
{
	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetEnergyAttribute(), NewValue);
			OnEnergyChanged.Broadcast(NewValue, EStatChange::Reset, Instigator);
		}
	}
}

int32 UEnergyComponent::GetCurrentEnergy()
{
	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
			{
				return PixelAS->GetEnergy();
			}
		}
	}
	return 0;
}

int32 UEnergyComponent::GetMaxEnergy()
{
	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
			{
				return PixelAS->GetMaxEnergy();
			}
		}
	}
	return 100;
}

void UEnergyComponent::SetMaxEnergy(int32 NewValue)
{
	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetMaxEnergyAttribute(), NewValue);
		}
	}
}

bool UEnergyComponent::IsEnergyEnough(int32 Cost)
{
	if (!PlayerCharacter) return false;

	Cost = FMath::RoundToInt(Cost * (PlayerCharacter->WeatherEffect.StaminaConsumePercent + 1.0f));

	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
			{
				return PixelAS->GetEnergy() >= Cost;
			}
		}
	}
	
	return false;
}

