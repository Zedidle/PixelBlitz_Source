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
}

void ABaseNPC::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

void ABaseNPC::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
}
