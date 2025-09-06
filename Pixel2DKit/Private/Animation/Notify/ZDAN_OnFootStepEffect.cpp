// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/ZDAN_OnFootStepEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "PaperFlipbookComponent.h"
#include "PaperZDCharacter.h"
#include "Basic/PXGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/CustomResourceSettings.h"
#include "NiagaraComponent.h"
#include "Basic/PXGameState.h"
#include "Utilitys/SoundFuncLib.h"

class UNiagaraComponent;

void UZDAN_OnFootStepEffect::OnReceiveNotify_Implementation(UPaperZDAnimInstance* OwningInstance)
{
	Super::OnReceiveNotify_Implementation(OwningInstance);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwningInstance)

	const UCustomResourceSettings* ResourceSettings = GetDefault<UCustomResourceSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceSettings)

	APaperZDCharacter* Character = OwningInstance->GetPaperCharacter();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Character)

	UPaperFlipbookComponent* PFComp = Character->GetComponentByClass<UPaperFlipbookComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PFComp)

	FVector TargetLocation = PFComp->GetComponentLocation();
	FVector Velocity = Character->GetVelocity();


	// 粒子特效
	APXGameState* GS = Cast<APXGameState>(UGameplayStatics::GetGameState(GetWorld()));
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GS)
	
	TSoftObjectPtr<UNiagaraSystem> NS = ResourceSettings->WeatherTypeToNiagara.FindRef(GS->WeatherType);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(NS)

	UNiagaraSystem* LoadedNS = NS.LoadSynchronous();
	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LoadedNS,
		TargetLocation, FRotator(0), PFComp->GetRelativeScale3D(),true, true);
	
	NiagaraComponent->SetVariableVec3(TEXT("Dir"), -Velocity.GetSafeNormal());
	NiagaraComponent->SetVariableFloat(TEXT("Speed"), Velocity.Size());

	// 音效
	TSoftObjectPtr<USoundBase> Sound = ResourceSettings->WeatherTypeToSound.FindRef(GS->WeatherType);
	USoundBase* LoadedSound = Sound.LoadSynchronous();
	USoundFuncLib::PlaySoundAtLocation(LoadedSound, TargetLocation);
	
}
