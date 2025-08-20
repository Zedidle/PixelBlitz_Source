// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/GA_SkyhandPower.h"

#include "NiagaraFunctionLibrary.h"
#include "Character/BasePXCharacter.h"
#include "Character/Components/AbilityComponent.h"
#include "Fight/Components/HealthComponent.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/CustomResourceSettings.h"


void UGA_SkyhandPower::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ABasePXCharacter* PXCharacter = Cast<ABasePXCharacter>(GetAvatarActorFromActorInfo());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter)

	UAbilityComponent* PlayerAbilityComponent = PXCharacter->GetAbilityComponent();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PlayerAbilityComponent)

	AActor* HurtInstigator = PlayerAbilityComponent->HurtInstigator;
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(HurtInstigator);
	ABaseEnemy* Enemy = Cast<ABaseEnemy>(HurtInstigator);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Enemy);

	if (!Enemy->Implements<UFight_Interface>()) return;
	if (!IFight_Interface::Execute_IsAlive(Enemy)) return;

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(PXCharacter);

	if (!PXCharacter->Implements<UBuff_Interface>()) return;

	int Damage = IBuff_Interface::Execute_Buff_CalDamage(PXCharacter, PXCharacter->GetWeaponInitDamage());
	FVector PreEnemyLocation = Enemy->GetActorLocation();

	Enemy->SetActorLocation(Enemy->GetActorScale3D() * FVector(0, 0, 44.0f) + PXCharacter->GetActorLocation());
	PXCharacter->SetActorLocation(PreEnemyLocation);
	
	UHealthComponent* EnemyHealthComponent = Enemy->GetComponentByClass<UHealthComponent>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(EnemyHealthComponent);
	EnemyHealthComponent->DecreaseHP(Damage, FVector(0,0,0),
		PXCharacter, true, true, false);

	const UCustomResourceSettings* ResourceSettings = GetDefault<UCustomResourceSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceSettings);
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceSettings->NS_Mobiliarbus);

	UNiagaraSystem* LoadedNS = ResourceSettings->NS_Mobiliarbus.LoadSynchronous();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LoadedNS, PXCharacter->GetActorLocation());
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LoadedNS, Enemy->GetActorLocation());

	K2_EndAbility();
}
