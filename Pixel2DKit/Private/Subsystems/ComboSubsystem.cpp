// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ComboSubsystem.h"

#include "Pixel2DKit.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXResourceDataAsset.h"
#include "Settings/Config/PXWidgetsDataAsset.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "Utilitys/DebugFuncLab.h"
#include "Utilitys/SoundFuncLib.h"
#include "Utilitys/UserWidgetFuncLib.h"

float UComboSubsystem::GetDamagePlusPercent()
{
	return 0.17 + CurComboWeakPoint * 0.03;
}

void UComboSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UComboSubsystem::Deinitialize()
{
	Super::Deinitialize();
	if (ComboWeakPointCountWidget)
	{
		ComboWeakPointCountWidget->RemoveFromRoot();
	}
}

int UComboSubsystem::CalDamageByComboWeakPoint(int InitDamage)
{
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(Settings, InitDamage)

	const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(ResourceDataAsset, InitDamage)
	if (ResourceDataAsset->SW_ComboWeakPoints.IsValidIndex(CurComboWeakPoint))
	{
		if (USoundBase* Sound = ResourceDataAsset->SW_ComboWeakPoints[CurComboWeakPoint].LoadSynchronous())
		{
			USoundFuncLib::PlaySound2D(Sound);			
		}
	}
	else
	{
		USoundFuncLib::PlaySound2D(ResourceDataAsset->SW_ComboWeakPoints.Last().LoadSynchronous());	
	}

	if (!ComboWeakPointCountWidget)
	{
		if (const UPXWidgetsDataAsset* WidgetsDataAsset = Settings->WidgetsDataAsset.LoadSynchronous())
		{
			if (WidgetsDataAsset->ComboWeakPointCountWidget)
			{
				ComboWeakPointCountWidget = CreateWidget<UComboWeakPointCountWidget>(GetWorld(), WidgetsDataAsset->ComboWeakPointCountWidget);
				ComboWeakPointCountWidget->AddToViewport(100);
			}
		}
	}
	CurComboWeakPoint = FMath::Min(CurComboWeakPoint + 1, MaxComboWeakPoint);

	ComboWeakPointCountWidget->UpdateCount(CurComboWeakPoint, GetDamagePlusPercent() * 100);
	
	UTimerSubsystemFuncLib::SetDelayLoop(this, "UComboSubsystem::CalDamageByComboWeakPoint",
		[WeakThis = TWeakObjectPtr(this)]
		{
			if (!WeakThis.IsValid()) return;
			WeakThis->CurComboWeakPoint = FMath::Max(WeakThis->CurComboWeakPoint-1, 0);
			
			if (WeakThis->ComboWeakPointCountWidget)
			{
				if (WeakThis->CurComboWeakPoint == 0)
				{
					WeakThis->ComboWeakPointCountWidget->Fade(0.2);
				}
				else
				{
					WeakThis->ComboWeakPointCountWidget->UpdateCount(WeakThis->CurComboWeakPoint, WeakThis->GetDamagePlusPercent() * 100);
				}

				if (WeakThis->CurComboWeakPoint == 0)
				{
					UTimerSubsystemFuncLib::CancelDelay(WeakThis.Get(), "UComboSubsystem::CalDamageByComboWeakPoint");
				}
			}
		}, ComboWeakPointSustainTime);

	
	return InitDamage * (1 + GetDamagePlusPercent());
}
