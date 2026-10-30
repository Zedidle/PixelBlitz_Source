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

	UPROPERTY(EditDefaultsOnly, Category = NPC, meta = (AllowPrivateAccess = "true"))
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
	 * 负责"靠近时显示 E 对话提示"，真正触发对话走 OnInteractionCompleted。
	 *
	 * ❗ 它继承自 UWidgetComponent，提示是跟随 NPC 的世界空间 Widget，
	 *   必须提供 WidgetClass（NPC 蓝图指定，或回落到插件 Project Settings 默认值），
	 *   且该 Widget 必须实现 ActorInteractionWidget 接口，否则组件会被 Validate 判定为 Disabled。
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="NPC|Interaction")
	TObjectPtr<UActorInteractableComponentPress> InteractableComponent;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:

	/** 交互完成回调（Press 类型按下即完成），在这里发起对话。 */
	UFUNCTION()
	void HandleInteractionCompleted(const float& FinishTime, const TScriptInterface<IActorInteractorInterface>& CausingInteractor);

	/** 发起对话。做成 BlueprintNativeEvent，方便蓝图侧插入额外逻辑或临时禁用。 */
	UFUNCTION(BlueprintNativeEvent, Category="NPC|Interaction")
	void StartDialogueWith(AActor* Initiator);
	virtual void StartDialogueWith_Implementation(AActor* Initiator);
	
	
	
	
	
	
};
