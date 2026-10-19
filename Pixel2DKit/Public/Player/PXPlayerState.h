// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "PXPlayerState.generated.h"

class UMounteaDialogueManager;
class UUserWidget;

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API APXPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	APXPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { checkf(false, TEXT("%s need implement GetAbilitySystemComponent interface"), *GetName()); return nullptr; }

	/** Mountea Dialogue Manager。
	 *  ❗ 必须挂在 PlayerState 上：UMounteaDialogueManager::CalculateManagerType 只在
	 *  Pawn / PlayerState / PlayerController 三种宿主下判定为 EDMT_PlayerDialogue，
	 *  其他宿主（含 AGameState、AMounteaDialogueManagerActor）会落到 EDMT_EnvironmentDialogue，
	 *  那条路径要求 PlayerController 上必须有 UMounteaDialogueDialogueNetSync。
	 *  挂在 PlayerState 上还有两个好处：跨关卡存活、SetupPlayerDialogue 不需要任何网络组件。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialogue", DisplayName="MounteaDialogueManager", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMounteaDialogueManager> MounteaDialogueManager = nullptr;

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** 对话 Widget 创建/销毁时触发（Manager 的 OnDialogueUserInterfaceChanged）。
	 *  Manager 只负责 CreateWidget，不负责上屏；这里接住事件，
	 *  把 Widget 交给 HUD（APXHUD 实现了 IMounteaDialogueHUDClassInterface）挂进 Viewport。
	 *  DialogueWidget 为 null 表示 Widget 已被销毁，此时执行摘除。
	 *
	 *  ❗ 注意：OnDialogueUserInterfaceChanged 本身是 Manager 的 protected 成员，
	 *  C++ 无法直接访问；必须经由 IMounteaDialogueManagerInterface 的
	 *  GetDialogueUserInterfaceChangedEventHandle() 拿到引用再绑定。 */
	UFUNCTION()
	void HandleDialogueUserInterfaceChanged(TSubclassOf<UUserWidget> DialogueWidgetClass, UUserWidget* DialogueWidget);

	/** 当前已挂进 Viewport 的对话 Widget，用于在对话结束时摘除。 */
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> PickedUpDialogueWidget = nullptr;
};
