// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/BasePlayerStatusWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Core/PXSaveGameSubsystem.h"
#include "Pixel2DKit/Pixel2DKit.h"

void UBasePlayerStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateHP();
	UpdateEP();

	if (Anim_ShowIn)
	{
		PlayAnimationForward(Anim_ShowIn, 1.0f,false);
	}

	if (HeartsWidget)
	{
		if (UPXMainSaveGame* MainSaveGame = GetGameInstance()->GetSubsystem<UPXSaveGameSubsystem>()->GetMainData())
		{
			HeartsWidget->SetMaxHeartsNum(MainSaveGame->MaxLife);
		}
	}
	RefreshLife();
	if (PXAttributeSet)
	{
		PXAttributeSet->OnPXAttributeChange.AddDynamic(this, &UBasePlayerStatusWidget::OnAttributeChanged);
	}
}

void UBasePlayerStatusWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (PXAttributeSet)
	{
		PXAttributeSet->OnPXAttributeChange.RemoveDynamic(this, &UBasePlayerStatusWidget::OnAttributeChanged);
	}
}

void UBasePlayerStatusWidget::Init(const UPXAttributeSet* _PXAttributeSet)
{
	PXAttributeSet = _PXAttributeSet;
}

void UBasePlayerStatusWidget::UpdateHP()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXAttributeSet)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ProgressBar_HP)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(TextBlock_HP)

	float HP = PXAttributeSet->GetHP();
	float MaxHP = PXAttributeSet->GetMaxHP();
	ProgressBar_HP->SetPercent(HP / MaxHP);

	TextBlock_HP->SetText(FText::FromString(FString::Printf( TEXT("%d / %d"), FMath::RoundToInt(HP), FMath::RoundToInt(MaxHP))));
}

void UBasePlayerStatusWidget::UpdateEP()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXAttributeSet)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ProgressBar_EP)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(TextBlock_EP)

	float EP = PXAttributeSet->GetHP();
	float MaxEP = PXAttributeSet->GetMaxHP();
	ProgressBar_EP->SetPercent(EP / MaxEP);

	TextBlock_EP->SetText(FText::FromString(FString::Printf( TEXT("%d / %d"), FMath::RoundToInt(EP), FMath::RoundToInt(MaxEP))));
}

void UBasePlayerStatusWidget::OnAttributeChanged(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == UPXAttributeSet::GetHPAttribute() || Attribute == UPXAttributeSet::GetMaxHPAttribute())
	{
		UpdateHP();
	}
	if (Attribute == UPXAttributeSet::GetEPAttribute() || Attribute == UPXAttributeSet::GetMaxEPAttribute())
	{
		UpdateEP();
	}
}

void UBasePlayerStatusWidget::RefreshLife_Implementation()
{
	UGameInstance* GameInstance = GetGameInstance();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance);

	UPXMainSaveGame* MainSaveGame = GameInstance->GetSubsystem<UPXSaveGameSubsystem>()->GetMainData();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(MainSaveGame);
	
	if (HeartsWidget)
	{
		HeartsWidget->FillHearts(MainSaveGame->SupLife);
	}
}


void UBasePlayerStatusWidget::UpdateDark_Implementation(float ViewWeekEffectPercent)
{
	
}


void UBasePlayerStatusWidget::SetViewEffect_Implementation(bool Show, FLinearColor Color, float LargeRadius,
                                                           float VelocitySpeed)
{
	
}
