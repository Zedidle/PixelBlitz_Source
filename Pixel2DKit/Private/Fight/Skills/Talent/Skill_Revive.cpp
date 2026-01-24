// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Skills/Talent/Skill_Revive.h"

#include "NiagaraFunctionLibrary.h"
#include "Pixel2DKit.h"
#include "Interfaces/Fight_Interface.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXResourceDataAsset.h"
#include "Sound/SoundCue.h"
#include "Utilitys/SoundFuncLib.h"


class UPXCustomSettings;
// Sets default values
ASkill_Revive::ASkill_Revive()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASkill_Revive::BeginPlay()
{
	Super::BeginPlay();
	if (Owner && Owner->Implements<UFight_Interface>())
	{
		float Result = 0;
		IFight_Interface::Execute_FindEffectGameplayTag(Owner, TAG("Ability.Revive.Set.Times"), Result);
		RemReviveTimes = Result;
	}
}

// Called every frame
void ASkill_Revive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASkill_Revive::OnDying(int& _RemReviveTimes)
{
	if (!Super::OnDying(_RemReviveTimes)) return false;
	if (!Owner) return false;
	
	if (RemReviveTimes > 0)
	{
		const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
		if (!Settings) return false;
		const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
		if (!ResourceDataAsset) return false;
		
		if (UNiagaraSystem* NS_Revive = ResourceDataAsset->NS_Revive.LoadSynchronous())
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(NS_Revive, Owner->GetRootComponent(), FName(""), FVector::Zero(), FRotator(), EAttachLocation::Type::KeepRelativeOffset, true);
		}
		if (USoundCue* SC_Revive = ResourceDataAsset->SC_Revive.LoadSynchronous())
		{
			USoundFuncLib::PlaySound2D(SC_Revive);
		}
		RemReviveTimes--;
		_RemReviveTimes = RemReviveTimes;

		if (_RemReviveTimes == 0)
		{
			if (Owner->Implements<UFight_Interface>())
			{
				// IBuff_Interface::Execute_RemoveBuff(Owner, TAG("Ability.Revive"), true);
			}
		}
		
		return true;
	}
	
	return false;
}

