// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Components/StateComponent.h"

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
#include "Core/PXGameState.h"
#include "GAS/PXASComponent.h"
#include "Settings/PXSettingsShared.h"
#include "Settings/Config/PXCameraSourceDataAsset.h"
#include "Settings/Config/PXCustomSettings.h"
#include "Settings/Config/PXForceFeedbackEffectDataAsset.h"
#include "Settings/Config/PXResourceDataAsset.h"
#include "Subsystems/TimerManagerFuncLib.h"
#include "Utilitys/DebugFuncLab.h"
#include "Utilitys/PXGameplayStatics.h"

#define LOCTEXT_NAMESPACE "PX"

class ULegacyCameraShake;

int UStateComponent::CalAcceptDamage(int InDamage, AActor* Maker)
{
	AActor* Owner = GetOwner();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(Owner, InDamage)
	if (!Owner->Implements<UFight_Interface>()) return InDamage;
	if (GetHP() <= 0) return 0;
	
	int DamagePlus = 0;
	if (Maker && Maker->Implements<UFight_Interface>())
	{
		DamagePlus = IFight_Interface::Execute_DamagePlus(Maker, InDamage, Owner);
	}
	
	return InDamage + DamagePlus;
}

void UStateComponent::OnHurtInvulnerable()
{
	SetInvulnerable(false);
	FName TimerName = FName( GetReadableName()+ "_OnHurtInvulnerable");
	
	UTimerManagerFuncLib::SetRetriggerableDelay(GetWorld(), TimerName,
[WeakThis = TWeakObjectPtr(this)]
	{
		if (!WeakThis.IsValid()) return;
		WeakThis->InvulnerableForDuration(WeakThis->InvulnerableDuration);
	}, OnHurtInvulnerableDelay);
}

void UStateComponent::InvulnerableForDuration(float duration)
{
	SetInvulnerable(true);
	UTimerManagerFuncLib::SetDelay(GetWorld(), [WeakThis = TWeakObjectPtr(this)]
	{
		if (!WeakThis.IsValid()) return;
		WeakThis->SetInvulnerable(false);
	}, duration);
}

void UStateComponent::FlashForDuration(float Duration, float FlashRate, FLinearColor FlashColor) 
{
	UWorld* World = GetWorld();
	if (!World) return;

	int FlashTimes = Duration / FlashRate;
	if (FlashTimes % 2 == 1) FlashTimes++;
	
	FName TimerName = FName("UHealthComponent::FlashForDuration" + FGuid::NewGuid().ToString());
	UTimerManagerFuncLib::SetDelayLoop(World, TimerName,
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

FVector UStateComponent::CalRepel(FVector& IncomeRepel, const AActor* Instigator) const
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
UStateComponent::UStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UStateComponent::SetInvulnerable(const bool v)
{
	bInvulnerable = v;
}

void UStateComponent::ModifyMaxHP(int32 value, const EStatChange ChangeType, const bool current)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;

	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)

	value = FMath::Max(value, 0);
	
	int CurrentHealth = CachedASC->GetAttributeValue(EPXAttribute::HP);
	int MaxHealth = CachedASC->GetAttributeValue(EPXAttribute::CurMaxHP);
	
	if (ChangeType == EStatChange::Increase)
	{
		CachedASC->SetAttributeValue(EPXAttribute::BasicMaxHP, MaxHealth + value);
		if (current)
		{
			SetHP(CurrentHealth + value);
		}

	}
	else if (ChangeType == EStatChange::Decrease)
	{
		CachedASC->SetAttributeValue(EPXAttribute::BasicMaxHP, FMath::Max(0, MaxHealth - value));
		if (current)
		{
			SetHP(FMath::Max(0, CurrentHealth - value));
		}
	}
	else if (ChangeType == EStatChange::Reset)
	{
		CachedASC->SetAttributeValue(EPXAttribute::BasicMaxHP, value);
		if (current || value < CurrentHealth)
		{
			SetHP(value);
		}
	}
}

void UStateComponent::SetHP(const int32 value)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)

	int CurrentHealth = FMath::Clamp(value, 0, GetMaxHP());
	CachedASC->SetAttributeValue(EPXAttribute::HP, CurrentHealth);
}

void UStateComponent::IncreaseHP(int32 value, AActor* Instigator)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	const UPXAttributeSet* PixelAS = CachedASC->GetSet<UPXAttributeSet>();
	if (!IsValid(PixelAS)) return;

	// 应该在 AttributeSet 的 Pre事件中控制生命值范围
	CachedASC->SetNumericAttributeBase(UPXAttributeSet::GetHPAttribute(), value);
}


