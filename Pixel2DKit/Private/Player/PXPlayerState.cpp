// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/PXPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Character/BasePXCharacter.h"
#include "Components/MounteaDialogueManager.h"
#include "GameFramework/PlayerController.h"
#include "Helpers/MounteaDialogueHUDStatics.h"
#include "Helpers/MounteaDialogueManagerStatics.h"
#include "Interfaces/Core/MounteaDialogueManagerInterface.h"
#include "UI/PXHUD.h"

APXPlayerState::APXPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MounteaDialogueManager = CreateDefaultSubobject<UMounteaDialogueManager>(TEXT("MounteaDialogueManager"));
	ensure(MounteaDialogueManager != nullptr);
}

void APXPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// 在 BeginPlay 绑定：此时 Manager 组件已 RegisterComponent，委托安全可绑。
	// ❗ OnDialogueUserInterfaceChanged 是 Manager 的 protected 成员，C++ 不能直接访问，
	//   必须经由 IMounteaDialogueManagerInterface 的 GetDialogueUserInterfaceChangedEventHandle()
	//   取到引用再绑定（该函数是接口 public 的虚函数，非静态）。
	//   签名：FDialogueUserInterfaceChanged(TSubclassOf<UUserWidget>, UUserWidget*)
	if (MounteaDialogueManager)
	{
		FDialogueUserInterfaceChanged& UIChangedEvent =
			MounteaDialogueManager->GetDialogueUserInterfaceChangedEventHandle();

		UIChangedEvent.AddDynamic(this, &APXPlayerState::HandleDialogueUserInterfaceChanged);
	}
}

void APXPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MounteaDialogueManager)
	{
		FDialogueUserInterfaceChanged& UIChangedEvent =
			MounteaDialogueManager->GetDialogueUserInterfaceChangedEventHandle();

		UIChangedEvent.RemoveDynamic(this, &APXPlayerState::HandleDialogueUserInterfaceChanged);
	}

	Super::EndPlay(EndPlayReason);
}

void APXPlayerState::HandleDialogueUserInterfaceChanged(TSubclassOf<UUserWidget> DialogueWidgetClass, UUserWidget* DialogueWidget)
{
	APlayerController* PC = GetPlayerController();
	if (!PC || !PC->IsLocalPlayerController())
	{
		return;
	}

	// HUD 才是 Mountea 的 ViewportManager：
	// AddChildWidgetToViewport 会检查它是否实现 IMounteaDialogueHUDClassInterface，
	// 未实现会直接报错返回，对话 UI 永远上不了屏。APXHUD 已实现该接口，
	// 且由 APXGameMode 构造函数指定为 HUDClass。
	APXHUD* PXHUD = Cast<APXHUD>(PC->GetHUD());
	if (!PXHUD)
	{
		return;
	}

	if (!DialogueWidget)
	{
		// Widget 为空 = Manager 已走 CloseDialogueUI。
		// ❗ 此时插件已经做过 MarkAsGarbage() + RemoveFromParent()，
		//   不要再拿旧指针去 RemoveChildWidgetFromViewport（对象已在销毁队列，属悬空访问）。
		//   这里只清引用即可。
		PickedUpDialogueWidget = nullptr;
		return;
	}

	// 防重：SetDialogueWidgetZOrder 也会广播本事件（传当前 DialogueWidget）。
	// 同一个 Widget 已经挂过 Viewport 就跳过，避免重复 AddChildWidget。
	if (PickedUpDialogueWidget == DialogueWidget)
	{
		return;
	}

	// 拿到 Manager 上配置的 ZOrder（UMounteaDialogueManager 构造里默认 12）
	const int32 ZOrder = UMounteaDialogueManagerStatics::GetDialogueWidgetZOrder(MounteaDialogueManager);

	FWidgetAdditionParams WidgetParams;
	WidgetParams.ZOrder = ZOrder;
	WidgetParams.Anchors = FAnchors(0.f, 0.f, 1.f, 1.f);
	WidgetParams.Margin = FMargin();

	UMounteaDialogueHUDStatics::AddChildWidgetToViewport(PXHUD, DialogueWidget, WidgetParams);

	// 记住指针，对话结束时用来摘除
	PickedUpDialogueWidget = DialogueWidget;
}
