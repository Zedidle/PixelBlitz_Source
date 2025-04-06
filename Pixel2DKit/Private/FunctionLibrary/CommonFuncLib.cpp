// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary/CommonFuncLib.h"

#include "NiagaraMeshRendererProperties.h"
#include "NiagaraRibbonRendererProperties.h"
#include "StaticMeshAttributes.h"
#include "VisualizeTexture.h"
#include "Settings/UserWidgetSettings.h"
#include "UserWidget/UWBaseFloatingText.h"
#include "FunctionLibrary/LocalizationFuncLib.h"


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
	if (IsValid(World) && Settings && Settings->FloatingText_Default)
	{
		if (UUWBaseFloatingText* Widget = CreateWidget<UUWBaseFloatingText>(World, Settings->FloatingText_Default))
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
	UWorld* World = GEngine->GetCurrentPlayWorld();
	if (IsValid(World) && Settings && Settings->FloatingText_Default)
	{
		if (UUWBaseFloatingText* Widget = CreateWidget<UUWBaseFloatingText>(World, Settings->FloatingText_Default))
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
	const FLocationTableData D = FLocationTableData(InTableName, InRowName);
	FText Text = FText::FromString(ULocalizationFuncLib::GetLocalizedString(D));
	SpawnFloatingText(Text, WorldLocation, TextColor, RenderScale, Icon);
}
