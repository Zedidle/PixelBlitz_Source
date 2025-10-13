// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/CommonFuncLib.h"
#include "Controller/PXPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
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

			if (TextType == EFloatingTextType::Health)
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
