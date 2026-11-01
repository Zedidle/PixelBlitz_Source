// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Character/BasePXCharacter.h"
#include "Components/MounteaDialogueParticipant.h"
#include "Components/Interactable/ActorInteractableComponentPress.h"
#include "Interfaces/ActorInteractableInterface.h"
#include "Interfaces/ActorInteractorInterface.h"
#include "Interfaces/Interact_Interface.h"
#include "BaseNPC.generated.h"

class UActorInteractableComponentPress;

/**
 * 
 */
UCLASS()
class PIXELBLITZ_API ABaseNPC : public APaperZDCharacter, public IInteract_Interface
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = NPC, meta = (AllowPrivateAccess = "true"))
	ABasePXCharacter* PxCharacter;

	UPROPERTY(BlueprintReadWrite, Category = NPC, meta = (AllowPrivateAccess = "true"))
	FName NPCID;
	
	void Tick_KeepFaceToPlayer2D();
	
	
public:

	ABaseNPC(const FObjectInitializer& ObjectInitializer);
	
	virtual void Tick(float DeltaSeconds) override;

	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMounteaDialogueParticipant* DialogueParticipant;

	/**
	 * Mountea 交互组件（Press 类型：按一下即完成）。
	 * 负责"靠近时显示 E 对话提示"，真正触发对话走 OnInteractionCycleCompleted。
	 *
	 * ❗ 它继承自 UWidgetComponent，提示是跟随 NPC 的世界空间 Widget。
	 *   Widget Class / Interactable Data / Lifecycle Mode / Cooldown Period
	 *   全部在 B_NPC_Sophia 的这个组件面板里设。
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="NPC|Interaction")
	TObjectPtr<UActorInteractableComponentPress> InteractableComponent;

	/** 交互提示相对胶囊中心的抬高量（cm），用来把它顶到头顶。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="NPC|Interaction", meta=(AllowPrivateAccess="true"))
	float InteractionPromptHeight = 120.f;

	/**
	 * 碰撞通道：必须与玩家 Interactor 的 Collision Channel 完全一致，否则
	 * ProcessOverlap 会直接 continue，根本走不到"被发现"。
	 * 插件 Interactor 基类默认 ECC_Camera，示例角色也是它；BeginPlay 里还会再自动对齐一次。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="NPC|Interaction", meta=(AllowPrivateAccess="true"))
	TEnumAsByte<ECollisionChannel> InteractionCollisionChannel = ECC_Camera;

	// ❗ 除上面两项外，Interactable 的 Widget Class / Interactable Data / Lifecycle Mode
	//    一律在 B_NPC_Sophia 的组件面板里设，C++ 不镜像、不覆盖
	//    （LifecycleMode 是 SaveGame + Replicated，强行 Set 会冲掉存档恢复的值）。
	//
	//    窄相位（"玩家有没有朝着 NPC"）也不在这里做 —— 它属于目标选择，是 Interactor 的职责，
	//    已搬到 ABasePXCharacter（全场 1 个定时器，而不是每个 NPC 各起一个）。

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:

	/** 交互回调。Cycled 模式下每按一次都发 OnInteractionCycleCompleted，在这里发起对话。 */
	UFUNCTION()
	void HandleInteractionCycleCompleted(const float& FinishTime, const int32 RemainingLifecycles, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);

	/** 发起对话。做成 BlueprintNativeEvent，方便蓝图侧插入额外逻辑或临时禁用。 */
	UFUNCTION(BlueprintNativeEvent, Category="NPC|Interaction")
	void StartDialogueWith(AActor* Initiator);
	virtual void StartDialogueWith_Implementation(AActor* Initiator);

private:

	/**
	 * 运行时读本地玩家 Interactor 的 ResponseChannel 来对齐，省得手抄错。
	 * 这是唯一没法用蓝图替代的一项：蓝图只能写死一个值，这里读的是玩家运行时真值。
	 */
	void AlignInteractionCollisionChannel();
	
	
	
	
	
	
};
