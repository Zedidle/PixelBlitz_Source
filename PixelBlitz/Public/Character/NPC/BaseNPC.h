// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Character/BasePXCharacter.h"
#include "Components/MounteaDialogueParticipant.h"
#include "Interfaces/Interact_Interface.h"
#include "BaseNPC.generated.h"

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
	
	
	
	
	
	
};
