// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/ZDAN_OnFootStepEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "PaperFlipbookComponent.h"
#include "PaperZDCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/Config/CustomResourceSettings.h"
#include "NiagaraComponent.h"
#include "Core/PXGameState.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXResourceDataAsset.h"
#include "Utilitys/PXGameplayStatics.h"
#include "Utilitys/SoundFuncLib.h"

class UPXCustomSettings;
class UNiagaraComponent;

void UZDAN_OnFootStepEffect::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance)
{
	Super::OnReceiveNotify_Implementation(OwningInstance);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningInstance)

	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

	const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceDataAsset)

	APaperZDCharacter* Character = OwningInstance->GetPaperCharacter();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Character)

	UPaperFlipbookComponent* PFComp = Character->GetComponentByClass<UPaperFlipbookComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PFComp)

	FVector TargetLocation = PFComp->GetComponentLocation();
	FVector Velocity = Character->GetVelocity();


	// 粒子特效
	APXGameState* GS = UPXGameplayStatics::GetGameState(OwningInstance);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GS)

	if (UNiagaraSystem* LoadedNS = ResourceDataAsset->WeatherTypeToNiagara.FindRef(GS->WeatherType).LoadSynchronous())
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LoadedNS,
				TargetLocation, FRotator(0), PFComp->GetRelativeScale3D(),true, true);
		NiagaraComponent->SetVariableVec3(TEXT("Dir"), -Velocity.GetSafeNormal());
		NiagaraComponent->SetVariableFloat(TEXT("Speed"), Velocity.Size());
	}

	// 音效
	if (USoundBase* LoadedSound = ResourceDataAsset->WeatherTypeToSound.FindRef(GS->WeatherType).LoadSynchronous())
	{
		USoundFuncLib::PlaySoundAtLocation(LoadedSound, TargetLocation);
	}
	
}
