// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PXGameViewportClient.h"
#include "CommonUISettings.h"
#include "ICommonUIModule.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXGameViewportClient)

class UGameInstance;

namespace GameViewportTags
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_Input_HardwareCursor, "Platform.Trait.Input.HardwareCursor");
}

UPXGameViewportClient::UPXGameViewportClient(): Super(FObjectInitializer::Get())
{
}

void UPXGameViewportClient::Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance,
	bool bCreateNewAudioDevice)
{
	Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);
	
	const bool UseHardwareCursor = ICommonUIModule::GetSettings().GetPlatformTraits().HasTag(GameViewportTags::TAG_Platform_Trait_Input_HardwareCursor);
	SetUseSoftwareCursorWidgets(!UseHardwareCursor);
}
