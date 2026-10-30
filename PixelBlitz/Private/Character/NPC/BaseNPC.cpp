// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NPC/BaseNPC.h"

#include "PixelBlitz/PixelBlitz.h"
#include "Utilitys/SpaceFuncLib.h"

#include "Blueprint/UserWidget.h"
#include "Components/MounteaDialogueManager.h"
#include "GameFramework/PlayerController.h"
#include "Helpers/ActorInteractionFunctionLibrary.h"
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

	// WidgetClass 为空时回落到插件 Project Settings（Mountea Interaction）里的默认 Widget，
	// 否则 ValidateInteractable 会判定组件 Disabled，提示永远不显示。
	if (!InteractableComponent->GetWidgetClass())
	{
		if (const TSubclassOf<UUserWidget> DefaultWidget = UActorInteractionFunctionLibrary::GetInteractableDefaultWidgetClass())
		{
			InteractableComponent->SetWidgetClass(DefaultWidget);
		}
	}

	// ❗ OnInteractionCompleted 是 protected 成员，C++ 必须走 GetOnInteractionCompletedHandle() 取引用再绑定
	InteractableComponent->GetOnInteractionCompletedHandle().AddDynamic(this, &ThisClass::HandleInteractionCompleted);
}

void ABaseNPC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (InteractableComponent)
	{
		InteractableComponent->GetOnInteractionCompletedHandle().RemoveDynamic(this, &ThisClass::HandleInteractionCompleted);
	}

	Super::EndPlay(EndPlayReason);
}

void ABaseNPC::HandleInteractionCompleted(const float& FinishTime, const TScriptInterface<IActorInteractorInterface>& CausingInteractor)
{
	const UObject* InteractorObject = CausingInteractor.GetObject();
	if (!InteractorObject)
	{
		return;
	}

	AActor* Initiator = nullptr;
	if (const UActorComponent* InteractorComp = Cast<UActorComponent>(InteractorObject))
	{
		Initiator = InteractorComp->GetOwner();
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
