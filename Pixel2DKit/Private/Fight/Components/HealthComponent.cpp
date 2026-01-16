// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Components/HealthComponent.h"

#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/AttributeSet/PXAttributeSet.h"
#include "Interfaces/Fight_Interface.h"
#include "Kismet/GameplayStatics.h"
#include "Utilitys/CommonFuncLib.h"
#include "AbilitySystemGlobals.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Pixel2DKit.h"
#include "Settings/PXSettingsShared.h"
#include "Settings/Config/PXCameraSourceDataAsset.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXForceFeedbackEffectDataAsset.h"
#include "Settings/Config/PXResourceDataAsset.h"
#include "Subsystems/TimerSubsystemFuncLib.h"
#include "Utilitys/DebugFuncLab.h"
#include "Utilitys/PXGameplayStatics.h"

#define LOCTEXT_NAMESPACE "PX"

class ULegacyCameraShake;

int UHealthComponent::CalAcceptDamage(int InDamage, AActor* Maker)
{
	AActor* Owner = GetOwner();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(Owner, InDamage)
	if (!Owner->Implements<UFight_Interface>()) return InDamage;
	if (GetCurrentHP() <= 0) return 0;
	
	int DamagePlus = 0;
	if (Maker && Maker->Implements<UFight_Interface>())
	{
		DamagePlus = IFight_Interface::Execute_DamagePlus(Maker, InDamage, Owner);
	}
	
	return InDamage + DamagePlus;
}

void UHealthComponent::OnHurtInvulnerable()
{
	SetInvulnerable(false);
	FName TimerName = FName( GetReadableName()+ "_OnHurtInvulnerable");
	
	UTimerSubsystemFuncLib::SetRetriggerableDelay(GetWorld(), TimerName,
[WeakThis = TWeakObjectPtr(this)]
	{
		if (!WeakThis.IsValid()) return;
		WeakThis->InvulnerableForDuration(WeakThis->InvulnerableDuration);
	}, OnHurtInvulnerableDelay);
}

void UHealthComponent::InvulnerableForDuration(float duration)
{
	SetInvulnerable(true);
	UTimerSubsystemFuncLib::SetDelay(GetWorld(), [WeakThis = TWeakObjectPtr(this)]
	{
		if (!WeakThis.IsValid()) return;
		WeakThis->SetInvulnerable(false);
	}, duration);
}

void UHealthComponent::FlashForDuration(float Duration, float FlashRate, FLinearColor FlashColor) 
{
	UWorld* World = GetWorld();
	if (!World) return;

	int FlashTimes = Duration / FlashRate;
	if (FlashTimes % 2 == 1) FlashTimes++;
	
	FName TimerName = FName("UHealthComponent::FlashForDuration" + FGuid::NewGuid().ToString());
	UTimerSubsystemFuncLib::SetDelayLoop(World, TimerName,
		[WeakThis = TWeakObjectPtr(this), FlashColor]
		{
			if (!WeakThis.IsValid()) return;
			
			UPaperFlipbookComponent* PF = WeakThis->GetOwner()->GetComponentByClass<UPaperFlipbookComponent>();
			if (!PF) return;
			
			WeakThis->bFlashing = !WeakThis->bFlashing;
			PF->SetSpriteColor(WeakThis->bFlashing ? FlashColor :
				FMath::GetMappedRangeValueClamped(FVector2D(0.2f, 1),FVector2D(0.4, 1), WeakThis->GetHPPercent()) * FLinearColor::White
			);
		}, FlashRate, -1, FlashTimes);
}

FVector UHealthComponent::CalRepel(FVector& IncomeRepel, const AActor* Instigator) const
{
	if (!IsValid(Instigator)) return IncomeRepel;
	IncomeRepel = (1.0f - RepelResistancePercent) * IncomeRepel;
	IncomeRepel.X = IncomeRepel.X > 0 ? FMath::Max(IncomeRepel.X - RepelResistance.X, 0) : FMath::Min(IncomeRepel.X + RepelResistance.X, 0);
	IncomeRepel.Y = IncomeRepel.Y > 0 ? FMath::Max(IncomeRepel.Y - RepelResistance.Y, 0) : FMath::Min(IncomeRepel.Y + RepelResistance.Y, 0);
	IncomeRepel.Z = IncomeRepel.Z > 0 ? FMath::Max(IncomeRepel.Z - RepelResistance.Z, 0) : FMath::Min(IncomeRepel.Z + RepelResistance.Z, 0);
	
	FVector Result = (GetOwner()->GetActorLocation() - Instigator->GetActorLocation()).GetSafeNormal2D() * IncomeRepel.Size2D();
	Result.Z = IncomeRepel.Z;
	return Result;
}

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

UHealthComponent::UHealthComponent(const FObjectInitializer& ObjectInitializer)
{
}

UHealthComponent::~UHealthComponent()
{
}


