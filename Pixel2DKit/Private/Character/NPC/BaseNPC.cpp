// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NPC/BaseNPC.h"

#include "Pixel2DKit/Pixel2DKit.h"
#include "Utilitys/SpaceFuncLib.h"

void ABaseNPC::Tick_KeepFaceToPlayer2D()
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PxCharacter)

	bool flag = USpaceFuncLib::ActorAtActorRight(PxCharacter, this);
	SetActorRotation(FRotator(0, flag ? 0.0f : 180.f, 0));
		
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
