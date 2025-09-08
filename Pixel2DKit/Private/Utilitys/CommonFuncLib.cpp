// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/CommonFuncLib.h"

#include "NiagaraMeshRendererProperties.h"
#include "NiagaraRibbonRendererProperties.h"
#include "StaticMeshAttributes.h"
#include "VisualizeTexture.h"
#include "Basic/PXPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/CustomResourceSettings.h"
#include "Settings/UserWidgetSettings.h"
#include "UI/Common/CenterWordTipWidget.h"
#include "UI/Common/BaseFloatingTextWidget.h"
#include "Utilitys/LocalizationFuncLib.h"
// #include "InputMappingContext.h"



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
	const UUserWidgetSettings* Settings = GetDefault<UUserWidgetSettings>();
	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (IsValid(World) && Settings && Settings->FloatingTextDefaultClass)
	{
		if (UBaseFloatingTextWidget* Widget = CreateWidget<UBaseFloatingTextWidget>(World, Settings->FloatingTextDefaultClass))
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
	const UUserWidgetSettings* Settings = GetDefault<UUserWidgetSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)
	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)
	if (Settings->FloatingTextDefaultClass)
	{
		if (UBaseFloatingTextWidget* Widget = CreateWidget<UBaseFloatingTextWidget>(World, Settings->FloatingTextDefaultClass))
		{
			Widget->Init(Text, TextColor, Icon, WorldLocation);
			Widget->SetRenderScale(RenderScale);
			Widget->AddToViewport();
		}
	}
}

void UCommonFuncLib::SpawnFloatingTextDefault(const FString InTableName, const FName InRowName, FVector WorldLocation,
	FLinearColor TextColor, FVector2D RenderScale, UPaperSprite* Icon)
{
	const FLocalizedTableData D = FLocalizedTableData(InTableName, InRowName);
	FText Text = FText::FromString(ULocalizationFuncLib::GetLocalizedString(D));
	SpawnFloatingText(Text, WorldLocation, TextColor, RenderScale, Icon);
}

void UCommonFuncLib::SpawnFloatingTextLocalized(FLocalizedTableData LocalizedTableData, FVector WorldLocation,
	FLinearColor TextColor, FVector2D RenderScale, UPaperSprite* Icon)
{
	FText Text = FText::FromString(ULocalizationFuncLib::GetLocalizedString(LocalizedTableData));
	SpawnFloatingText(Text, WorldLocation, TextColor, RenderScale, Icon);
}

float UCommonFuncLib::DealDeltaTime(float Seconds)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	return Seconds * UGameplayStatics::GetWorldDeltaSeconds(World) * 60;
}

void UCommonFuncLib::SpawnCenterTip(FText Tip, FLinearColor Color, FVector2D Translation, FVector2D Scale,
	float PlaybackSpeed, EFloatingTextAnimType AnimType)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	const UUserWidgetSettings* WidgetSettings = GetDefault<UUserWidgetSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(WidgetSettings)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(WidgetSettings->CenterWordTipClass)
	
	UCenterWordTipWidget* TipWidget = CreateWidget<UCenterWordTipWidget>(World, WidgetSettings->CenterWordTipClass);
	TipWidget->Init(Tip, Color, Translation, Scale, PlaybackSpeed, AnimType);
	TipWidget->AddToViewport(1000);
}

FKey UCommonFuncLib::GetActionKey(UInputAction* IA)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(World, FKey());
	if (APXPlayerController* Controller = Cast<APXPlayerController>(UGameplayStatics::GetPlayerController(World, 0)))
	{
		if (UInputMappingContext* IMC = Controller->GetCurrentIMC())
		{
			for (auto& ele : IMC->GetMappings())
			{
				if (ele.Action == IA)
				{
					return ele.Key;
				}
			}
		}
	}

	return FKey();
}

bool UCommonFuncLib::IsGamePadControlling()
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(World, false);
	
	APXPlayerController* Controller = Cast<APXPlayerController>(UGameplayStatics::GetPlayerController(World, 0));
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(Controller, false);

	return Controller->GamePadControlling;
	
}

void UCommonFuncLib::SetGamePadControlling(bool On)
{
	UWorld* World = GEngine->GetCurrentPlayWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World);
	
	APXPlayerController* Controller = Cast<APXPlayerController>(UGameplayStatics::GetPlayerController(World, 0));
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Controller);

	Controller->GamePadControlling = On;
}

bool UCommonFuncLib::CalRandMap_Float(const TMap<float, int32>& M, float& Result)
{
	return CalRandomMap(M, Result);
}

bool UCommonFuncLib::CalRandMap_Int(const TMap<int, int32>& M, int& Result)
{
	return CalRandomMap(M, Result);
}
