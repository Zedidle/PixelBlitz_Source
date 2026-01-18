// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/GA_SkyHandPower.h"

#include "NiagaraFunctionLibrary.h"
#include "Character/BasePXCharacter.h"
#include "Character/Components/AbilityComponent.h"
#include "Enemy/BaseEnemy.h"
#include "Fight/Components/HealthComponent.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXResourceDataAsset.h"


void UGA_SkyHandPower::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APawn* Pawn = GetPawn();
	if (!Pawn)
	{
		K2_EndAbility();
		return;
	}
	
	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(Pawn);
	if (!PXCharacter || !PXCharacter->Implements<UBuff_Interface>() ||
		!PXCharacter->Implements<UFight_Interface>() ||
		!IFight_Interface::Execute_IsAlive(PXCharacter))
	{
		K2_EndAbility();
		return;
	}

	UAbilityComponent* PlayerAbilityComponent = PXCharacter->AbilityComponent;
	if (!PlayerAbilityComponent)
	{
		K2_EndAbility();
		return;
	}
	
	AActor* HurtInstigator = PlayerAbilityComponent->HurtMaker;
	if (!HurtInstigator)
	{
		K2_EndAbility();
		return;
	}
	ABaseEnemy* Enemy = Cast<ABaseEnemy>(HurtInstigator);
	if (!Enemy || !Enemy->Implements<UFight_Interface>() ||
		!IFight_Interface::Execute_IsAlive(Enemy) ||
		IFight_Interface::Execute_GetOwnCamp(Enemy).HasTag(TAG("Enemy.BOSS")))
	{
		K2_EndAbility();
		return;
	}
	UHealthComponent* EnemyHealthComponent = Enemy->GetComponentByClass<UHealthComponent>();
	if (!EnemyHealthComponent)
	{
		K2_EndAbility();
		return;
	}

	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	float MultiPercent;
	IFight_Interface::Execute_FindEffectGameplayTag(PXCharacter, TAG("Ability.SkyHandPower.Set.DamageOnAttackDamagePercent"), MultiPercent);
	
	int Damage = MultiPercent * IBuff_Interface::Execute_Buff_CalInitDamage(PXCharacter, IFight_Interface::Execute_GetAttackDamage(PXCharacter));
	FVector PreEnemyLocation = Enemy->GetActorLocation();
	
	float SwitchedEnemyHeightOffset = Enemy->GetDefaultHalfHeight();

	FVector EnemyNewLocation = Enemy->GetActorScale3D() * FVector(0, 0, SwitchedEnemyHeightOffset) + PXCharacter->GetActorLocation();

	FVector PlayerCameraOffset = EnemyNewLocation - PreEnemyLocation;
	PXCharacter->CameraOffset_BulletTime(0.2f, PlayerCameraOffset, 0.5);
	
	Enemy->SetActorLocation(EnemyNewLocation);
	PXCharacter->SetActorLocation(PreEnemyLocation);

	EnemyHealthComponent->DecreaseHP(Damage, PXCharacter, PlayerCameraOffset, true);

	const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

	const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceDataAsset)
	
	if (!ResourceDataAsset || !ResourceDataAsset->NS_Mobiliarbus)
	{
		K2_EndAbility();
		return;
	}

	UNiagaraSystem* LoadedNS = ResourceDataAsset->NS_Mobiliarbus.LoadSynchronous();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LoadedNS, PXCharacter->GetActorLocation());
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LoadedNS, Enemy->GetActorLocation());

	K2_EndAbility();
}
