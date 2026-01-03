// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/Talent/TalentSkillRevive.h"

#include "NiagaraFunctionLibrary.h"
#include "Pixel2DKit.h"
#include "Interfaces/Fight_Interface.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXResourceDataAsset.h"
#include "Sound/SoundCue.h"
#include "Utilitys/SoundFuncLib.h"


class UPXCustomSettings;
// Sets default values
ATalentSkillRevive::ATalentSkillRevive()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATalentSkillRevive::BeginPlay()
{
	Super::BeginPlay();
	if (Owner && Owner->Implements<UFight_Interface>())
	{
		float Result = 0;
		IFight_Interface::Execute_FindEffectGameplayTag(Owner, TAG("TalentSet.ReviveTimesPlus"), Result);
		RemReviveTimes = Result;
	}
}

// Called every frame
void ATalentSkillRevive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ATalentSkillRevive::OnDying(int& _RemReviveTimes)
{
	if (!Super::OnDying(_RemReviveTimes)) return false;
	
	if (RemReviveTimes > 0)
	{
		const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
		if (!Settings) return false;
		const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
		if (!ResourceDataAsset) return false;
		
		if (UNiagaraSystem* NS_Revive = ResourceDataAsset->NS_Revive.LoadSynchronous())
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(NS_Revive, GetRootComponent(), FName(""), FVector::Zero(), FRotator(), EAttachLocation::Type::KeepRelativeOffset, true);
		}
		if (USoundCue* SC_Revive = ResourceDataAsset->SC_Revive.LoadSynchronous())
		{
			USoundFuncLib::PlaySound2D(SC_Revive);
		}
		RemReviveTimes--;
		_RemReviveTimes = RemReviveTimes;
		
		return true;
	}
	
	return false;
}

