// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "Interfaces/HUD/MounteaDialogueHUDClassInterface.h"
#include "PXHUD.generated.h"

namespace EEndPlayReason { enum Type : int; }

class AActor;
class UObject;
class UUserWidget;

/**
 * 项目的 HUD。
 *
 * 除了 GAS 调试列表，它同时充当 Mountea Dialogue System 的 **ViewportManager**：
 * Mountea 的 UMounteaDialogueHUDStatics::AddChildWidgetToViewport 会检查传入的
 * AActor 是否实现 IMounteaDialogueHUDClassInterface，未实现则直接报错返回。
 * 因此这里实现该接口，把对话 Widget 挂到屏幕。
 *
 * ❗ 不再使用插件自带的 WBP_Viewport 容器层（官方示例 BP_HUDClass 的做法）。
 *   理由：插件 C++ 源码从不调用本接口的任何函数（5 个函数只有 BlueprintCallable
 *   的 static 包装定义，零调用点），WBP_Viewport 内部也只是一个 CanvasPanel +
 *   一个写而不读的 AllWidgets 数组，属于官方示例自带的中间层。
 *   且插件的 SetDialogueWidgetZOrder 走的是 UGameViewportSubsystem::AddWidgetForPlayer
 *   （直接挂 viewport 根），与 WBP_Viewport 那套 CanvasPanel 路径互不兼容。
 *   这里统一用 UGameViewportSubsystem，与插件自身的 ZOrder 机制保持一致。
 *
 * 调用链：
 *   Manager(PlayerState) 广播 OnDialogueUserInterfaceChanged
 *     → APXPlayerState 接到 → GetHUD() 拿到本 HUD → 调本类的 AddChildWidgetToViewport
 */
UCLASS(Config = Game)
class PIXEL2DKIT_API APXHUD : public AHUD, public IMounteaDialogueHUDClassInterface
{
	GENERATED_BODY()

public:
	APXHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	//~UObject interface
	virtual void PreInitializeComponents() override;
	//~End of UObject interface

	//~AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~AHUD interface
	virtual void GetDebugActorList(TArray<AActor*>& InOutList) override;

#pragma region Mountea Dialogue HUD Interface

	//~Begin IMounteaDialogueHUDClassInterface
	// 前三个函数是接口的纯虚函数，必须提供实现；本项目不使用 Viewport 容器层，
	// 因此统一返回 nullptr / 空实现。
	virtual TSubclassOf<UUserWidget> GetViewportBaseClass_Implementation() const override;
	virtual void InitializeViewportWidget_Implementation() override;
	virtual UUserWidget* GetViewportWidget_Implementation() const override;

	/** 把对话 Widget 直接挂到屏幕（UGameViewportSubsystem，per-player slot）。 */
	virtual void AddChildWidgetToViewport_Implementation(UUserWidget* ChildWidget, const int32 ZOrder, const FAnchors WidgetAnchors, const FMargin& WidgetMargin) override;
	virtual void RemoveChildWidgetFromViewport_Implementation(UUserWidget* ChildWidget) override;
	//~End IMounteaDialogueHUDClassInterface

#pragma endregion Mountea Dialogue HUD Interface
};
