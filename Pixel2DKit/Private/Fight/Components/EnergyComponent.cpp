// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Components/EnergyComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Core/PXGameState.h"
#include "Fight/Components/HealthComponent.h"
#include "GAS/AttributeSet/PXAttributeSet.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/PXGameplayStatics.h"

#define LOCTEXT_NAMESPACE "PX"

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

	PXCharacter = Cast<ABasePXCharacter>(GetOwner());

	
	
	
}


// Called every frame
void UEnergyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int32 UEnergyComponent::CalEPCustomInFact(int32 Amount)
{
	APXGameState* GS = UPXGameplayStatics::GetGameState(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GS, Amount)
	
	return FMath::RoundToInt(Amount * (GS->GetEPConsumePlusPercent() + 1.0f));
}

bool UEnergyComponent::DecreaseEP(int32 Amount)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(PXCharacter, false)
	APXGameState* GS = UPXGameplayStatics::GetGameState(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GS, false)
	
	int32 Consume = CalEPCustomInFact(Amount);

	if (!IsEPEnough(Consume, true)) return false;

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PXCharacter))
	{
		if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
		{
			float CurValue = PixelAS->GetEP();
			
			if (CurValue >= Consume)
			{
				CurValue -= Consume;
				TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetEPAttribute(), CurValue);
				return true;
			}
		}
	}

	return false;
}

void UEnergyComponent::IncreaseEP(int32 Amount)
{
	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
			{
				float NewValue = FMath::Min(PixelAS->GetEP() + Amount, PixelAS->GetMaxEP());
				TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetEPAttribute(), NewValue);
			}
		}
	}
}

void UEnergyComponent::SetEP(int32 NewValue)
{
	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetEPAttribute(), NewValue);
		}
	}
}

int32 UEnergyComponent::GetCurrentEP()
{
	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
			{
				return PixelAS->GetEP();
			}
		}
	}
	return 0;
}

int32 UEnergyComponent::GetMaxEP()
{
	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
			{
				return PixelAS->GetMaxEP();
			}
		}
	}
	return 100;
}

void UEnergyComponent::SetMaxEP(int32 NewValue)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
	if (!IsValid(TargetASC)) return;
	const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>();
	if (!IsValid(PixelAS)) return;

	NewValue = FMath::Max(NewValue, 0);
	TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetMaxEPAttribute(), NewValue);
	TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetEPAttribute(), NewValue);
}

bool UEnergyComponent::IsEPEnough(int32 Cost, bool bNeedTip)
{
	if (!PXCharacter) return false;
	
	Cost = CalEPCustomInFact(Cost);

	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
			{
				return PixelAS->GetEP() >= Cost;
			}
		}
	}

	if (bNeedTip)
	{
		UCommonFuncLib::SpawnCenterTip(LOCTEXT("EP_NOT_ENOUGH", "体力不足"),
			FLinearColor::White, FVector2D(0, 300));
	}

	return false;
}

#undef LOCTEXT_NAMESPACE