// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Character/BasePXCharacter.h"
#include "Interfaces/Interact_Interface.h"
#include "BaseNPC.generated.h"

/**
 * 
 */
UCLASS()
class PIXEL2DKIT_API ABaseNPC : public APaperZDCharacter, public IInteract_Interface
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = NPC, meta = (AllowPrivateAccess = "true"))
	ABasePXCharacter* PxCharacter;

	
	void Tick_KeepFaceToPlayer2D();

	
	
public:

	virtual void Tick(float DeltaSeconds) override;

	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
};