// Called when the game starts
void UStateComponent::BeginPlay()
{
	Super::BeginPlay();

	bOwnerIsPlayer = IFight_Interface::Execute_GetOwnCamp(GetOwner()).HasTag(FGameplayTag::RequestGameplayTag(FName("Player")));

	
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		CachedASC = Cast<UPXASComponent>(ASI->GetAbilitySystemComponent());
	}
	
	OnHPChanged.AddDynamic(this, &UStateComponent::Event_OnHPChanged);
}

void UStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);


	OnHPChanged.RemoveAll(this);
}


// Called every frame
void UStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}



void UStateComponent::Event_OnHPChanged(int OldValue, int NewValue)
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

int UStateComponent::GetHP()
{
	return CachedASC ? CachedASC->GetAttributeValue(EPXAttribute::HP) : 0;
}

int UStateComponent::GetMaxHP()
{
	return CachedASC ? CachedASC->GetAttributeValue(EPXAttribute::CurMaxHP) : 100;
}

float UStateComponent::GetHPPercent()
{
	return float(GetHP()) / float(GetMaxHP());
}

void UStateComponent::DecreaseHP(int Damage, AActor* Maker, const FVector KnockbackForce, bool bForce)
{
	if (Damage <= 0) return;
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->Implements<UFight_Interface>()) return;
	
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC);

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

	int preHealth = CachedASC->GetAttributeValue(EPXAttribute::HP);
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
	CachedASC->SetAttributeValue(EPXAttribute::HP, CurrentHealth);
	// CachedASC->SetNumericAttributeBase(UPXAttributeSet::GetHPAttribute(), CurrentHealth);
}

void UStateComponent::KnockBack(FVector Repel, AActor* Maker)
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

float UStateComponent::CalHurtDuration(int32 ChangedHP)
{
	if (ChangedHP > GetMaxHP() * 0.1)
	{
		return 0.18f * (1 + ChangedHP / GetMaxHP());
	}

	return 0.1f;
}

int32 UStateComponent::CalEPCustomInFact(int32 Amount)
{
	APXGameState* GS = UPXGameplayStatics::GetGameState(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GS, Amount)
	
	return FMath::RoundToInt(Amount * (GS->GetEPConsumePlusPercent() + 1.0f));
}

bool UStateComponent::DecreaseEP(int32 Amount)
{
	APXGameState* GS = UPXGameplayStatics::GetGameState(GetWorld());
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN_VAL(GS, false)
	
	int32 Consume = CalEPCustomInFact(Amount);

	if (!IsEPEnough(Consume, true)) return false;

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
		{
			float CurValue = PixelAS->GetEP();
			
			if (CurValue >= Consume)
			{
				CurValue -= Consume;
				TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetEPAttribute(), CurValue);
				return true;
			}
		}
	}

	return false;
}

void UStateComponent::IncreaseEP(int32 Amount)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(CachedASC)
	float NewValue = FMath::Min(CachedASC->GetAttributeValue(EPXAttribute::EP) + Amount, CachedASC->GetAttributeValue(EPXAttribute::CurMaxEP));
	CachedASC->SetAttributeValue(EPXAttribute::EP, NewValue);
}

void UStateComponent::SetEP(int32 NewValue)
{
	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetEPAttribute(), NewValue);
		}
	}
}

int32 UStateComponent::GetCurrentEP()
{
	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
			{
				return PixelAS->GetEP();
			}
		}
	}
	return 0;
}

int32 UStateComponent::GetMaxEP()
{
	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
			{
				return PixelAS->GetCurMaxEP();
			}
		}
	}
	return 100;
}

void UStateComponent::SetMaxEP(int32 NewValue)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (!IsValid(TargetASC)) return;
	const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>();
	if (!IsValid(PixelAS)) return;

	NewValue = FMath::Max(NewValue, 0);
	TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetCurMaxEPAttribute(), NewValue);
	TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetEPAttribute(), NewValue);
}

bool UStateComponent::IsEPEnough(int32 Cost, bool bNeedTip)
{
	Cost = CalEPCustomInFact(Cost);

	if (const AActor* TargetActor = GetOwner())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor))
		{
			if (const UPXAttributeSet* PixelAS = TargetASC->GetSet<UPXAttributeSet>())
			{
				return PixelAS->GetEP() >= Cost;
			}
		}
	}

	if (bNeedTip)
	{
		UCommonFuncLib::SpawnCenterTip(LOCTEXT("EP_NOT_ENOUGH", "体力不足"),
			FLinearColor::White, FVector2D(0, 300));
	}

	return false;
}

#undef LOCTEXT_NAMESPACE
