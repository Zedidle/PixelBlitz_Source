// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/PXHUD.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Async/TaskGraphInterfaces.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "UObject/UObjectIterator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PXHUD)

class AActor;
class UWorld;

//////////////////////////////////////////////////////////////////////
// APXHUD

APXHUD::APXHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void APXHUD::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void APXHUD::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

void APXHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void APXHUD::GetDebugActorList(TArray<AActor*>& InOutList)
{
	UWorld* World = GetWorld();

	Super::GetDebugActorList(InOutList);

	// Add all actors with an ability system component.
	for (TObjectIterator<UAbilitySystemComponent> It; It; ++It)
	{
		if (UAbilitySystemComponent* ASC = *It)
		{
			if (!ASC->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
			{
				AActor* AvatarActor = ASC->GetAvatarActor();
				AActor* OwnerActor = ASC->GetOwnerActor();

				if (AvatarActor && UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AvatarActor))
				{
					AddActorToDebugList(AvatarActor, InOutList, World);
				}
				else if (OwnerActor && UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor))
				{
					AddActorToDebugList(OwnerActor, InOutList, World);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Mountea Dialogue HUD Interface

TSubclassOf<UUserWidget> APXHUD::GetViewportBaseClass_Implementation() const
{
	// 不使用 Viewport 容器层（见 PXHUD.h 顶部说明）。
	return nullptr;
}

void APXHUD::InitializeViewportWidget_Implementation()
{
	// 无 Viewport 容器，无需初始化。
}

UUserWidget* APXHUD::GetViewportWidget_Implementation() const
{
	return nullptr;
}

void APXHUD::AddChildWidgetToViewport_Implementation(UUserWidget* ChildWidget, const int32 ZOrder,
	const FAnchors WidgetAnchors, const FMargin& WidgetMargin)
{
	if (!ChildWidget)
	{
		return;
	}

	// 与插件自身 SetDialogueWidgetZOrder 使用同一套机制（per-player viewport slot），
	// 这样 UMounteaDialogueHUDStatics::GetWidgetZOrder 也能正确读到 ZOrder。
	APlayerController* PC = GetOwningPlayerController();
	ULocalPlayer* LocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;
	UGameViewportSubsystem* ViewportSubsystem = UGameViewportSubsystem::Get(GetWorld());

	if (!LocalPlayer || !ViewportSubsystem)
	{
		// 拿不到 Subsystem（极少见）时的兜底：直接挂到屏幕，避免对话 UI 完全不显示
		ChildWidget->AddToViewport(ZOrder);
		return;
	}

	FGameViewportWidgetSlot Slot;
	Slot.Anchors = WidgetAnchors;
	Slot.Offsets = WidgetMargin;
	Slot.ZOrder = ZOrder;

	ViewportSubsystem->AddWidgetForPlayer(ChildWidget, LocalPlayer, Slot);
}

void APXHUD::RemoveChildWidgetFromViewport_Implementation(UUserWidget* ChildWidget)
{
	if (!ChildWidget)
	{
		return;
	}

	if (UGameViewportSubsystem* ViewportSubsystem = UGameViewportSubsystem::Get(GetWorld()))
	{
		ViewportSubsystem->RemoveWidget(ChildWidget);
		return;
	}

	ChildWidget->RemoveFromParent();
}