void UHealthComponent::SetInvulnerable(const bool v)
{
	bInvulnerable = v;
}

void UHealthComponent::ModifyMaxHP(int32 value, const EStatChange ChangeType, const bool current)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
	if (!IsValid(TargetASC)) return;
	const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>();
	if (!IsValid(PixelAS)) return;

	value = FMath::Max(value, 0);
	
	int CurrentHealth = PixelAS->GetHP();
	int MaxHealth = PixelAS->GetMaxHP();
	
	if (ChangeType == EStatChange::Increase)
	{
		TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetMaxHPAttribute(), MaxHealth + value);
		if (current)
		{
			SetHP(CurrentHealth + value, false);
		}

	}else if (ChangeType == EStatChange::Decrease)
	{
		TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetMaxHPAttribute(), FMath::Max(0, MaxHealth - value));
		if (current)
		{
			SetHP(FMath::Max(0, CurrentHealth - value), false);
		}
	}
	else if (ChangeType == EStatChange::Reset)
	{
		TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetMaxHPAttribute(), value);
		if (current || value < CurrentHealth)
		{
			SetHP(value, false);
		}
	}
}

void UHealthComponent::SetHP(const int32 value, const bool broadcast)
{
	AActor* TargetActor = GetOwner();
	if (!IsValid(TargetActor)) return;

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
	{
		if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
		{
			int CurrentHealth = FMath::Clamp(value, 0, PixelAS->GetMaxHP());
			TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetHPAttribute(), CurrentHealth);
		}
	}
}



void UHealthComponent::IncreaseHP(int32 value, AActor* Instigator)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
	if (!IsValid(TargetASC)) return;
	const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>();
	if (!IsValid(PixelAS)) return;

	value = FMath::Max(value, 0);
	
	int preHealth = PixelAS->GetHP();
	int MaxHealth = PixelAS->GetMaxHP();
	int CurrentHealth = FMath::Min(value + preHealth, MaxHealth);

	if (CurrentHealth - preHealth > 0)
	{
		TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetHPAttribute(), CurrentHealth);
	}
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	bOwnerIsPlayer = IFight_Interface::Execute_GetOwnCamp(GetOwner()).HasTag(FGameplayTag::RequestGameplayTag(FName("Player")));
	
	OnHPChanged.AddDynamic(this, &UHealthComponent::Event_OnHPChanged);
}

void UHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);


	OnHPChanged.RemoveAll(this);
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



void UHealthComponent::Event_OnHPChanged(int OldValue, int NewValue)
{
	int32 ChangedHP = OldValue - NewValue;
	if (ChangedHP > 0)
	{
		if (ChangedHP > 0.1 * GetMaxHP())
		{
			float HurtDuration = CalHurtDuration(ChangedHP);
			// 触发受伤闪烁
			FlashForDuration(HurtDuration);
			// 触发无敌帧
			OnHurtInvulnerable();
		}
	}
}

int UHealthComponent::GetCurrentHP()
{
	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
			{
				return PixelAS->GetHP();
			}
		}
	}
	
	return 0;
}

int UHealthComponent::GetMaxHP()
{
	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
			{
				return PixelAS->GetMaxHP();
			}
		}
	}

	return 100;
}

float UHealthComponent::GetHPPercent()
{
	return float(GetCurrentHP()) / float(GetMaxHP());
}

void UHealthComponent::DecreaseHP(int Damage, AActor* Maker, const FVector KnockbackForce, bool bForce)
{
	if (Damage <= 0) return;
	AActor* Owner = GetOwner();
	if (!Owner->Implements<UFight_Interface>()) return;

	if (!IsValid(Owner)) return;
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
	if (!IsValid(TargetASC)) return;
	const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>();
	if (!IsValid(PixelAS)) return;

	if (bInvulnerable && !bForce)
	{
		IFight_Interface::Execute_OnBeAttacked_Invulnerable(Owner);
		return;
	}

	int OutDamage = Damage;

	IFight_Interface::Execute_OnBeAttacked(Owner, Maker, Damage, OutDamage, bForce);
	KnockBack(KnockbackForce * (OutDamage / Damage), Maker);
	
	
	if (OutDamage <= 0) return;

	int calDamage = CalAcceptDamage(OutDamage, Maker);
	calDamage = IFight_Interface::Execute_OnDefendingHit(Owner, calDamage);

	int preHealth = PixelAS->GetHP();
	int CurrentHealth = FMath::Max(preHealth - calDamage, 0);
	int ChangedValue = FMath::Abs(preHealth - CurrentHealth);
	if (ChangedValue <= 0) return;
	
	UPXSettingsShared* SettingsShared = UPXGameplayStatics::GetSettingsShared(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(SettingsShared);
	
	if (SettingsShared->GetShowBloodVFX())
	{
		const UPXCustomSettings* Settings = GetDefault<UPXCustomSettings>();
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Settings)

		const UPXResourceDataAsset* ResourceDataAsset = Settings->ResourceDataAsset.LoadSynchronous();
		CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceDataAsset)
		
		if ( ChangedValue < 10)
		{
			if (UNiagaraSystem* NS_HitBlood_Little = ResourceDataAsset->NS_HitBlood_Little.LoadSynchronous())
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_HitBlood_Little, Owner->GetActorLocation());
			}
		}
		else
		{
			if (Maker)
			{
				if (UNiagaraSystem* NS_HitBlood = ResourceDataAsset->NS_HitBlood.LoadSynchronous())
				{
					UNiagaraComponent* NSEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_HitBlood, Owner->GetActorLocation());
					NSEffect->SetVariableVec3(FName("Dir"), (Owner->GetActorLocation() - Maker->GetActorLocation()).GetSafeNormal()) ;
				}
			}
		}
	}
	
	TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetHPAttribute(), CurrentHealth);
}

