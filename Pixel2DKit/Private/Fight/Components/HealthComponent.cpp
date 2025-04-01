// Fill out your copyright notice in the Description page of Project Settings.


#include "Fight/Components/HealthComponent.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/Fight_Interface.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/CameraShakeSettings.h"

class ULegacyCameraShake;

int UHealthComponent::CalAcceptDamage(int initDamage, AActor* instigator)
{
	AActor* owner = GetOwner();
	if (!owner) return initDamage;
	
	int tmpDamageAdd = 0;
	if (instigator && instigator->Implements<UFight_Interface>())
	{
		tmpDamageAdd = IFight_Interface::Execute_DamagePlus(instigator, initDamage, GetOwner());
	}

	if (!owner->Implements<UFight_Interface>()) return initDamage;

	FGameplayTagContainer ownerTags = IFight_Interface::Execute_GetOwnCamp(owner);
	if (ownerTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Player"))))
	{
		return tmpDamageAdd + initDamage;
	}

	if (GetCurrentHealth() <= 0) return 0;
	
	// 判断是否为BOSS
	if (ownerTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Enemy.BOSS"))))
	{
		// 技能【巨人杀手】处理，AbilityComponent_CPP
		float xxx = 1.5;
		initDamage = FMath::RoundToInt(xxx * initDamage);
	}
	
	return initDamage + tmpDamageAdd;
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

void UHealthComponent::ModifyMaxHealth(const int32 value, const EStatChange ChangeType, const bool current)
{
	if (ChangeType == EStatChange::Increase)
	{
		MaxHealth += value;
		if (current)
		{
			SetHealth(CurrentHealth + value, false);
		}
	}

	if (ChangeType == EStatChange::Decrease)
	{
		MaxHealth -= FMath::Max(0, value);
		if (current)
		{
			SetHealth(CurrentHealth - value, false);
		}
	}

	if (ChangeType == EStatChange::Reset)
	{
		MaxHealth = value;
		if (current)
		{
			SetHealth(value, false);
		}
		else
		{
			SetHealth(FMath::Clamp(CurrentHealth, 0, MaxHealth), false);
		}
	}

	OnMaxHealthChanged.Broadcast(MaxHealth, ChangeType);
}

void UHealthComponent::SetHealth(const int32 value, const bool broadcast)
{
	int preHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(value, 0, MaxHealth);
	int changedValue = CurrentHealth - preHealth;
	EStatChange changeType = changedValue > 0 ? EStatChange::Increase : EStatChange::Decrease;
	if (broadcast)
	{
		OnHealthChanged.Broadcast(CurrentHealth, changedValue, changeType, nullptr, false);
	}
}

void UHealthComponent::SetEffectBySeconds(const int32 value, const FGameplayTag Tag)
{
	if (value == 0)
	{
		Tag2EffectHealthPoint.Remove(Tag);
	}
	else
	{
		Tag2EffectHealthPoint.Add(Tag, value);
		SetActivateHealthEffectBySeconds();
	}
}


void UHealthComponent::IncreaseHealth(const int value, AActor* Instigator)
{
	int preHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(value + CurrentHealth, 0, MaxHealth);
	if (CurrentHealth - preHealth > 0)
	{
		OnHealthChanged.Broadcast(CurrentHealth, CurrentHealth - preHealth, EStatChange::Increase, Instigator, false);
	}
}

void UHealthComponent::SetActivateHealthEffectBySeconds(const bool activate)
{
	UWorld* world = GetWorld();
	if (!IsValid(world)) return;
	
	if (!activate)
	{
		if (TimerHandle_EffectBySeconds.IsValid() && GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_EffectBySeconds))
		{
			world->GetTimerManager().ClearTimer(TimerHandle_EffectBySeconds);
		}
		return;
	}

	if (world->GetTimerManager().IsTimerActive(TimerHandle_EffectBySeconds)) return;
	
	FTimerDelegate TimerDel = FTimerDelegate::CreateLambda(
	[this]
	{
		UWorld* world = GetWorld();
		if (!IsValid(world)) return;
		
		if (Tag2EffectHealthPoint.IsEmpty())
		{
			world->GetTimerManager().ClearTimer(TimerHandle_EffectBySeconds);
			return;
		}
			
		for (auto& effect : Tag2EffectHealthPoint)
		{
			if (effect.Value > 0)
			{
				IncreaseHealth(effect.Value, GetOwner());
			}
			else
			{
				DecreaseHealth(effect.Value, FVector(0), GetOwner(), true, false, true);
			}
		}
	});
	world->GetTimerManager().SetTimer(TimerHandle_EffectBySeconds, TimerDel, 1.0f, true);
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
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int UHealthComponent::GetCurrentHealth()
{
	return CurrentHealth;
}

int UHealthComponent::GetMaxHealth()
{
	return MaxHealth;
}

float UHealthComponent::GetHealthPercent()
{
	return float(CurrentHealth) / float(MaxHealth);
}

void UHealthComponent::DecreaseHealth(int Damage, const FVector KnockbackForce, AActor* Instigator, bool bForce,
                                      bool bCauseInvul, bool bInner)
{
	if (Damage <= 0) return;
	AActor* owner = GetOwner();
	if (!owner->Implements<UFight_Interface>()) return;

	// 玩家受伤逻辑
	FGameplayTag PlayerTag = FGameplayTag::RequestGameplayTag(FName("Player"));
	if (IFight_Interface::Execute_GetOwnCamp(owner).HasTag(PlayerTag))
	{
		// 天气影响的伤害增幅
		float weatherAddPercent = 0.1;
		Damage = Damage + Damage * weatherAddPercent;
		if (bInvulnerable && !bForce)
		{
			if (owner->Implements<UFight_Interface>())
			{
				IFight_Interface::Execute_OnBeAttacked_Invulnerable(owner);
			}
			return;
		}
		if (!bInner)
		{
			if (owner->Implements<UFight_Interface>())
			{
				if (IFight_Interface::Execute_OnBeAttacked(owner, Instigator, Damage))
				{
					KnockBack(KnockbackForce * 0.9, Instigator);
					return;
				}
			}
		}
	}

	// 怪物受伤逻辑
	FGameplayTag EnemyTag = FGameplayTag::RequestGameplayTag(FName("Enemy"));
	if (IFight_Interface::Execute_GetOwnCamp(owner).HasTag(EnemyTag))
	{
		if (bInvulnerable && !bForce)
		{
			IFight_Interface::Execute_OnBeAttacked_Invulnerable(owner);
		}
	}

	int calDamage = CalAcceptDamage(Damage, Instigator);
	calDamage = IFight_Interface::Execute_OnDefendingHit(owner, calDamage);

	int preHealth = CurrentHealth;
	CurrentHealth = FMath::Max(CurrentHealth - calDamage, 0);
	int changedValue = FMath::Abs(preHealth - CurrentHealth);
	if (changedValue > 0)
	{
		OnHealthChanged.Broadcast(CurrentHealth, changedValue, EStatChange::Decrease, Instigator, bInner);
	}

	if (bCauseInvul)
	{
		OnHurtInvulnerable();
	}
	if (!bInner)
	{
		FlashForDuration();
		KnockBack(KnockbackForce, Instigator);
	}
}

void UHealthComponent::KnockBack(FVector Repel, AActor* Instigator)
{
	if (bInRock)
	{
		// **AbilityComponent** SpawnFloatingText 击退无效
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
