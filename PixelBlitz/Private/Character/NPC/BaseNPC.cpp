// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NPC/BaseNPC.h"

#include "PixelBlitz/PixelBlitz.h"
#include "Utilitys/SpaceFuncLib.h"

#include "Components/MounteaDialogueManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/Core/MounteaDialogueManagerInterface.h"
#include "Player/PXPlayerState.h"

void ABaseNPC::Tick_KeepFaceToPlayer2D()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PxCharacter)

	bool flag = USpaceFuncLib::ActorAtActorRight(PxCharacter, this);
	SetActorRotation(FRotator(0, flag ? 0.0f : 180.f, 0));
		
}

ABaseNPC::ABaseNPC(const FObjectInitializer& ObjectInitializer)	:Super(ObjectInitializer)
{
	DialogueParticipant = CreateDefaultSubobject<UMounteaDialogueParticipant>(TEXT("DialogueParticipant"));

	InteractableComponent = CreateDefaultSubobject<UActorInteractableComponentPress>(TEXT("InteractableComponent"));
}

void ABaseNPC::BeginPlay()
{
	Super::BeginPlay();

	if (!InteractableComponent)
	{
		return;
	}

	// ① 通道对齐：ProcessOverlap 会拿它跟玩家 Interactor 的 ResponseChannel 比对，不等直接 continue。
	//    这是唯一没法用蓝图替代的：蓝图只能写死一个值，这里读的是玩家运行时的真值。
	AlignInteractionCollisionChannel();
	IActorInteractableInterface::Execute_SetCollisionChannel(InteractableComponent, InteractionCollisionChannel);

	UE_LOG(LogTemp, Log, TEXT("[%s] Interactable 就绪：Channel=%d, CollisionComponents=%d"),
		*GetName(),
		(int32)InteractionCollisionChannel.GetValue(),
		IActorInteractableInterface::Execute_GetCollisionComponents(InteractableComponent).Num());

	// ② 把提示顶到头顶。
	// ❗ 必须留在 C++：Attach 必须晚于 Super::BeginPlay()（组件的 AutoSetup 在里面跑），
	//    否则 EST_Quick 会拿 GetAttachParent() 当碰撞体，把胶囊登记进去。
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		InteractableComponent->AttachToComponent(Capsule, FAttachmentTransformRules::KeepRelativeTransform);
		InteractableComponent->SetRelativeLocation(FVector(0.f, 0.f, InteractionPromptHeight));
	}

	// ③ Cycled 模式下 Press 每按一次发的是 OnInteractionCycleCompleted（OnInteractionCompleted 被
	//    TriggerCooldown 的提前 return 吞掉了）。委托是 protected 成员，C++ 必须走
	//    GetOnInteractionCycleCompletedHandle() 取引用再绑定。
	InteractableComponent->GetOnInteractionCycleCompletedHandle().AddDynamic(this, &ThisClass::HandleInteractionCycleCompleted);
}

void ABaseNPC::AlignInteractionCollisionChannel()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn)
	{
		return;
	}

	if (UActorComponent* Interactor = PlayerPawn->FindComponentByInterface(UActorInteractorInterface::StaticClass()))
	{
		InteractionCollisionChannel = IActorInteractorInterface::Execute_GetResponseChannel(Interactor);
	}
}

void ABaseNPC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (InteractableComponent)
	{
		// 兜底：把状态机放回 Awake，防止任何路径（交互中断/冷却）把它留在失效态
		IActorInteractableInterface::Execute_SetState(InteractableComponent, EInteractableStateV2::EIS_Awake);

		InteractableComponent->GetOnInteractionCycleCompletedHandle().RemoveDynamic(this, &ThisClass::HandleInteractionCycleCompleted);
	}

	Super::EndPlay(EndPlayReason);
}

void ABaseNPC::HandleInteractionCycleCompleted(const float& FinishTime, const int32 RemainingLifecycles, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
	AActor* Initiator = nullptr;
	if (const UObject* InteractorObject = CausingInteractor.GetObject())
	{
		Initiator = IActorInteractorInterface::Execute_GetOwningActor(InteractorObject);
	}

	StartDialogueWith(Initiator);
}

