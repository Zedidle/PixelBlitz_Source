// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilitys/SettingFuncLib.h"

#include "Kismet/KismetStringLibrary.h"

FIntPoint USettingFuncLib::GetScreenResolution(int Index)
{
	TArray<FText> Options =  {
		FText::FromString("1024x576"),
		FText::FromString("1280x720"),
		FText::FromString("1600x900"),
		FText::FromString("1920x1080"),
		FText::FromString("2560x1440"),
		FText::FromString("3840x2160")
	};

	if (Options.IsValidIndex(Index))
	{
		FString LeftS, RightS;
		UKismetStringLibrary::Split( Options[Index].ToString(), "x", LeftS, RightS, ESearchCase::IgnoreCase, ESearchDir::FromStart);
		return FIntPoint( FCString::Atoi(*LeftS), FCString::Atoi(*RightS));
	}

	return FIntPoint(1920, 1080);
}

TEnumAsByte<EWindowMode::Type> USettingFuncLib::GetWindowMode(int Index)
{
	TArray<TEnumAsByte<EWindowMode::Type>> Modes = {
		EWindowMode::WindowedFullscreen,
		EWindowMode::Windowed
	};
	if (Modes.IsValidIndex(Index))
	{
		return Modes[Index];
	}
	return EWindowMode::Type::NumWindowModes;
}

