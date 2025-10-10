// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/ZDAN_OnSoundPlay.h"

#include "PaperZDAnimInstance.h"
#include "PaperZDCharacter.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Utilitys/SoundFuncLib.h"

void UZDAN_OnSoundPlay::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance) const
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningInstance)
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Sound)
	
	if (APaperZDCharacter* PaperZDCharacter = OwningInstance->GetPaperCharacter())
	{
		FVector Location = PaperZDCharacter->GetActorLocation();
		USoundFuncLib::PlaySoundAtLocation(Sound, Location);		
	}
}
