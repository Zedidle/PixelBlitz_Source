// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Components/HealthComponent.h"

#include "GameDelegates.h"
#include "PaperFlipbookComponent.h"
#include "Character/BasePXCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/AttributeSet/PXAttributeSet.h"
#include "Interfaces/Fight_Interface.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/CameraShakeSettings.h"
#include "Utilitys/CommonFuncLib.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemGlobals.h"
#include "Basic/PXGameState.h"
#include "GAS/AttributeSet/PXAttributeSet.h"

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
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
		[this]
		{
			InvulnerableForDuration(InvulnerableDuration);
		});

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1, false, OnHurtInvulnerableDelay);
}

void UHealthComponent::InvulnerableForDuration(float duration)
{
	bInvulnerable = true;
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
		[this, &TimerHandle]
		{
			bInvulnerable = false;
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1, false, duration);
}

void UHealthComponent::FlashForDuration(FLinearColor color, float duration, bool force) 
{
	if (bInvulnerable && !force) return;

	FlashColor = color;
	PreHurtTime = GetWorld()->GetTimeSeconds();
	FlashDuration = duration;
	
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
		[this]
		{
			if (!IsValid(GetOwner()))
			{
				if (UWorld* world = GetWorld())
				{
					world->GetTimerManager().ClearTimer(FlashTimerHandle);
				}
				return;
			}
			
			if (IsValid(GetWorld()) && GetWorld()->GetTimeSeconds() - PreHurtTime > FlashDuration)
			{
				if (UPaperFlipbookComponent* PF = GetOwner()->GetComponentByClass<UPaperFlipbookComponent>())
				{
					PF->SetSpriteColor(FLinearColor::White);
				}
				GetWorld()->GetTimerManager().ClearTimer(FlashTimerHandle);
				return;
			}
			
			bFlashing = !bFlashing;
			if (UPaperFlipbookComponent* PF = GetOwner()->GetComponentByClass<UPaperFlipbookComponent>())
			{
				PF->SetSpriteColor(bFlashing ? FlashColor : FLinearColor::White);
			}
		});
	GetWorld()->GetTimerManager().SetTimer(FlashTimerHandle, TimerDel, 0.2, true);
	
}

FVector UHealthComponent::GetRepel(FVector IncomeRepel, const AActor* Instigator) const
{
	if (!IsValid(Instigator)) return IncomeRepel;
	IncomeRepel = (1.0f - RepelResistancePercent) * IncomeRepel;
	IncomeRepel.X = IncomeRepel.X > 0 ? FMath::Max(IncomeRepel.X - RepelResistance.X, 0) : FMath::Min(IncomeRepel.X + RepelResistance.X, 0);
	IncomeRepel.Y = IncomeRepel.Y > 0 ? FMath::Max(IncomeRepel.Y - RepelResistance.Y, 0) : FMath::Min(IncomeRepel.Y + RepelResistance.Y, 0);
	
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
			int preHealth = PixelAS->GetHP();
			int CurrentHealth = FMath::Clamp(value, 0, PixelAS->GetMaxHP());
			int changedValue = CurrentHealth - preHealth;
			TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetHPAttribute(), CurrentHealth);
		
			EStatChange changeType = changedValue > 0 ? EStatChange::Increase : EStatChange::Decrease;
			if (broadcast)
			{
				OnHPChanged.Broadcast(CurrentHealth, changedValue, changeType, nullptr, false);
			}
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
		OnHPChanged.Broadcast(CurrentHealth, CurrentHealth - preHealth, EStatChange::Increase, Instigator, false);
	}
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UWorld* world = GetWorld();
	if (!IsValid(world)) return;
	if (TimerHandle_EffectBySeconds.IsValid() && world->GetTimerManager().IsTimerActive(TimerHandle_EffectBySeconds))
	{
		world->GetTimerManager().ClearTimer(TimerHandle_EffectBySeconds);
	}
	if (FlashTimerHandle.IsValid() && world->GetTimerManager().IsTimerActive(FlashTimerHandle))
	{
		world->GetTimerManager().ClearTimer(FlashTimerHandle);
	}
	OnHPChanged.RemoveAll(this);
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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