void UHealthComponent::KnockBack(FVector Repel, AActor* Maker)
{
	if (InRockPercent > 0.0f)
	{
		UCommonFuncLib::SpawnFloatingText(LOCTEXT("BUFF_INROCK", "霸体"),
			GetOwner()->GetActorLocation(), FColor::Purple, FVector2D(0.8, 0.8));
	}

	if (bInvulnerable) return;

	Repel = (1 - InRockPercent) * CalRepel(Repel, Maker) * KnockBackMultiplier;
	if (UCharacterMovementComponent* MovementComponent = GetOwner()->GetComponentByClass<UCharacterMovementComponent>())
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->AddImpulse(Repel , true);
	}

	const UPXCustomSettings* CustomSettings = GetDefault<UPXCustomSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CustomSettings)
	UPXCameraResourceDataAsset* CameraShakeDataAsset = CustomSettings->CameraResourceDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CameraShakeDataAsset)
	UPXForceFeedbackEffectDataAsset* ForceFeedbackEffectDataAsset = CustomSettings->ForceFeedbackEffectDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ForceFeedbackEffectDataAsset)
	const UPXResourceDataAsset* ResourceDataAsset = CustomSettings->ResourceDataAsset.LoadSynchronous();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(ResourceDataAsset)
	
	if (!GetOwner()->Implements<UFight_Interface>()) return;

	APawn* OwnerPawn = GetOwner<APawn>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(OwnerPawn)
	
	FVector ownerLocation = OwnerPawn->GetActorLocation();
	// 如果受力大于300则属于强效击退
	const UWorld* World = GetWorld();
	if (World && Repel.Size() > MinPowerRepelForceValue)
	{
		if (bOwnerIsPlayer && OwnerPawn->GetController() && OwnerPawn->GetController()->IsLocalController())
		{
			UGameplayStatics::PlayWorldCameraShake(World, CameraShakeDataAsset->PlayerHitedShake_Powerful,
				ownerLocation, 0, 500, 0, true);
			UPXGameplayStatics::SpawnForceFeedbackAttached(OwnerPawn, ForceFeedbackEffectDataAsset->HugeKnockback.LoadSynchronous(), GetOwner()->GetRootComponent());
		}
		else
		{
			UGameplayStatics::PlayWorldCameraShake(World, CameraShakeDataAsset->PlayerHitedShake_Powerful,
				ownerLocation, 0, 500, 0.2, true);
		}
		IFight_Interface::Execute_PowerRepulsion(GetOwner(), Repel.Size());

		UNiagaraComponent* NS_HitSmoke = UNiagaraFunctionLibrary::SpawnSystemAttached(ResourceDataAsset->NS_HitSmoke.LoadSynchronous(),
			OwnerPawn->GetRootComponent(), 
			FName(""),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::Type::KeepRelativeOffset,
			true,
			true,
			ENCPoolMethod::None,
			true
		);
		NS_HitSmoke->SetVariableFloat(FName("Power"), Repel.Size() / 1.5);
	}
	else
	{
		if (bOwnerIsPlayer && OwnerPawn->GetController() && OwnerPawn->GetController()->IsLocalController())
		{
			UGameplayStatics::PlayWorldCameraShake(World, CameraShakeDataAsset->PlayerHitedShake,
				ownerLocation, 0, 500, 0, true);
			UPXGameplayStatics::SpawnForceFeedbackAttached(OwnerPawn, ForceFeedbackEffectDataAsset->Knockback.LoadSynchronous(), GetOwner()->GetRootComponent());
		}
		else
		{
			UGameplayStatics::PlayWorldCameraShake(World, CameraShakeDataAsset->MonsterHitedShake,
				ownerLocation, 0, 500, 1, true);
		}
	}
	
}

float UHealthComponent::CalHurtDuration(int32 ChangedHP)
{
	if (ChangedHP > GetMaxHP() * 0.1)
	{
		return 0.18f * (1 + ChangedHP / GetMaxHP());
	}

	return 0.1f;
}

#undef LOCTEXT_NAMESPACE
