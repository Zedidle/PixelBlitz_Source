// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/CommonFuncLib.h"
#include "Controller/PXPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "PixelBlitz/PixelBlitz.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXGameDataAsset.h"
#include "Settings/Config/PXWidgetsDataAsset.h"
#include "UI/UIManager.h"
#include "UI/Tips/BaseFloatingTextWidget.h"
#include "Utilitys/SpaceFuncLib.h"


class UCustomConfigSettings;

namespace FloatingTextColor
{
	FLinearColor DamageColor = FLinearColor(1, 0, 0, 1);
	FLinearColor HealthColor = FLinearColor(1, 0.127, 0.045, 1);
	FLinearColor EnergyColor = FLinearColor(0.162, 1, 0.157, 1);
	FLinearColor GoldColor = FLinearColor(1, 0.762, 0, 1);
	FLinearColor ExperienceColor = FLinearColor(0.138, 0.071, 1, 1);
	FLinearColor ItemColor = FLinearColor(0.85, 0.85, 0.85, 1);
	
}



void UCommonFuncLib::SpawnFloatingCombatText(EFloatingTextType TextType, FText Text, UPaperSprite* Icon,
	FVector WorldLocation, FVector2D Scale, FLinearColor ColorMulti)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)
	
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

	const UPXWidgetsDataAsset* WidgetsDataAsset = Settings->WidgetsDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(WidgetsDataAsset)
	
	if (WidgetsDataAsset->FloatingTextDefaultClass)
	{
		if (UBaseFloatingTextWidget* Widget = CreateWidget<UBaseFloatingTextWidget>(World, WidgetsDataAsset->FloatingTextDefaultClass))
		{
			if (TextType == EFloatingTextType::Damage)
			{
				Widget->Init(Text, FloatingTextColor::DamageColor * ColorMulti, Icon, WorldLocation);
			}

			if (TextType == EFloatingTextType::Heal)
			{
				Widget->Init(FText::FromString("+" + Text.ToString()), FloatingTextColor::HealthColor * ColorMulti, Icon, WorldLocation);
			}

			if (TextType == EFloatingTextType::Energy)
			{
				Widget->Init(FText::FromString("+" + Text.ToString()), FloatingTextColor::EnergyColor * ColorMulti, Icon, WorldLocation);
			}

			if (TextType == EFloatingTextType::Gold)
			{
				Widget->Init(FText::FromString("+" + Text.ToString()), FloatingTextColor::GoldColor * ColorMulti, Icon, WorldLocation);
			}

			if (TextType == EFloatingTextType::Experience)
			{
				Widget->Init(FText::FromString("+" + Text.ToString()), FloatingTextColor::ExperienceColor * ColorMulti, Icon, WorldLocation);
			}

			if (TextType == EFloatingTextType::Item)
			{
				Widget->Init(FText::FromString("+" + Text.ToString()), FloatingTextColor::ItemColor * ColorMulti, Icon, WorldLocation);
			}
		
			Widget->SetRenderScale(Scale);
			Widget->AddToViewport();
		}
	}
}

void UCommonFuncLib::SpawnFloatingText(FText Text, FVector WorldLocation, FLinearColor TextColor, FVector2D RenderScale, UPaperSprite* Icon)
{
	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)
	
	UPXWidgetsDataAsset* WidgetsDataAsset = Settings->WidgetsDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(WidgetsDataAsset)
	

	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	if (!USpaceFuncLib::IsPointInScreen(World, WorldLocation, -0.05)) return;
	
	if (WidgetsDataAsset->FloatingTextDefaultClass)
	{
		if (UBaseFloatingTextWidget* Widget = CreateWidget<UBaseFloatingTextWidget>(World, WidgetsDataAsset->FloatingTextDefaultClass))
		{
			Widget->Init(Text, TextColor, Icon, WorldLocation);
			Widget->SetRenderScale(RenderScale);
			Widget->AddToViewport();
		}
	}
}

float UCommonFuncLib::DealDeltaTime(float Seconds)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	return Seconds * UGameplayStatics::GetWorldDeltaSeconds(World) * 60;
}

void UCommonFuncLib::SpawnCenterTip(FText _Tip, FLinearColor _Color, FVector2D _Translation, FVector2D _Scale,
                                    float _PlaybackSpeed, EFloatingTextAnimType _AnimType)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)
	
	UPXWidgetsDataAsset* WidgetsDataAsset = Settings->WidgetsDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(WidgetsDataAsset)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(WidgetsDataAsset->CenterWordTipClass)
	
	UUIManager* UIManager = UUIManager::GetSelfInstance(World);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(UIManager)

	UIManager->PopupTip(_Tip.ToString());
}


bool UCommonFuncLib::CalRandMap_Float(const TMap<float, int32>& M, float& Result)
{
	return CalRandomMap(M, Result);
}

bool UCommonFuncLib::CalRandMap_Int(const TMap<int, int32>& M, int& Result)
{
	return CalRandomMap(M, Result);
}

bool UCommonFuncLib::CalRandMap_Vector2D(const TMap<FVector2D, int32>& M, FVector2D& Result)
{
	return CalRandomMap(M, Result);
}

bool UCommonFuncLib::CalRandMap_ChineseHour(const TMap<EChineseHour, int32>& M, EChineseHour& Result)
{
	return CalRandomMap(M, Result);
}

FVector2D UCommonFuncLib::GetChineseHourRange(EChineseHour Hour, bool bRandomRange)
{
	// 子时 23:00-01:00 跨零点，拆成 [2300,2400) / [0000,0100) 两段
	if (Hour == EChineseHour::Zi)
	{
		const bool bLateNight = bRandomRange ? FMath::RandBool() : true;
		return bLateNight ? FVector2D(2300.f, 2400.f) : FVector2D(0.f, 100.f);
	}

	// 丑时(index 1)=01:00 起，每加一个时辰 +2h
	const float StartHour = static_cast<float>(static_cast<int32>(Hour) * 2 - 1);
	return FVector2D(StartHour * 100.f, (StartHour + 2.f) * 100.f);
}

float UCommonFuncLib::ChineseHourToTime(EChineseHour Hour, bool bRandom)
{
	const FVector2D Range = GetChineseHourRange(Hour, bRandom);
	return bRandom ? FMath::FRandRange(Range.X, Range.Y) : (Range.X + Range.Y) * 0.5f;
}

float UCommonFuncLib::CalRandTimeByChineseHourRate(const TMap<EChineseHour, int32>& M, bool& bSuccess)
{
	EChineseHour Hour = EChineseHour::Zi;
	bSuccess = CalRandomMap(M, Hour);
	return bSuccess ? ChineseHourToTime(Hour, true) : 0.f;
}

float UCommonFuncLib::CalEndTime(float Duration)
{
	if (!GEngine)
	{
		return Duration; // 返回一个默认值
	}
	
	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (!World)
	{
		return Duration; // 返回一个默认值
	}
	
	return World->GetTimeSeconds() + Duration;
}