void UHealthComponent::DecreaseHP(int Damage, const FVector KnockbackForce, AActor* Maker, bool bForce,
                                      bool bCauseInvul, bool bInner)
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

	// 天气影响的伤害增幅, 记录在 GameState的天气数值
	if (APXGameState* GS = Cast<APXGameState>(UGameplayStatics::GetGameState(Owner)))
	{
		Damage += Damage * GS->WeatherEffectData.DamageAddPercent;
	}

	
	int OutDamage = Damage;
	if (!bInner)
	{
		IFight_Interface::Execute_OnBeAttacked(Owner, Maker, Damage, OutDamage);
		KnockBack(KnockbackForce * (OutDamage / Damage), Maker);
	}
	
	if (OutDamage <= 0) return;

	int calDamage = CalAcceptDamage(OutDamage, Maker);
	calDamage = IFight_Interface::Execute_OnDefendingHit(Owner, calDamage);

	int preHealth = PixelAS->GetHP();
	int CurrentHealth = FMath::Max(preHealth - calDamage, 0);
	int changedValue = FMath::Abs(preHealth - CurrentHealth);
	if (changedValue <= 0) return;

	TargetASC->SetNumericAttributeBase(UPXAttributeSet::GetHPAttribute(), CurrentHealth);
	OnHPChanged.Broadcast(CurrentHealth, changedValue, EStatChange::Decrease, Maker, bInner);

	// 触发受伤闪烁
	if (!bInner)
	{
		FlashForDuration();
	}

	// 触发无敌帧
	if (bCauseInvul)
	{
		OnHurtInvulnerable();
	}
}

void UHealthComponent::KnockBack(FVector Repel, AActor* Instigator)
{
	if (bInRock)
	{
		UCommonFuncLib::SpawnFloatingTextDefault(TEXT("Buff/BuffEffectText"), TEXT("Buff_InRock"),
			GetOwner()->GetActorLocation(), FColor::Purple, FVector2D(0.8, 0.8));
		return;
	}

	if (bInvulnerable) return;

	Repel = GetRepel(Repel, Instigator) * KnockBackMultiplier;
	
	if (UCharacterMovementComponent* MovementComponent = GetOwner()->GetComponentByClass<UCharacterMovementComponent>())
	{
		MovementComponent->AddImpulse(Repel , true);
	}
	else if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent()))
	{
		if (PrimitiveComponent->GetCollisionEnabled() != ECollisionEnabled::QueryAndPhysics) return;
		PrimitiveComponent->AddImpulse(Repel, FName(), true);
	}
	else
	{
		return;
	}

	const UCameraShakeSettings* CameraShakeSettings = GetDefault<UCameraShakeSettings>();

	if (!GetOwner()->Implements<UFight_Interface>()) return;
	bool bOwnerIsPlayer = IFight_Interface::Execute_GetOwnCamp(GetOwner()).HasTag(FGameplayTag::RequestGameplayTag(FName("Player")));
	FVector ownerLocation = GetOwner()->GetActorLocation();
	// 如果受力大于300则属于强效击退
	const UWorld* World = GetWorld();
	if (World && Repel.Size() > MinPowerRepelForceValue)
	{
		if (bOwnerIsPlayer)
		{
			UGameplayStatics::PlayWorldCameraShake(World, CameraShakeSettings->PlayerHitedShake_Powerful,
				ownerLocation, 0, 500, 0, true);
		}
		else
		{
			UGameplayStatics::PlayWorldCameraShake(World, CameraShakeSettings->PlayerHitedShake_Powerful,
				ownerLocation, 0, 500, 0.2, true);
		}
		IFight_Interface::Execute_PowerRepulsion(GetOwner(), Repel.Size());
	}
	else
	{
		if (bOwnerIsPlayer)
		{
			UGameplayStatics::PlayWorldCameraShake(World, CameraShakeSettings->PlayerHitedShake,
				ownerLocation, 0, 500, 0, true);
		}
		else
		{
			UGameplayStatics::PlayWorldCameraShake(World, CameraShakeSettings->MonsterHitedShake,
				ownerLocation, 0, 500, 1, true);
		}
	}
	
}