void ABaseNPC::StartDialogueWith_Implementation(AActor* Initiator)
{
	if (!Initiator)
	{
		return;
	}

	const APawn* InitiatorPawn = Cast<APawn>(Initiator);
	const APlayerController* PC = InitiatorPawn ? InitiatorPawn->GetController<APlayerController>() : nullptr;
	if (!PC)
	{
		return;
	}

	APXPlayerState* PXPlayerState = PC->GetPlayerState<APXPlayerState>();
	if (!PXPlayerState || !PXPlayerState->MounteaDialogueManager)
	{
		return;
	}

	FDialogueParticipants Participants;
	Participants.MainParticipant = this;

	IMounteaDialogueManagerInterface::Execute_RequestStartDialogue(
		PXPlayerState->MounteaDialogueManager,
		Initiator,
		Participants);
}

void ABaseNPC::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Tick_KeepFaceToPlayer2D();
	// 换人恢复兜底：不依赖重叠时机，每帧校验占用关系（见 TryRecoverInteraction 注释）
	TryRecoverInteraction();
}

void ABaseNPC::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	TryRecoverInteraction();
}

void ABaseNPC::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
}

void ABaseNPC::TryRecoverInteraction()
{
	if (!InteractableComponent)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn)
	{
		return;
	}

	const TScriptInterface<IActorInteractorInterface> BoundInteractor =
		IActorInteractableInterface::Execute_GetInteractor(InteractableComponent);
	AActor* BoundOwner = BoundInteractor.GetObject()
		? IActorInteractorInterface::Execute_GetOwningActor(BoundInteractor.GetObject())
		: nullptr;

	// ① 通道对齐：只在"玩家Pawn 变化"或"存在陈旧占用"时执行，避免每帧查找开销
	const bool bStaleBinding = BoundOwner != nullptr && BoundOwner != PlayerPawn;
	if (LastAlignedPlayerPawn.Get() != PlayerPawn || bStaleBinding)
	{
		AlignInteractionCollisionChannel();
		IActorInteractableInterface::Execute_SetCollisionChannel(InteractableComponent, InteractionCollisionChannel);
		LastAlignedPlayerPawn = PlayerPawn;
	}

	if (!bStaleBinding)
	{
		return;
	}

	// ② 陈旧绑定清理（换人后旧 Pawn 幽灵仍占着 Interactable）
	APawn* BoundPawn = Cast<APawn>(BoundOwner);
	if (BoundPawn && !BoundPawn->IsPlayerControlled())
	{
		FString SuppressError;
		IActorInteractorInterface::Execute_SuppressInteractor(BoundInteractor.GetObject(), SuppressError);
	}

	IActorInteractableInterface::Execute_SetInteractor(InteractableComponent, nullptr);

	const EInteractableStateV2 CurrentState = IActorInteractableInterface::Execute_GetState(InteractableComponent);
	if (CurrentState == EInteractableStateV2::EIS_Suppressed || CurrentState == EInteractableStateV2::EIS_Disabled)
	{
		IActorInteractableInterface::Execute_SetState(InteractableComponent, EInteractableStateV2::EIS_Awake);
	}

	UE_LOG(LogTemp, Log, TEXT("[%s] 清理陈旧交互绑定：占用者=%s（已压制），当前玩家=%s"),
		*GetName(), *GetNameSafe(BoundOwner), *PlayerPawn->GetName());

	// ③ 主动补发 InteractorFound：清理时玩家多半已在重叠范围内，干等下次 BeginOverlap 会丢提示；
	//    直接驱动一次"发现"，让 E 提示立刻点亮（内部自带 CanBeTriggered 校验，不安全则静默跳过）
	if (UActorComponent* PlayerInteractorComp = PlayerPawn->FindComponentByInterface(UActorInteractorInterface::StaticClass()))
	{
		TScriptInterface<IActorInteractorInterface> PlayerInteractor;
		PlayerInteractor.SetObject(PlayerInteractorComp);
		PlayerInteractor.SetInterface(Cast<IActorInteractorInterface>(PlayerInteractorComp));

		if (PlayerInteractor.GetInterface() &&
			IActorInteractableInterface::Execute_CanBeTriggered(InteractableComponent))
		{
			IActorInteractableInterface::Execute_InteractorFound(InteractableComponent, PlayerInteractor);
		}
	}
}
